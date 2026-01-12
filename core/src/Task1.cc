#include "Task1.h"

#include "DichotomyMinimizer.h"
#include "Expression.h"
#include "GoldenSectionMinimizer.h"

namespace matan {

TaskResult Task1Dichotomy::run(const TaskContext& ctx) const {
  Expression expr(ctx.func);
  DichotomyMinimizer minimizer(ctx.delta);
  MinimizationContext mctx{expr, ctx.a, ctx.b, ctx.eps};
  return minimizer.minimize(mctx);
}

TaskResult Task1Golden::run(const TaskContext& ctx) const {
  Expression expr(ctx.func);
  GoldenSectionMinimizer minimizer;
  MinimizationContext mctx{expr, ctx.a, ctx.b, ctx.eps};
  return minimizer.minimize(mctx);
}

}  // namespace matan
