#include "GoldenSectionMinimizer.h"

#include <cmath>
#include <stdexcept>

#include "Expression.h"

namespace matan {

GoldenSectionMinimizer::GoldenSectionMinimizer() : Minimizer("golden") {}

MinimizationResult GoldenSectionMinimizer::minimize(const MinimizationContext& ctx) const {
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

  const double tau = (std::sqrt(5.0) - 1.0) * 0.5;
  double y = a + (1.0 - tau) * (b - a);
  double z = a + tau * (b - a);
  double fy = f.eval(y);
  double fz = f.eval(z);

  MinimizationResult result;
  int k = 0;

  while (true) {
    logIteration(result, k, a, b, y, z, fy, fz);

    if ((b - a) <= eps) {
      break;
    }

    if (fy <= fz) {
      b = z;
      z = y;
      fz = fy;
      y = a + (1.0 - tau) * (b - a);
      fy = f.eval(y);
    } else {
      a = y;
      y = z;
      fy = fz;
      z = a + tau * (b - a);
      fz = f.eval(z);
    }
    ++k;
  }

  result.x_min = 0.5 * (a + b);
  result.f_min = f.eval(result.x_min);
  return result;
}

}  // namespace matan
