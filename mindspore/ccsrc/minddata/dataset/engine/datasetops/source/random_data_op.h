/**
 * Copyright 2019-2021 Huawei Technologies Co., Ltd
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
#ifndef MINDSPORE_CCSRC_MINDDATA_DATASET_ENGINE_DATASETOPS_SOURCE_RANDOM_DATA_OP_
#define MINDSPORE_CCSRC_MINDDATA_DATASET_ENGINE_DATASETOPS_SOURCE_RANDOM_DATA_OP_

#include <atomic>
#include <memory>
#include <mutex>
#include <random>
#include <string>
#include <vector>
#include <utility>
#include "minddata/dataset/util/status.h"
#include "minddata/dataset/core/tensor.h"
#include "minddata/dataset/core/data_type.h"
#include "minddata/dataset/engine/data_schema.h"
#include "minddata/dataset/engine/datasetops/parallel_op.h"
#include "minddata/dataset/util/wait_post.h"

namespace mindspore {
namespace dataset {
// The RandomDataOp is a leaf node storage operator that generates random data based
// on the schema specifications.  Typically, it's used for testing and demonstrating
// various dataset operator pipelines.  It is not "real" data to train with.
// The data that is random created is just random and repeated bytes, there is no
// "meaning" behind what these bytes are.
class RandomDataOp : public ParallelOp {
 public:
  // Some constants to provide limits to random generation.
  static constexpr int32_t kMaxNumColumns = 4;
  static constexpr int32_t kMaxRank = 4;
  static constexpr int32_t kMaxDimValue = 32;
  static constexpr int32_t kMaxTotalRows = 1024;

  /**
   * Constructor for RandomDataOp
   * @note Private constructor.  Must use builder to construct.
   * @param num_workers - The number of workers
   * @param op_connector_size - The size of the output connector
   * @param data_schema - A user-provided schema
   * @param total_rows - The total number of rows in the dataset
   * @return Builder - The modified builder by reference
   */
  RandomDataOp(int32_t num_workers, int32_t op_connector_size, int64_t total_rows,
               std::unique_ptr<DataSchema> data_schema);

  /**
   * Destructor
   */
  ~RandomDataOp() = default;

  /**
   * A print method typically used for debugging
   * @param out - The output stream to write output to
   * @param show_all - A bool to control if you want to show all info or just a summary
   */
  void Print(std::ostream &out, bool show_all) const override;

  /**
   * << Stream output operator overload
   * @notes This allows you to write the debug print info using stream operators
   * @param out - reference to the output stream being overloaded
   * @param so - reference to the ShuffleOp to display
   * @return - the output stream must be returned
   */
  friend std::ostream &operator<<(std::ostream &out, const RandomDataOp &op) {
    op.Print(out, false);
    return out;
  }

  /**
   * Class functor operator () override.
   * All DatasetOps operate by launching a thread (see ExecutionTree). This class functor will
   * provide the master loop that drives the logic for performing the work.
   * @return Status The status code returned
   */
  Status operator()() override;

  /**
   * Overrides base class reset method.  When an operator does a reset, it cleans up any state
   * info from it's previous execution and then initializes itself so that it can be executed
   * again.
   * @return Status The status code returned
   */
  Status Reset() override;

  /**
   * Quick getter for total rows.
   */
  int64_t GetTotalRows() const { return total_rows_; }

  // Op name getter
  // @return Name of the current Op
  std::string Name() const override { return "RandomDataOp"; }

 private:
  /**
   * The entry point code for when workers are launched
   * @param worker_id - The worker id
   * @return Status The status code returned
   */
  Status WorkerEntry(int32_t worker_id) override;

  /**
   * Helper function to produce a default/random schema if one didn't exist
   */
  void GenerateSchema();

  /**
   * Performs a synchronization between workers at the end of an epoch
   * @param worker_id - The worker id
   * @return Status The status code returned
   */
  Status EpochSync(int32_t worker_id, bool *quitting);

  /**
   * A helper function to create random data for the row
   * @param worker_id - The worker id
   * @param new_row - The output row to produce
   * @return Status The status code returned
   */
  Status CreateRandomRow(int32_t worker_id, TensorRow *new_row);

  /**
   * A quick inline for producing a random number between (and including) min/max
   * @param min - minimum number that can be generated
   * @param max - maximum number that can be generated
   * @return - The generated random number
   */
  inline int32_t GenRandomInt(int32_t min, int32_t max) {
    std::uniform_int_distribution<int32_t> uniDist(min, max);
    return uniDist(rand_gen_);
  }

  // Private function for computing the assignment of the column name map.
  // @return - Status
  Status ComputeColMap() override;

  int64_t total_rows_;
  int64_t epoch_rows_sent_;
  std::atomic<int32_t> guys_in_;
  std::atomic<int32_t> guys_out_;
  int32_t eoe_worker_id_;
  std::unique_ptr<DataSchema> data_schema_;
  std::vector<int64_t> worker_max_rows_;
  std::vector<int64_t> worker_rows_packed_;
  std::mt19937 rand_gen_;
  WaitPost epoch_sync_wait_post_;
  WaitPost all_out_;
};  // class RandomDataOp
}  // namespace dataset
}  // namespace mindspore

#endif  // MINDSPORE_CCSRC_MINDDATA_DATASET_ENGINE_DATASETOPS_SOURCE_RANDOM_DATA_OP_
