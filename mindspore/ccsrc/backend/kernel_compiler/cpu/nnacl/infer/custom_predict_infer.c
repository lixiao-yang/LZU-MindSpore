/**
 * Copyright 2021 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nnacl/infer/custom_predict_infer.h"
#include "nnacl/infer/infer_register.h"

int CustomPredictInferShape(const TensorC *const *inputs, size_t inputs_size, TensorC **outputs, size_t outputs_size,
                            OpParameter *parameter) {
  int check_ret = CheckAugmentWithMinSize(inputs, inputs_size, outputs, outputs_size, parameter, 1, 2);
  if (check_ret != NNACL_OK) {
    return check_ret;
  }

  const TensorC *input = inputs[0];
  TensorC *output0 = outputs[0];
  TensorC *output1 = outputs[1];

  CustomPredictParameter *param = (CustomPredictParameter *)parameter;
  output0->shape_size_ = 1;
  output0->shape_[0] = param->output_num;
  output0->data_type_ = kNumberTypeInt32;
  output0->format_ = input->format_;
  output1->shape_size_ = 1;
  output1->shape_[0] = param->output_num;
  output1->data_type_ = kNumberTypeFloat32;
  output1->format_ = input->format_;
  return NNACL_OK;
}

REG_INFER(CustomPredict, PrimType_CustomPredict, CustomPredictInferShape)
