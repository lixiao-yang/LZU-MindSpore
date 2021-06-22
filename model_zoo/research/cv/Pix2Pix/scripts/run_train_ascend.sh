#!/bin/bash
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

echo "====================================================================================================================="
echo "Please run the train as: "
echo "python train.py device_target device_id dataset_size train_data_dir"
echo "for example: python train.py --device_target Ascend --device_id 0 --dataset_size 400 --train_data_dir ./facades/train"
echo "====================================================================================================================="

python train.py --device_target Ascend --device_id 0 --dataset_size 400 --train_data_dir ./facades/train
