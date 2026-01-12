#pragma once

#include <string>
#include <vector>

#include "Differentiator.h"

namespace matan {

struct Task2Results {
  DerivativeResult right;
  DerivativeResult left;
  DerivativeResult central;
  std::string derivative_expr;
};

struct Task2RmseRow {
  double h = 0.0;
  double right = 0.0;
  double left = 0.0;
  double central = 0.0;
};

Task2Results runAllDifferences(const std::string& f_str, double a, double b, double h);

std::vector<Task2RmseRow> runRmseSweep(const std::string& f_str, double a, double b, double h0,
                                       int steps);

std::string deriveSymbolicDerivative(const std::string& f_str);

}  // namespace matan
