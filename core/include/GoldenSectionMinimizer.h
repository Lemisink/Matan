#pragma once

#include "Minimizer.h"

namespace matan {

class GoldenSectionMinimizer final : public Minimizer {
 public:
  GoldenSectionMinimizer();
  MinimizationResult minimize(const MinimizationContext& ctx) const override;
};

}
