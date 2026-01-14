#pragma once

#include "Minimizer.h"

namespace matan {

class DichotomyMinimizer final : public Minimizer {
 public:
  explicit DichotomyMinimizer(double delta = -1.0);
  MinimizationResult minimize(const MinimizationContext& ctx) const override;

 private:
  double delta_;
};

}
