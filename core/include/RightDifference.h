#pragma once

#include "Differentiator.h"

namespace matan {

class RightDifference final : public Differentiator {
 public:
  RightDifference();
  DerivativeResult differentiate(const DifferentiationContext& ctx) const override;
};

}
