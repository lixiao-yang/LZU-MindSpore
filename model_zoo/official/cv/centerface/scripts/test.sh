#!/bin/bash
# Copyright 2020-21 Huawei Technologies Co., Ltd
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

if [ $# -gt 7 ]
then
    echo "Usage: sh test.sh [DEVICE_TARGET] [MODEL_PATH] [DATASET] [GROUND_TRUTH_MAT] [SAVE_PATH] [DEVICE_ID] [CKPT]"
    echo "   or: sh test.sh [DEVICE_TARGET] [MODEL_PATH] [DATASET] [GROUND_TRUTH_MAT] [SAVE_PATH] [DEVICE_ID]"
    echo "   or: sh test.sh [DEVICE_TARGET] [MODEL_PATH] [DATASET] [GROUND_TRUTH_MAT] [SAVE_PATH]"
    echo "   or: sh test.sh [DEVICE_TARGET] [MODEL_PATH] [DATASET] [GROUND_TRUTH_MAT]"
    echo "   or: sh test.sh [DEVICE_TARGET] [MODEL_PATH] [DATASET]"
    echo "   or: sh test.sh [DEVICE_TARGET] [MODEL_PATH]"
    echo "   or: sh test.sh [DEVICE_TARGET]"
    echo "   or: sh test.sh "
exit 1
fi

get_real_path(){
  if [ "${1:0:1}" == "/" ]; then
    echo "$1"
  else
    echo "$(realpath -m $PWD/$1)"
  fi
}

current_exec_path=$(pwd)
echo ${current_exec_path}

dirname_path=$(dirname "$(pwd)")
echo ${dirname_path}

SCRIPT_NAME='test.py'

ulimit -c unlimited

root=${current_exec_path} # your script path
model_path=$root/model/
dataset_root=$root/dataset
dataset_path=$dataset_root/centerface/images/val/images/
ground_truth_mat=$dataset_root/centerface/ground_truth/val.mat
save_path=$root/output/centerface/
device_target="Ascend"
device_id=0
ckpt="0-140_221620.ckpt" # the model saved for epoch=140

if [ $# -ge 1 ]
then
    device_target="$1"
    if [ "$device_target" != "Ascend" ] && [ "$device_target" != "GPU" ]
    then
        echo "error: device_target=$device_target is not a valid option (Ascend or GPU)"
    exit 1
    fi
fi

if [ $# -ge 2 ]
then
    model_path=$(get_real_path $2)
    if [ ! -d $model_path ]
    then
        echo "error: model_path=$model_path is not a file"
    exit 1
    fi
fi

if [ $# -ge 3 ]
then
    dataset_path=$(get_real_path $3)
    if [ ! -d $dataset_path ]
    then
        echo "error: dataset_path=$dataset_path is not a file"
    exit 1
    fi
fi

if [ $# -ge 4 ]
then
    ground_truth_mat=$(get_real_path $4)
    if [ ! -f $ground_truth_mat ]
    then
        echo "error: ground_truth_mat=$ground_truth_mat is not a file"
    exit 1
    fi
fi

if [ $# -ge 5 ]
then
    save_path=$(get_real_path $5)
    if [ ! -d $save_path ]
    then
        echo "error: save_path=$save_path is not a file"
    exit 1
    fi
fi

if [ $# -ge 6 ]
then
    device_id=$6
fi

if [ $# == 7 ]
then
    ckpt=$7
fi

echo $model_path
echo $dataset_path
echo $ground_truth_mat
echo $save_path

export PYTHONPATH=${dirname_path}:$PYTHONPATH
export RANK_SIZE=1

echo 'start testing'
rm -rf ${current_exec_path}/device_test$device_id
echo 'start rank '$device_id
mkdir ${current_exec_path}/device_test$device_id
cd ${current_exec_path}/device_test$device_id || exit
export RANK_ID=0
dev=`expr $device_id + 0`
export DEVICE_ID=$dev
python ${dirname_path}/${SCRIPT_NAME} \
    --is_distributed=0 \
    --data_dir=$dataset_path \
    --test_model=$model_path \
    --ground_truth_mat=$ground_truth_mat \
    --save_dir=$save_path \
    --rank=$device_id \
    --device_target=$device_target \
    --ckpt_name=$ckpt > test.log  2>&1 &

echo 'running'
