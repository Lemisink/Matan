#pragma once

#include "Differentiator.h"

namespace matan {

class LeftDifference final : public Differentiator {
 public:
  LeftDifference();
  DerivativeResult differentiate(const DifferentiationContext& ctx) const override;
};

}
