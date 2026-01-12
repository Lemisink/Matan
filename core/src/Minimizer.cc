#include "Minimizer.h"

#include <utility>

namespace matan {

Minimizer::Minimizer(std::string method_name) : method_name_(std::move(method_name)) {}

void Minimizer::logIteration(MinimizationResult& result, int k, double a, double b, double y,
                             double z, double fy, double fz) const {
  if (result.method.empty()) {
    result.method = method_name_;
  }
  IterationState state;
  state.k = k;
  state.a = a;
  state.b = b;
  state.y = y;
  state.z = z;
  state.fy = fy;
  state.fz = fz;
  state.x_star = 0.5 * (a + b);
  state.length = b - a;
  result.iterations.push_back(state);
}

}  // namespace matan
