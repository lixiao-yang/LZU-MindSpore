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

#include "ops/split_with_overlap.h"
#include "ops/op_utils.h"
namespace mindspore {
namespace ops {
void SplitWithOverlap::Init(int64_t number_split, const std::vector<int64_t> &ratio,
                            const std::vector<int64_t> &extend_top, const std::vector<int64_t> &extend_bottom,
                            int64_t split_dim, int64_t stride, int64_t pad_top, bool trans_format) {
  this->set_number_split(number_split);
  this->set_ratio(ratio);
  this->set_extend_top(extend_top);
  this->set_extend_bottom(extend_bottom);
  this->set_split_dim(split_dim);
  this->set_stride(stride);
  this->set_pad_top(pad_top);
  this->set_trans_format(trans_format);
}

void SplitWithOverlap::set_ratio(const std::vector<int64_t> &ratio) { this->AddAttr(kRatio, MakeValue(ratio)); }

void SplitWithOverlap::set_extend_top(const std::vector<int64_t> &extend_top) {
  this->AddAttr(kExtendTop, MakeValue(extend_top));
}

void SplitWithOverlap::set_extend_bottom(const std::vector<int64_t> &extend_bottom) {
  this->AddAttr(kExtendBottom, MakeValue(extend_bottom));
}

void SplitWithOverlap::set_number_split(int64_t number_split) { this->AddAttr(kNumberSplit, MakeValue(number_split)); }

void SplitWithOverlap::set_split_dim(int64_t split_dim) { this->AddAttr(kSplitDim, MakeValue(split_dim)); }

void SplitWithOverlap::set_stride(int64_t stride) { this->AddAttr(kSplitStride, MakeValue(stride)); }

void SplitWithOverlap::set_pad_top(int64_t pad_top) { this->AddAttr(kPadTop, MakeValue(pad_top)); }

void SplitWithOverlap::set_trans_format(bool trans_format) { this->AddAttr(kTransFormat, MakeValue(trans_format)); }

std::vector<int64_t> SplitWithOverlap::get_ratio() const {
  auto value_ptr = GetAttr(kRatio);
  return GetValue<std::vector<int64_t>>(value_ptr);
}

std::vector<int64_t> SplitWithOverlap::get_extend_top() const {
  auto value_ptr = GetAttr(kExtendTop);
  return GetValue<std::vector<int64_t>>(value_ptr);
}

std::vector<int64_t> SplitWithOverlap::get_extend_bottom() const {
  auto value_ptr = GetAttr(kExtendBottom);
  return GetValue<std::vector<int64_t>>(value_ptr);
}

int64_t SplitWithOverlap::get_number_split() const {
  auto value_ptr = GetAttr(kNumberSplit);
  return GetValue<int64_t>(value_ptr);
}

int64_t SplitWithOverlap::get_split_dim() const {
  auto value_ptr = GetAttr(kSplitDim);
  return GetValue<int64_t>(value_ptr);
}

int64_t SplitWithOverlap::get_stride() const {
  auto value_ptr = GetAttr(kSplitStride);
  return GetValue<int64_t>(value_ptr);
}

int64_t SplitWithOverlap::get_pad_top() const {
  auto value_ptr = GetAttr(kPadTop);
  return GetValue<int64_t>(value_ptr);
}

bool SplitWithOverlap::get_trans_format() const {
  auto value_ptr = GetAttr(kTransFormat);
  return GetValue<bool>(value_ptr);
}

REGISTER_PRIMITIVE_C(kNameSplitWithOverlap, SplitWithOverlap);
}  // namespace ops
}  // namespace mindspore