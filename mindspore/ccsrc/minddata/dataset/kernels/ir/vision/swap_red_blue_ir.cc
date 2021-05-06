/**
 * Copyright 2020-2021 Huawei Technologies Co., Ltd
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
#include <algorithm>

#include "minddata/dataset/kernels/ir/vision/swap_red_blue_ir.h"

#ifndef ENABLE_ANDROID
#include "minddata/dataset/kernels/image/swap_red_blue_op.h"
#endif

#include "minddata/dataset/kernels/ir/validators.h"

namespace mindspore {
namespace dataset {

namespace vision {
#ifndef ENABLE_ANDROID

// SwapRedBlueOperation.
SwapRedBlueOperation::SwapRedBlueOperation() {}

SwapRedBlueOperation::~SwapRedBlueOperation() = default;

std::string SwapRedBlueOperation::Name() const { return kSwapRedBlueOperation; }

Status SwapRedBlueOperation::ValidateParams() { return Status::OK(); }

std::shared_ptr<TensorOp> SwapRedBlueOperation::Build() {
  std::shared_ptr<SwapRedBlueOp> tensor_op = std::make_shared<SwapRedBlueOp>();
  return tensor_op;
}

#endif

}  // namespace vision
}  // namespace dataset
}  // namespace mindspore