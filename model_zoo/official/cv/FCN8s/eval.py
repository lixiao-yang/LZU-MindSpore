# Copyright 2021 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================
"""eval FCN8s."""


import numpy as np
import cv2
from PIL import Image
from mindspore import Tensor
import mindspore.common.dtype as mstype
import mindspore.nn as nn
from mindspore import context
from mindspore.train.serialization import load_checkpoint, load_param_into_net
from src.nets.FCN8s import FCN8s
from src.model_utils.config import config
from src.model_utils.moxing_adapter import moxing_wrapper
from src.model_utils.device_adapter import get_device_id


def cal_hist(a, b, n):
    k = (a >= 0) & (a < n)
    return np.bincount(n * a[k].astype(np.int32) + b[k], minlength=n ** 2).reshape(n, n)


def resize_long(img, long_size=513):
    h, w, _ = img.shape
    if h > w:
        new_h = long_size
        new_w = int(1.0 * long_size * w / h)
    else:
        new_w = long_size
        new_h = int(1.0 * long_size * h / w)
    imo = cv2.resize(img, (new_w, new_h))
    return imo


class BuildEvalNetwork(nn.Cell):
    def __init__(self, network):
        super(BuildEvalNetwork, self).__init__()
        self.network = network
        self.softmax = nn.Softmax(axis=1)

    def construct(self, input_data):
        output = self.network(input_data)
        output = self.softmax(output)
        return output


def pre_process(configs, img_, crop_size=512):
    # resize
    img_ = resize_long(img_, crop_size)
    resize_h, resize_w, _ = img_.shape

    # mean, std
    image_mean = np.array(configs.image_mean)
    image_std = np.array(configs.image_std)
    img_ = (img_ - image_mean) / image_std

    # pad to crop_size
    pad_h = crop_size - img_.shape[0]
    pad_w = crop_size - img_.shape[1]
    if pad_h > 0 or pad_w > 0:
        img_ = cv2.copyMakeBorder(img_, 0, pad_h, 0, pad_w, cv2.BORDER_CONSTANT, value=0)

    # hwc to chw
    img_ = img_.transpose((2, 0, 1))
    return img_, resize_h, resize_w


def eval_batch(configs, eval_net, img_lst, crop_size=512, flip=True):
    result_lst = []
    batch_size = len(img_lst)
    batch_img = np.zeros((configs.eval_batch_size, 3, crop_size, crop_size), dtype=np.float32)
    resize_hw = []
    for l in range(batch_size):
        img_ = img_lst[l]
        img_, resize_h, resize_w = pre_process(configs, img_, crop_size)
        batch_img[l] = img_
        resize_hw.append([resize_h, resize_w])

    batch_img = np.ascontiguousarray(batch_img)
    net_out = eval_net(Tensor(batch_img, mstype.float32))
    net_out = net_out.asnumpy()

    if flip:
        batch_img = batch_img[:, :, :, ::-1]
        net_out_flip = eval_net(Tensor(batch_img, mstype.float32))
        net_out += net_out_flip.asnumpy()[:, :, :, ::-1]

    for bs in range(batch_size):
        probs_ = net_out[bs][:, :resize_hw[bs][0], :resize_hw[bs][1]].transpose((1, 2, 0))
        ori_h, ori_w = img_lst[bs].shape[0], img_lst[bs].shape[1]
        probs_ = cv2.resize(probs_.astype(np.float32), (ori_w, ori_h))
        result_lst.append(probs_)

    return result_lst


def eval_batch_scales(configs, eval_net, img_lst, scales,
                      base_crop_size=512, flip=True):
    sizes_ = [int((base_crop_size - 1) * sc) + 1 for sc in scales]
    probs_lst = eval_batch(configs, eval_net, img_lst, crop_size=sizes_[0], flip=flip)
    print(sizes_)
    for crop_size_ in sizes_[1:]:
        probs_lst_tmp = eval_batch(configs, eval_net, img_lst, crop_size=crop_size_, flip=flip)
        for pl, _ in enumerate(probs_lst):
            probs_lst[pl] += probs_lst_tmp[pl]

    result_msk = []
    for i in probs_lst:
        result_msk.append(i.argmax(axis=2))
    return result_msk


@moxing_wrapper()
def net_eval():
    context.set_context(mode=context.GRAPH_MODE, device_target=config.device_target, device_id=get_device_id(),
                        save_graphs=False)

    # data list
    with open(config.data_lst) as f:
        img_lst = f.readlines()

    net = FCN8s(n_class=config.num_classes)

    # load model
    param_dict = load_checkpoint(config.ckpt_file)
    load_param_into_net(net, param_dict)

    # evaluate
    hist = np.zeros((config.num_classes, config.num_classes))
    batch_img_lst = []
    batch_msk_lst = []
    bi = 0
    image_num = 0
    for i, line in enumerate(img_lst):

        img_name = line.strip('\n')
        data_root = config.data_root
        img_path = data_root + '/JPEGImages/' + str(img_name) + '.jpg'
        msk_path = data_root + '/SegmentationClass/' + str(img_name) + '.png'

        img_ = np.array(Image.open(img_path), dtype=np.uint8)
        msk_ = np.array(Image.open(msk_path), dtype=np.uint8)

        batch_img_lst.append(img_)
        batch_msk_lst.append(msk_)
        bi += 1
        if bi == config.eval_batch_size:
            batch_res = eval_batch_scales(config, net, batch_img_lst, scales=config.scales,
                                          base_crop_size=config.crop_size, flip=config.flip)
            for mi in range(config.eval_batch_size):
                hist += cal_hist(batch_msk_lst[mi].flatten(), batch_res[mi].flatten(), config.num_classes)

            bi = 0
            batch_img_lst = []
            batch_msk_lst = []
            print('processed {} images'.format(i+1))
        image_num = i

    if bi > 0:
        batch_res = eval_batch_scales(config, net, batch_img_lst, scales=config.scales,
                                      base_crop_size=config.crop_size, flip=config.flip)
        for mi in range(bi):
            hist += cal_hist(batch_msk_lst[mi].flatten(), batch_res[mi].flatten(), config.num_classes)
        print('processed {} images'.format(image_num + 1))

    print(hist)
    iu = np.diag(hist) / (hist.sum(1) + hist.sum(0) - np.diag(hist))
    print('per-class IoU', iu)
    print('mean IoU', np.nanmean(iu))


if __name__ == '__main__':
    net_eval()
