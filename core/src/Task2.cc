#include "Task2.h"

#include "CentralDifference.h"
#include "Expression.h"
#include "LeftDifference.h"
#include "RightDifference.h"
#include "Task2Runner.h"

namespace matan {

TaskResult Task2Right::run(const TaskContext& ctx) const {
  Expression f(ctx.func);
  std::string dfunc_str = ctx.dfunc.empty() ? deriveSymbolicDerivative(ctx.func) : ctx.dfunc;
  Expression df(dfunc_str);
  RightDifference method;
  DifferentiationContext dctx{f, df, ctx.a, ctx.b, ctx.h};
  return method.differentiate(dctx);
}

TaskResult Task2Left::run(const TaskContext& ctx) const {
  Expression f(ctx.func);
  std::string dfunc_str = ctx.dfunc.empty() ? deriveSymbolicDerivative(ctx.func) : ctx.dfunc;
  Expression df(dfunc_str);
  LeftDifference method;
  DifferentiationContext dctx{f, df, ctx.a, ctx.b, ctx.h};
  return method.differentiate(dctx);
}

TaskResult Task2Central::run(const TaskContext& ctx) const {
  Expression f(ctx.func);
  std::string dfunc_str = ctx.dfunc.empty() ? deriveSymbolicDerivative(ctx.func) : ctx.dfunc;
  Expression df(dfunc_str);
  CentralDifference method;
  DifferentiationContext dctx{f, df, ctx.a, ctx.b, ctx.h};
  return method.differentiate(dctx);
}

}  // namespace matan
