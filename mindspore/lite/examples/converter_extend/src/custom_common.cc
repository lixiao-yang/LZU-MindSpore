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

#include "src/custom_common.h"
#include <vector>

namespace mindspore {
namespace common {
int CheckInputs(const std::vector<tensor::MSTensor *> &inputs) {
  for (auto &input : inputs) {
    auto input_shape = input->shape();
    if (std::find(input_shape.begin(), input_shape.end(), -1) != input_shape.end()) {
      return lite::RET_INFER_INVALID;
    }
  }
  return lite::RET_OK;
}

int CheckOutputs(const std::vector<tensor::MSTensor *> &outputs) {
  for (auto &output : outputs) {
    auto output_shape = output->shape();
    if (std::find(output_shape.begin(), output_shape.end(), -1) != output_shape.end()) {
      return lite::RET_INFER_INVALID;
    }
  }
  return lite::RET_OK;
}
}  // namespace common
}  // namespace mindspore
