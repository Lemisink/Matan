#include "DichotomyMinimizer.h"

#include <algorithm>
#include <stdexcept>

#include "Expression.h"

namespace matan {

DichotomyMinimizer::DichotomyMinimizer(double delta) : Minimizer("dichotomy"), delta_(delta) {}

MinimizationResult DichotomyMinimizer::minimize(const MinimizationContext& ctx) const {
  const Expression& f = ctx.f;
  double a = ctx.a;
  double b = ctx.b;
  double eps = ctx.eps;
  if (a >= b) {
    throw std::runtime_error("Invalid interval: a must be less than b");
  }
  if (eps <= 0.0) {
    throw std::runtime_error("Invalid eps: must be positive");
  }

  MinimizationResult result;
  if ((b - a) <= 2.0 * eps) {
    result.x_min = 0.5 * (a + b);
    result.f_min = f.eval(result.x_min);
    return result;
  }

  double delta = delta_;
  if (delta <= 0.0) {
    delta = eps * 0.5;
  }
  if (delta <= 0.0) {
    throw std::runtime_error("Invalid delta: must be positive");
  }
  delta = std::min(delta, eps * 0.5);

  int k = 0;

  while (true) {
    double mid = 0.5 * (a + b);
    double y = mid - delta;
    double z = mid + delta;
    double fy = f.eval(y);
    double fz = f.eval(z);
    logIteration(result, k, a, b, y, z, fy, fz);

    if (fy <= fz) {
      b = z;
    } else {
      a = y;
    }

    if ((b - a) <= 2.0 * eps) {
      break;
    }
    ++k;
  }

  result.x_min = 0.5 * (a + b);
  result.f_min = f.eval(result.x_min);
  return result;
}

}  // namespace matan
