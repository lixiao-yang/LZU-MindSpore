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

#include "minddata/dataset/kernels/ir/vision/crop_ir.h"

#include "minddata/dataset/kernels/image/crop_op.h"

#include "minddata/dataset/kernels/ir/validators.h"

namespace mindspore {
namespace dataset {

namespace vision {

CropOperation::CropOperation(std::vector<int32_t> coordinates, std::vector<int32_t> size)
    : coordinates_(coordinates), size_(size) {}

CropOperation::~CropOperation() = default;

std::string CropOperation::Name() const { return kCropOperation; }

Status CropOperation::ValidateParams() {
  // We have to limit crop size due to library restrictions, optimized to only iterate over size_ once
  // we don't check the coordinates here because we don't have access to image dimensions
  RETURN_IF_NOT_OK(ValidateVectorSize("Crop", size_));

  if (coordinates_.size() != 2) {
    std::string err_msg = "Crop: coordinates must be a vector of two values";
    MS_LOG(ERROR) << err_msg;
    RETURN_STATUS_SYNTAX_ERROR(err_msg);
  }
  RETURN_IF_NOT_OK(ValidateVectorNonNegative("Crop", "coordinates", coordinates_));
  return Status::OK();
}

std::shared_ptr<TensorOp> CropOperation::Build() {
  int32_t y, x, height, width;

  x = coordinates_[0];
  y = coordinates_[1];

  height = size_[0];
  width = size_[0];
  // User has specified crop_width.
  if (size_.size() == 2) {
    width = size_[1];
  }

  std::shared_ptr<CropOp> tensor_op = std::make_shared<CropOp>(y, x, height, width);
  return tensor_op;
}

}  // namespace vision
}  // namespace dataset
}  // namespace mindspore
