#include <gtest/gtest.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "core/perf/include/perf.hpp"
#include "core/task/include/task.hpp"
#include "omp/Muradov_m_rect_int/include/ops_omp.hpp"

TEST(Muradov_m_rect_int_omp, test_pipeline_run) {
  int iterations = 475;
  std::vector<std::pair<double, double>> bounds(3, {-3.0, 3.0});
  double out = 0.0;

  auto task_data_omp = std::make_shared<ppc::core::TaskData>();
  task_data_omp->inputs.emplace_back(reinterpret_cast<uint8_t *>(&iterations));
  task_data_omp->inputs.emplace_back(reinterpret_cast<uint8_t *>(bounds.data()));
  task_data_omp->inputs_count.emplace_back(1);
  task_data_omp->inputs_count.emplace_back(bounds.size());
  task_data_omp->outputs.emplace_back(reinterpret_cast<uint8_t *>(&out));
  task_data_omp->outputs_count.emplace_back(1);

  auto test_task_ompuential = std::make_shared<muradov_m_rect_int_omp::RectIntTaskOmp>(
      task_data_omp, [](const auto &args) { return args[0] + args[1] + args[2]; });

  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();
  perf_attr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perf_attr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  auto perf_analyzer = std::make_shared<ppc::core::Perf>(test_task_ompuential);
  perf_analyzer->PipelineRun(perf_attr, perf_results);
  ppc::core::Perf::PrintPerfStatistic(perf_results);

  EXPECT_NEAR(out, -4.09, 0.3);
}

TEST(Muradov_m_rect_int_omp, test_task_run) {
  int iterations = 475;
  std::vector<std::pair<double, double>> bounds(3, {-3.0, 3.0});
  double out = 0.0;

  auto task_data_omp = std::make_shared<ppc::core::TaskData>();
  task_data_omp->inputs.emplace_back(reinterpret_cast<uint8_t *>(&iterations));
  task_data_omp->inputs.emplace_back(reinterpret_cast<uint8_t *>(bounds.data()));
  task_data_omp->inputs_count.emplace_back(1);
  task_data_omp->inputs_count.emplace_back(bounds.size());
  task_data_omp->outputs.emplace_back(reinterpret_cast<uint8_t *>(&out));
  task_data_omp->outputs_count.emplace_back(1);

  // Create Task
  auto test_task_ompuential = std::make_shared<muradov_m_rect_int_omp::RectIntTaskOmp>(
      task_data_omp, [](const auto &args) { return args[0] + args[1] + args[2]; });

  // Create Perf attributes
  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();
  perf_attr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perf_attr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  // Create and init perf results
  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perf_analyzer = std::make_shared<ppc::core::Perf>(test_task_ompuential);
  perf_analyzer->TaskRun(perf_attr, perf_results);
  ppc::core::Perf::PrintPerfStatistic(perf_results);

  EXPECT_NEAR(out, -4.09, 0.3);
}
