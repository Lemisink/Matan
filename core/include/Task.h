#pragma once

#include <string>
#include <variant>

#include "Differentiator.h"
#include "Minimizer.h"

namespace matan {

struct TaskContext {
  std::string func;
  std::string dfunc;
  double a = 0.0;
  double b = 0.0;
  double eps = 1e-4;
  double h = 0.1;
  double delta = -1.0;
};

using TaskResult = std::variant<MinimizationResult, DerivativeResult>;

class Task {
 public:
  virtual ~Task() = default;
  virtual std::string name() const = 0;
  virtual TaskResult run(const TaskContext& ctx) const = 0;
};

}  // namespace matan
