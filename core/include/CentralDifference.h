#pragma once

#include "Differentiator.h"

namespace matan {

class CentralDifference final : public Differentiator {
 public:
  CentralDifference();
  DerivativeResult differentiate(const DifferentiationContext& ctx) const override;
};

}
