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

#include "src/delegate/npu/op/convolution_base_npu.h"
#include "src/delegate/npu/npu_converter_utils.h"
#include "src/delegate/npu/transpose_kernel.h"

namespace mindspore {
ConvolutionBaseNPUOp::~ConvolutionBaseNPUOp() {
  if (act_ != nullptr) {
    delete act_;
    act_ = nullptr;
  }
  if (weight_ != nullptr) {
    delete weight_;
    weight_ = nullptr;
  }
  if (bias_ != nullptr) {
    delete bias_;
    bias_ = nullptr;
  }
}

int ConvolutionBaseNPUOp::InitWeightConst(const std::vector<mindspore::MSTensor> &inputs) {
  weight_ = new (std::nothrow) hiai::op::Const(name_ + "_w");
  if (weight_ == nullptr) {
    MS_LOG(ERROR) << "New weight const failed.";
    return RET_ERROR;
  }
  auto w_shape = inputs[1].Shape();
  auto origin_weight = inputs[1].Data().get();
  float *fp32_weight = nullptr;
  if (inputs[1].DataType() == DataType::kNumberTypeFloat16) {
#ifdef ENABLE_ARM64
    fp32_weight = reinterpret_cast<float *>(malloc(inputs[1].ElementNum() * sizeof(float)));
    // fp16->fp32
    Float16ToFloat32(reinterpret_cast<const float16_t *>(origin_weight), reinterpret_cast<float *>(fp32_weight),
                     inputs[1].ElementNum());
#else
    MS_LOG(ERROR) << "This platform does not support fp16.";
    return RET_ERROR;
#endif
  }
  auto nchw_weight = reinterpret_cast<float *>(malloc(inputs[1].ElementNum() * sizeof(float)));
  if (nchw_weight == nullptr) {
    MS_LOG(ERROR) << "Malloc buffer failed.";
    return RET_ERROR;
  }
  if (inputs[1].DataType() == DataType::kNumberTypeFloat16) {
    PackNHWCToNCHWFp32(fp32_weight, nchw_weight, w_shape[0], w_shape[1] * w_shape[2], w_shape[3]);
  } else if (inputs[1].DataType() == DataType::kNumberTypeFloat32) {
    PackNHWCToNCHWFp32(origin_weight, nchw_weight, w_shape[0], w_shape[1] * w_shape[2], w_shape[3]);
  } else {
    MS_LOG(ERROR) << "Unsupported data type of weight tensor for npu convolution.";
    return RET_ERROR;
  }

  std::shared_ptr<ge::Tensor> weight_tensor = std::shared_ptr<ge::Tensor>(new (std::nothrow) ge::Tensor());
  if (weight_tensor == nullptr) {
    MS_LOG(ERROR) << "new weight_tensor failed.";
    return RET_ERROR;
  }
  ge::TensorDesc tensor_desc(ConverterToNPUShape({w_shape[0], w_shape[3], w_shape[1], w_shape[2]}), ge::FORMAT_NCHW,
                             ConverterToNPUDataType(inputs[1].DataType()));
  weight_tensor->SetTensorDesc(tensor_desc);
  weight_tensor->SetData(reinterpret_cast<const uint8_t *>(nchw_weight), inputs[1].ElementNum() * sizeof(float));

  weight_->set_attr_value(weight_tensor);
  if (fp32_weight != nullptr) {
    free(fp32_weight);
    fp32_weight = nullptr;
  }
  free(nchw_weight);
  nchw_weight = nullptr;
  return RET_OK;
}

int ConvolutionBaseNPUOp::InitBiasConst(const std::vector<mindspore::MSTensor> &inputs) {
  if (inputs.size() >= 3) {
    bias_ = new (std::nothrow) hiai::op::Const(name_ + "_b");
    if (bias_ == nullptr) {
      MS_LOG(ERROR) << "New bias const failed.";
      return RET_ERROR;
    }
    std::shared_ptr<ge::Tensor> bias_tensor = ConverterToNPUTensor(inputs[2]);
    if (bias_tensor == nullptr) {
      MS_LOG(ERROR) << "Get bias_tensor failed.";
      return RET_ERROR;
    }
    bias_->set_attr_value(bias_tensor);
  }
  return RET_OK;
}

int ConvolutionBaseNPUOp::SetActivation(const ge::Operator *input, schema::ActivationType act_type) {
  act_ = new (std::nothrow) hiai::op::Activation(name_ + "_act");
  if (act_ == nullptr) {
    MS_LOG(ERROR) << "New activation npu operator for op " << name_ << " failed.";
    return RET_ERROR;
  }
  act_->set_input_x(*input);
  if (act_type == schema::ActivationType_RELU) {
    act_->set_attr_mode(1);
  } else if (act_type == schema::ActivationType_RELU6) {
    act_->set_attr_mode(14);
  } else {
    MS_LOG(ERROR) << "Unsupported activation type for convolution.";
    return RET_ERROR;
  }
  return RET_OK;
}
}  // namespace mindspore
