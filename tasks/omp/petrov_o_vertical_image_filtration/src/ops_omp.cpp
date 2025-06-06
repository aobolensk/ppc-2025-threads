#include "omp/petrov_o_vertical_image_filtration/include/ops_omp.hpp"

#include <omp.h>

#include <cmath>
#include <cstddef>
#include <vector>

bool petrov_o_vertical_image_filtration_omp::TaskOpenMP::PreProcessingImpl() {
  width_ = static_cast<int>(task_data->inputs_count[0]);
  height_ = static_cast<int>(task_data->inputs_count[1]);
  size_t input_size = static_cast<size_t>(width_) * static_cast<size_t>(height_);

  auto *in_ptr = reinterpret_cast<int *>(task_data->inputs[0]);
  input_ = std::vector<int>(in_ptr, in_ptr + input_size);

  output_ = std::vector<int>((width_ - 2) * (height_ - 2), 0);
  return true;
}

bool petrov_o_vertical_image_filtration_omp::TaskOpenMP::ValidationImpl() {
  size_t width = task_data->inputs_count[0];
  size_t height = task_data->inputs_count[1];

  if (width < 3 || height < 3) {  // Check if the input image is too small
    return false;
  }

  return task_data->outputs_count[0] == (width - 2) * (height - 2);
}

bool petrov_o_vertical_image_filtration_omp::TaskOpenMP::RunImpl() {
// Apply Gaussian filter
#pragma omp parallel for schedule(dynamic)
  for (int i = 1; i < height_ - 1; ++i) {
    for (int j = 1; j < width_ - 1; ++j) {
      float sum = 0.0F;
      for (int ki = -1; ki <= 1; ++ki) {
        for (int kj = -1; kj <= 1; ++kj) {
          sum +=
              static_cast<float>(input_[((i + ki) * width_) + (j + kj)]) * gaussian_kernel_[((ki + 1) * 3) + (kj + 1)];
        }
      }
      output_[((i - 1) * (width_ - 2)) + (j - 1)] = static_cast<int>(sum);
    }
  }
  return true;
}

bool petrov_o_vertical_image_filtration_omp::TaskOpenMP::PostProcessingImpl() {
  for (size_t i = 0; i < output_.size(); i++) {
    reinterpret_cast<int *>(task_data->outputs[0])[i] = output_[i];
  }
  return true;
}
