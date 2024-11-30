#include <gtest/gtest.h>

#include <chrono>
#include <random>
#include <vector>

#include "core/perf/include/perf.hpp"
#include "seq/somov_i_horizontal_scheme/include/ops_seq.hpp"

namespace somov_i_horizontal_scheme {

std::vector<std::vector<int32_t>> create_random_matrix_normal(uint32_t rowCount, uint32_t colCount) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution<float> dist(0.0f, 100.0f);

  std::vector<std::vector<int32_t>> matrix(rowCount, std::vector<int32_t>(colCount));

  for (auto &row : matrix) {
    for (auto &el : row) {
      el = std::clamp(static_cast<int32_t>(std::round(dist(gen))), -300, 300);
    }
  }

  return matrix;
}

std::vector<int32_t> create_random_vector_normal(uint32_t size) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution<float> dist(0.0f, 100.0f);

  std::vector<int32_t> vec(size);

  for (auto &el : vec) {
    el = std::clamp(static_cast<int32_t>(std::round(dist(gen))), -300, 300);
  }

  return vec;
}

}  // namespace somov_i_horizontal_scheme

TEST(somov_i_horizontal_scheme, test_performance_with_different_data_sizes) {
  std::vector<std::pair<uint32_t, uint32_t>> sizes = {{100, 100}, {500, 500}, {1000, 1000}, {2000, 2000}, {3000, 3000}};

  for (const auto &[rowCount, colCount] : sizes) {
    std::vector<std::vector<int32_t>> matrix;
    std::vector<int32_t> vec;
    std::vector<int32_t> result;

    auto task_data = std::make_shared<ppc::core::TaskData>();
    somov_i_horizontal_scheme::MatrixVectorTask task(task_data);

    matrix = somov_i_horizontal_scheme::create_random_matrix_normal(rowCount, colCount);
    vec = somov_i_horizontal_scheme::create_random_vector_normal(colCount);
    result.resize(rowCount);

    for (auto &row : matrix) {
      task_data->inputs.push_back(reinterpret_cast<uint8_t *>(row.data()));
    }

    task_data->inputs.push_back(reinterpret_cast<uint8_t *>(vec.data()));
    task_data->inputs_count.push_back(rowCount);
    task_data->inputs_count.push_back(colCount);
    task_data->outputs.push_back(reinterpret_cast<uint8_t *>(result.data()));
    task_data->outputs_count.push_back(result.size());

    auto seq_task = std::make_shared<somov_i_horizontal_scheme::MatrixVectorTask>(task_data);

    ASSERT_TRUE(seq_task->validation() && seq_task->pre_processing() && seq_task->run() && seq_task->post_processing());

    auto perf_attributes = std::make_shared<ppc::core::PerfAttr>();
    perf_attributes->num_running = 10;
    auto start_time = std::chrono::high_resolution_clock::now();
    perf_attributes->current_timer = [&start_time] {
      auto now = std::chrono::high_resolution_clock::now();
      return std::chrono::duration<double>(now - start_time).count();
    };

    auto performance_results = std::make_shared<ppc::core::PerfResults>();

    auto performance_analyzer = std::make_shared<ppc::core::Perf>(seq_task);
    performance_analyzer->task_run(perf_attributes, performance_results);
  }
}

TEST(somov_i_horizontal_scheme, test_pipeline_run) {
  uint32_t rowCount = 2000;
  uint32_t colCount = 2000;

  std::vector<std::vector<int32_t>> matrix;
  std::vector<int32_t> vec;
  std::vector<int32_t> result;

  auto task_data = std::make_shared<ppc::core::TaskData>();
  somov_i_horizontal_scheme::MatrixVectorTask task(task_data);

  matrix = somov_i_horizontal_scheme::create_random_matrix_normal(rowCount, colCount);
  vec = somov_i_horizontal_scheme::create_random_vector_normal(colCount);
  result.resize(rowCount);

  for (auto &row : matrix) {
    task_data->inputs.push_back(reinterpret_cast<uint8_t *>(row.data()));
  }

  task_data->inputs.push_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data->inputs_count.push_back(rowCount);
  task_data->inputs_count.push_back(colCount);
  task_data->outputs.push_back(reinterpret_cast<uint8_t *>(result.data()));
  task_data->outputs_count.push_back(result.size());

  auto seq_task = std::make_shared<somov_i_horizontal_scheme::MatrixVectorTask>(task_data);

  ASSERT_TRUE(seq_task->validation() && seq_task->pre_processing() && seq_task->run() && seq_task->post_processing());

  auto perf_attributes = std::make_shared<ppc::core::PerfAttr>();
  perf_attributes->num_running = 10;
  auto start_time = std::chrono::high_resolution_clock::now();
  perf_attributes->current_timer = [&start_time] {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(now - start_time).count();
  };

  auto performance_results = std::make_shared<ppc::core::PerfResults>();

  auto performance_analyzer = std::make_shared<ppc::core::Perf>(seq_task);
  performance_analyzer->pipeline_run(perf_attributes, performance_results);

  ppc::core::Perf::print_perf_statistic(performance_results);
}

TEST(somov_i_horizontal_scheme, test_task_run) {
  uint32_t rowCount = 2000;
  uint32_t colCount = 2000;

  std::vector<std::vector<int32_t>> matrix;
  std::vector<int32_t> vec;
  std::vector<int32_t> result;

  auto task_data = std::make_shared<ppc::core::TaskData>();
  somov_i_horizontal_scheme::MatrixVectorTask task(task_data);

  matrix = somov_i_horizontal_scheme::create_random_matrix_normal(rowCount, colCount);
  vec = somov_i_horizontal_scheme::create_random_vector_normal(colCount);
  result.resize(rowCount);

  for (auto &row : matrix) {
    task_data->inputs.push_back(reinterpret_cast<uint8_t *>(row.data()));
  }

  task_data->inputs.push_back(reinterpret_cast<uint8_t *>(vec.data()));
  task_data->inputs_count.push_back(rowCount);
  task_data->inputs_count.push_back(colCount);
  task_data->outputs.push_back(reinterpret_cast<uint8_t *>(result.data()));
  task_data->outputs_count.push_back(result.size());

  auto seq_task = std::make_shared<somov_i_horizontal_scheme::MatrixVectorTask>(task_data);

  ASSERT_TRUE(seq_task->validation() && seq_task->pre_processing() && seq_task->run() && seq_task->post_processing());

  auto perf_attributes = std::make_shared<ppc::core::PerfAttr>();
  perf_attributes->num_running = 10;
  auto start_time = std::chrono::high_resolution_clock::now();
  perf_attributes->current_timer = [&start_time] {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(now - start_time).count();
  };

  auto performance_results = std::make_shared<ppc::core::PerfResults>();

  auto performance_analyzer = std::make_shared<ppc::core::Perf>(seq_task);
  performance_analyzer->task_run(perf_attributes, performance_results);

  ppc::core::Perf::print_perf_statistic(performance_results);
}