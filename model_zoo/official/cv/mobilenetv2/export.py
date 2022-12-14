# Copyright 2020 Huawei Technologies Co., Ltd
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
"""
mobilenetv2 export file.
"""
import numpy as np
from mindspore import Tensor, export, context
from src.models import define_net, load_ckpt
from src.utils import set_context
from src.model_utils.config import config
from src.model_utils.device_adapter import get_device_id, get_device_num, get_rank_id
from src.model_utils.moxing_adapter import moxing_wrapper


config.device_id = get_device_id()
config.rank_id = get_rank_id()
config.rank_size = get_device_num()
config.run_distribute = config.rank_size > 1.
config.batch_size = config.batch_size_export
config.is_training = config.is_training_export

context.set_context(mode=context.GRAPH_MODE, device_target=config.platform)
if config.platform == "Ascend":
    context.set_context(device_id=get_device_id())

def modelarts_process():
    pass

@moxing_wrapper(pre_process=modelarts_process)
def export_mobilenetv2():
    """ export_mobilenetv2 """
    print('\nconfig: \n', config)
    set_context(config)
    _, _, net = define_net(config, config.is_training)

    load_ckpt(net, config.ckpt_file)
    input_shp = [config.batch_size, 3, config.image_height, config.image_width]
    input_array = Tensor(np.random.uniform(-1.0, 1.0, size=input_shp).astype(np.float32))
    export(net, input_array, file_name=config.file_name, file_format=config.file_format)

if __name__ == '__main__':
    export_mobilenetv2()
