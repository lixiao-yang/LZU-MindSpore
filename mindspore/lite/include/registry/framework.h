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

#ifndef MINDSPORE_LITE_INCLUDE_REGISTRY_FRAMEWORK_H_
#define MINDSPORE_LITE_INCLUDE_REGISTRY_FRAMEWORK_H_

#include "include/lite_utils.h"

namespace mindspore {
namespace lite {
namespace converter {
/// \brief FmkType defined frameworks which converter tool supports.
enum MS_API FmkType : int {
  FmkType_TF = 0,
  FmkType_CAFFE = 1,
  FmkType_ONNX = 2,
  FmkType_MS = 3,
  FmkType_TFLITE = 4,
};
}  // namespace converter
}  // namespace lite
}  // namespace mindspore
#endif  // MINDSPORE_LITE_INCLUDE_REGISTRY_FRAMEWORK_H_
