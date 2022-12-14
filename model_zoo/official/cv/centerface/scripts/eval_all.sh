#!/bin/sh
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

root=$PWD
save_path=$root/output/centerface/
ground_truth_path=$1
#for i in $(seq start_epoch end_epoch+1)
for i in $(seq 89 200)
do
    python ../dependency/evaluate/eval.py --pred=$save_path$i --gt=$ground_truth_path >> log_eval_all.txt 2>&1 &
    sleep 10
done
wait
