#include "CentralDifference.h"

#include "DiffCommon.h"
#include "Expression.h"

namespace matan {

CentralDifference::CentralDifference() : Differentiator("central") {}

DerivativeResult CentralDifference::differentiate(const DifferentiationContext& ctx) const {
  auto grid = buildGrid(ctx.f, ctx.a, ctx.b, ctx.h);
  DerivativeResult result;
  result.h = grid.h;

  const auto& x = grid.x;
  const auto& y = grid.y;
  int n = static_cast<int>(x.size());

  for (int i = 0; i < n; ++i) {
    double d_est = 0.0;
    if (i == 0) {
      d_est = leftBoundaryDerivative(y, grid.h);
    } else if (i == n - 1) {
      d_est = rightBoundaryDerivative(y, grid.h);
    } else {
      d_est = (y[i + 1] - y[i - 1]) / (2.0 * grid.h);
    }
    double d_true = ctx.f.derivative(x[i]);
    logSample(result, i, x[i], y[i], d_true, d_est);
  }

  finalize(result);
  return result;
}

}
