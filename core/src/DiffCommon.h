#pragma once

#include <cmath>
#include <stdexcept>
#include <vector>

#include "Expression.h"

namespace matan {

struct GridData {
  double h = 0.0;
  std::vector<double> x;
  std::vector<double> y;
};

inline GridData buildGrid(const Expression& f, double a, double b, double h) {
  if (a >= b) {
    throw std::runtime_error("Invalid interval: a must be less than b");
  }
  if (h <= 0.0) {
    throw std::runtime_error("Invalid h: must be positive");
  }

  double span = b - a;
  double n_raw = span / h;
  int n = static_cast<int>(std::round(n_raw));
  if (n < 2) {
    throw std::runtime_error("Invalid grid: need at least 3 points");
  }
  if (std::fabs(n_raw - static_cast<double>(n)) > 1e-9) {
    throw std::runtime_error("Invalid h: (b - a) must be divisible by h");
  }

  GridData grid;
  grid.h = h;
  grid.x.reserve(static_cast<std::size_t>(n) + 1);
  grid.y.reserve(static_cast<std::size_t>(n) + 1);

  for (int i = 0; i <= n; ++i) {
    double xi = a + static_cast<double>(i) * h;
    grid.x.push_back(xi);
    grid.y.push_back(f.eval(xi));
  }

  return grid;
}

inline double leftBoundaryDerivative(const std::vector<double>& y, double h) {
  return (-3.0 * y[0] + 4.0 * y[1] - y[2]) / (2.0 * h);
}

inline double rightBoundaryDerivative(const std::vector<double>& y, double h) {
  std::size_t n = y.size();
  return (y[n - 3] - 4.0 * y[n - 2] + 3.0 * y[n - 1]) / (2.0 * h);
}

}

