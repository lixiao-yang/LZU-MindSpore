/**
 * Copyright 2020 Huawei Technologies Co., Ltd
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

#ifndef MINDSPORE_LITE_NNACL_TRANSPOSE_H_
#define MINDSPORE_LITE_NNACL_TRANSPOSE_H_

#include "nnacl/op_base.h"

#define MAX_TRANSPOSE_DIM_SIZE 5

typedef struct TransposeParameter {
  // primitive parameter
  OpParameter op_parameter_;
  int perm_[8];
  size_t perm_size_;
  bool conjugate_;

  // shape correlative
  int strides_[8];
  int out_strides_[8];

  // other parameter
  int num_axes_;
  int data_size_;
} TransposeParameter;

#endif  // MINDSPORE_LITE_NNACL_TRANSPOSE_H_