#include "Task2.h"

#include "CentralDifference.h"
#include "Expression.h"
#include "LeftDifference.h"
#include "RightDifference.h"
#include "Task2Runner.h"

namespace matan {

TaskResult Task2Right::run(const TaskContext& ctx) const {
  Expression f(ctx.func);
  RightDifference method;
  DifferentiationContext dctx{f, f, ctx.a, ctx.b, ctx.h};
  return method.differentiate(dctx);
}

TaskResult Task2Left::run(const TaskContext& ctx) const {
  Expression f(ctx.func);
  LeftDifference method;
  DifferentiationContext dctx{f, f, ctx.a, ctx.b, ctx.h};
  return method.differentiate(dctx);
}

TaskResult Task2Central::run(const TaskContext& ctx) const {
  Expression f(ctx.func);
  CentralDifference method;
  DifferentiationContext dctx{f, f, ctx.a, ctx.b, ctx.h};
  return method.differentiate(dctx);
}

}
