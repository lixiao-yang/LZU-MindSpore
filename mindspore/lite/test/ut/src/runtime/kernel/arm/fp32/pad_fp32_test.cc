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
#include <vector>
#include "mindspore/lite/src/lite_kernel.h"
#include "mindspore/lite/src/tensor.h"
#include "common/common_test.h"
#include "nnacl/pad_parameter.h"
#include "mindspore/lite/src/kernel_registry.h"
#include "schema/ops_generated.h"

using mindspore::schema::Format_NHWC;
using mindspore::schema::PaddingMode;
using mindspore::schema::PaddingMode_CONSTANT;
using mindspore::schema::PaddingMode_REFLECT;
using mindspore::schema::PaddingMode_SYMMETRIC;
namespace mindspore {

class TestPadFp32 : public mindspore::CommonTest {
 public:
  TestPadFp32() = default;
  void Prepare(const std::vector<int> &input_shape, const std::vector<int> &output_shape, float *input_data,
               float *output_data, PaddingMode mode, int *paddings, int padding_length, float constant_value,
               const int thread_num);

  void TearDown() override;

 public:
  float err_tol = 1e-5;
  lite::Tensor in_tensor_;
  lite::Tensor paddings_tensor_;
  lite::Tensor out_tensor_;
  PadParameter param_;
  std::vector<lite::Tensor *> inputs_{&in_tensor_};
  std::vector<lite::Tensor *> outputs_{&out_tensor_};
  kernel::KernelKey desc = {kernel::KERNEL_ARCH::kCPU, kNumberTypeFloat32, schema::PrimitiveType_Pad};
  lite::InnerContext ctx_ = lite::InnerContext();
  kernel::KernelCreator creator_ = nullptr;
  kernel::LiteKernel *kernel_ = nullptr;
};

void TestPadFp32::TearDown() {
  paddings_tensor_.SetData(nullptr);
  in_tensor_.SetData(nullptr);
  out_tensor_.SetData(nullptr);
}

void TestPadFp32::Prepare(const std::vector<int> &input_shape, const std::vector<int> &output_shape, float *input_data,
                          float *output_data, PaddingMode mode, int *paddings, int padding_length, float constant_value,
                          const int thread_num) {
  in_tensor_.set_data_type(kNumberTypeFloat32);
  in_tensor_.SetFormat(Format_NHWC);
  in_tensor_.set_shape(input_shape);
  out_tensor_.set_data_type(kNumberTypeFloat32);
  out_tensor_.set_shape(output_shape);
  in_tensor_.SetData(input_data);
  out_tensor_.SetData(output_data);

  param_.pad_mode_ = static_cast<int>(mode);
  if (mode == PaddingMode_CONSTANT) {
    param_.constant_value_ = constant_value;
    param_.padding_length = padding_length;
    for (auto i = 0; i < padding_length; ++i) {
      param_.paddings_[i] = paddings[i];
    }
  } else {
    paddings_tensor_.set_data_type(kNumberTypeInt32);
    paddings_tensor_.set_shape({4, 2});
    paddings_tensor_.SetData(paddings);
    inputs_.emplace_back(&paddings_tensor_);
  }

  desc = {kernel::KERNEL_ARCH::kCPU, kNumberTypeFloat32, schema::PrimitiveType_Pad};
  ctx_ = lite::InnerContext();
  ctx_.thread_num_ = thread_num;
  ctx_.Init();
  creator_ = lite::KernelRegistry::GetInstance()->GetCreator(desc);
  ASSERT_NE(creator_, nullptr);
  kernel_ = creator_(inputs_, outputs_, reinterpret_cast<OpParameter *>(&param_), &ctx_, desc, nullptr);
  ASSERT_NE(kernel_, nullptr);
}

TEST_F(TestPadFp32, TestPad1) {
  std::vector<int> input_shape{1, 4, 4, 3};
  std::vector<int> output_shape{1, 12, 12, 3};
  float in_data[48] = {0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0, 11.0, 12.0, 13.0, 14.0, 15.0,
                       16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0,
                       32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0};
  float out_data[432] = {0};
  int paddings[8] = {0, 0, 4, 4, 4, 4, 0, 0};
  PaddingMode mode = PaddingMode_SYMMETRIC;
  int thread_num = 2;
  Prepare(input_shape, output_shape, in_data, out_data, mode, paddings, 8, 0.0f, thread_num);

  auto ret = kernel_->Run();
  EXPECT_EQ(0, ret);

  std::vector<float> expect{
    45.0, 46.0, 47.0, 42.0, 43.0, 44.0, 39.0, 40.0, 41.0, 36.0, 37.0, 38.0, 36.0, 37.0, 38.0, 39.0, 40.0, 41.0, 42.0,
    43.0, 44.0, 45.0, 46.0, 47.0, 45.0, 46.0, 47.0, 42.0, 43.0, 44.0, 39.0, 40.0, 41.0, 36.0, 37.0, 38.0, 33.0, 34.0,
    35.0, 30.0, 31.0, 32.0, 27.0, 28.0, 29.0, 24.0, 25.0, 26.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0,
    33.0, 34.0, 35.0, 33.0, 34.0, 35.0, 30.0, 31.0, 32.0, 27.0, 28.0, 29.0, 24.0, 25.0, 26.0, 21.0, 22.0, 23.0, 18.0,
    19.0, 20.0, 15.0, 16.0, 17.0, 12.0, 13.0, 14.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0,
    23.0, 21.0, 22.0, 23.0, 18.0, 19.0, 20.0, 15.0, 16.0, 17.0, 12.0, 13.0, 14.0, 9.0,  10.0, 11.0, 6.0,  7.0,  8.0,
    3.0,  4.0,  5.0,  0.0,  1.0,  2.0,  0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0, 11.0, 9.0,
    10.0, 11.0, 6.0,  7.0,  8.0,  3.0,  4.0,  5.0,  0.0,  1.0,  2.0,  9.0,  10.0, 11.0, 6.0,  7.0,  8.0,  3.0,  4.0,
    5.0,  0.0,  1.0,  2.0,  0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0, 11.0, 9.0,  10.0, 11.0,
    6.0,  7.0,  8.0,  3.0,  4.0,  5.0,  0.0,  1.0,  2.0,  21.0, 22.0, 23.0, 18.0, 19.0, 20.0, 15.0, 16.0, 17.0, 12.0,
    13.0, 14.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 21.0, 22.0, 23.0, 18.0, 19.0,
    20.0, 15.0, 16.0, 17.0, 12.0, 13.0, 14.0, 33.0, 34.0, 35.0, 30.0, 31.0, 32.0, 27.0, 28.0, 29.0, 24.0, 25.0, 26.0,
    24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0, 33.0, 34.0, 35.0, 33.0, 34.0, 35.0, 30.0, 31.0, 32.0, 27.0,
    28.0, 29.0, 24.0, 25.0, 26.0, 45.0, 46.0, 47.0, 42.0, 43.0, 44.0, 39.0, 40.0, 41.0, 36.0, 37.0, 38.0, 36.0, 37.0,
    38.0, 39.0, 40.0, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0, 45.0, 46.0, 47.0, 42.0, 43.0, 44.0, 39.0, 40.0, 41.0,
    36.0, 37.0, 38.0, 45.0, 46.0, 47.0, 42.0, 43.0, 44.0, 39.0, 40.0, 41.0, 36.0, 37.0, 38.0, 36.0, 37.0, 38.0, 39.0,
    40.0, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0, 45.0, 46.0, 47.0, 42.0, 43.0, 44.0, 39.0, 40.0, 41.0, 36.0, 37.0,
    38.0, 33.0, 34.0, 35.0, 30.0, 31.0, 32.0, 27.0, 28.0, 29.0, 24.0, 25.0, 26.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0,
    30.0, 31.0, 32.0, 33.0, 34.0, 35.0, 33.0, 34.0, 35.0, 30.0, 31.0, 32.0, 27.0, 28.0, 29.0, 24.0, 25.0, 26.0, 21.0,
    22.0, 23.0, 18.0, 19.0, 20.0, 15.0, 16.0, 17.0, 12.0, 13.0, 14.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0,
    20.0, 21.0, 22.0, 23.0, 21.0, 22.0, 23.0, 18.0, 19.0, 20.0, 15.0, 16.0, 17.0, 12.0, 13.0, 14.0, 9.0,  10.0, 11.0,
    6.0,  7.0,  8.0,  3.0,  4.0,  5.0,  0.0,  1.0,  2.0,  0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,
    10.0, 11.0, 9.0,  10.0, 11.0, 6.0,  7.0,  8.0,  3.0,  4.0,  5.0,  0.0,  1.0,  2.0};
  CompareOutputData(out_data, expect.data(), 432, err_tol);
}

TEST_F(TestPadFp32, TestPad2) {
  std::vector<int> input_shape{1, 4, 4, 3};
  std::vector<int> output_shape{1, 10, 10, 3};
  float in_data[48] = {0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0, 11.0, 12.0, 13.0, 14.0, 15.0,
                       16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0,
                       32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0};
  float out_data[300] = {0};
  int paddings[8] = {0, 0, 3, 3, 3, 3, 0, 0};
  PaddingMode mode = PaddingMode_REFLECT;
  int thread_num = 2;
  Prepare(input_shape, output_shape, in_data, out_data, mode, paddings, 8, 0.0f, thread_num);

  auto ret = kernel_->Run();
  EXPECT_EQ(0, ret);

  std::vector<float> expect{
    45.0, 46.0, 47.0, 42.0, 43.0, 44.0, 39.0, 40.0, 41.0, 36.0, 37.0, 38.0, 39.0, 40.0, 41.0, 42.0, 43.0, 44.0, 45.0,
    46.0, 47.0, 42.0, 43.0, 44.0, 39.0, 40.0, 41.0, 36.0, 37.0, 38.0, 33.0, 34.0, 35.0, 30.0, 31.0, 32.0, 27.0, 28.0,
    29.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0, 33.0, 34.0, 35.0, 30.0, 31.0, 32.0, 27.0, 28.0, 29.0,
    24.0, 25.0, 26.0, 21.0, 22.0, 23.0, 18.0, 19.0, 20.0, 15.0, 16.0, 17.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0,
    19.0, 20.0, 21.0, 22.0, 23.0, 18.0, 19.0, 20.0, 15.0, 16.0, 17.0, 12.0, 13.0, 14.0, 9.0,  10.0, 11.0, 6.0,  7.0,
    8.0,  3.0,  4.0,  5.0,  0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0, 11.0, 6.0,  7.0,  8.0,
    3.0,  4.0,  5.0,  0.0,  1.0,  2.0,  21.0, 22.0, 23.0, 18.0, 19.0, 20.0, 15.0, 16.0, 17.0, 12.0, 13.0, 14.0, 15.0,
    16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 18.0, 19.0, 20.0, 15.0, 16.0, 17.0, 12.0, 13.0, 14.0, 33.0, 34.0,
    35.0, 30.0, 31.0, 32.0, 27.0, 28.0, 29.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0, 33.0, 34.0, 35.0,
    30.0, 31.0, 32.0, 27.0, 28.0, 29.0, 24.0, 25.0, 26.0, 45.0, 46.0, 47.0, 42.0, 43.0, 44.0, 39.0, 40.0, 41.0, 36.0,
    37.0, 38.0, 39.0, 40.0, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0, 42.0, 43.0, 44.0, 39.0, 40.0, 41.0, 36.0, 37.0,
    38.0, 33.0, 34.0, 35.0, 30.0, 31.0, 32.0, 27.0, 28.0, 29.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0,
    33.0, 34.0, 35.0, 30.0, 31.0, 32.0, 27.0, 28.0, 29.0, 24.0, 25.0, 26.0, 21.0, 22.0, 23.0, 18.0, 19.0, 20.0, 15.0,
    16.0, 17.0, 12.0, 13.0, 14.0, 15.0, 16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 18.0, 19.0, 20.0, 15.0, 16.0,
    17.0, 12.0, 13.0, 14.0, 9.0,  10.0, 11.0, 6.0,  7.0,  8.0,  3.0,  4.0,  5.0,  0.0,  1.0,  2.0,  3.0,  4.0,  5.0,
    6.0,  7.0,  8.0,  9.0,  10.0, 11.0, 6.0,  7.0,  8.0,  3.0,  4.0,  5.0,  0.0,  1.0,  2.0};
  CompareOutputData(out_data, expect.data(), 300, err_tol);
}

TEST_F(TestPadFp32, TestPad3) {
  std::vector<int> input_shape{1, 4, 4, 3};
  std::vector<int> output_shape{1, 10, 10, 3};
  float in_data[48] = {0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0, 11.0, 12.0, 13.0, 14.0, 15.0,
                       16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0,
                       32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0};
  float out_data[300] = {0};
  int paddings[8] = {0, 0, 3, 3, 3, 3, 0, 0};
  PaddingMode mode = PaddingMode_CONSTANT;
  float pad_value = 0.0f;
  int thread_num = 2;
  Prepare(input_shape, output_shape, in_data, out_data, mode, paddings, 8, pad_value, thread_num);

  auto ret = kernel_->Run();
  EXPECT_EQ(0, ret);

  std::vector<float> expect{
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
    0.0,  0.0,  0.0,  0.0,  0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0, 11.0, 0.0,  0.0,  0.0,
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  12.0, 13.0, 14.0, 15.0,
    16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0, 33.0, 34.0, 35.0,
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  36.0,
    37.0, 38.0, 39.0, 40.0, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
    0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0};
  CompareOutputData(out_data, expect.data(), 300, err_tol);
}

TEST_F(TestPadFp32, TestPad4) {
  std::vector<int> input_shape{1, 4, 4, 3};
  std::vector<int> output_shape{1, 10, 10, 3};
  float in_data[48] = {0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0, 11.0, 12.0, 13.0, 14.0, 15.0,
                       16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0,
                       32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0};
  float out_data[300] = {0};
  int paddings[8] = {0, 0, 3, 3, 3, 3, 0, 0};
  PaddingMode mode = PaddingMode_CONSTANT;
  float pad_value = 1.0f;
  int thread_num = 2;
  Prepare(input_shape, output_shape, in_data, out_data, mode, paddings, 8, pad_value, thread_num);

  auto ret = kernel_->Run();
  EXPECT_EQ(0, ret);

  std::vector<float> expect{
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0, 11.0, 1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  12.0, 13.0, 14.0, 15.0,
    16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0, 33.0, 34.0, 35.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  36.0,
    37.0, 38.0, 39.0, 40.0, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0, 1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0};
  CompareOutputData(out_data, expect.data(), 300, err_tol);
}

TEST_F(TestPadFp32, TestPad5) {
  std::vector<int> input_shape{4, 4, 3};
  std::vector<int> output_shape{10, 10, 3};
  float in_data[48] = {0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0, 11.0, 12.0, 13.0, 14.0, 15.0,
                       16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0,
                       32.0, 33.0, 34.0, 35.0, 36.0, 37.0, 38.0, 39.0, 40.0, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0};
  float out_data[300] = {0};
  int paddings[8] = {3, 3, 3, 3, 0, 0, 0, 0};
  PaddingMode mode = PaddingMode_CONSTANT;
  float pad_value = 1.0f;
  int thread_num = 2;
  Prepare(input_shape, output_shape, in_data, out_data, mode, paddings, 6, pad_value, thread_num);

  auto ret = kernel_->Run();
  EXPECT_EQ(0, ret);

  std::vector<float> expect{
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  0.0,  1.0,  2.0,  3.0,  4.0,  5.0,  6.0,  7.0,  8.0,  9.0,  10.0, 11.0, 1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  12.0, 13.0, 14.0, 15.0,
    16.0, 17.0, 18.0, 19.0, 20.0, 21.0, 22.0, 23.0, 1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  24.0, 25.0, 26.0, 27.0, 28.0, 29.0, 30.0, 31.0, 32.0, 33.0, 34.0, 35.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  36.0,
    37.0, 38.0, 39.0, 40.0, 41.0, 42.0, 43.0, 44.0, 45.0, 46.0, 47.0, 1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,
    1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0,  1.0};
  CompareOutputData(out_data, expect.data(), 300, err_tol);
}
}  // namespace mindspore
