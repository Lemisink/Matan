#include "Differentiator.h"

#include <cmath>
#include <utility>

namespace matan {

Differentiator::Differentiator(std::string method_name) : method_name_(std::move(method_name)) {}

void Differentiator::logSample(DerivativeResult& result, int i, double x, double fx, double d_true,
                               double d_est) const {
  if (result.method.empty()) {
    result.method = method_name_;
  }
  DerivativeSample sample;
  sample.i = i;
  sample.x = x;
  sample.fx = fx;
  sample.d_true = d_true;
  sample.d_est = d_est;
  sample.err = d_est - d_true;
  result.samples.push_back(sample);
}

void Differentiator::finalize(DerivativeResult& result) const {
  if (result.samples.empty()) {
    result.rmse = 0.0;
    return;
  }
  double sum_sq = 0.0;
  for (const auto& s : result.samples) {
    sum_sq += s.err * s.err;
  }
  result.rmse = std::sqrt(sum_sq / static_cast<double>(result.samples.size()));
}

}
