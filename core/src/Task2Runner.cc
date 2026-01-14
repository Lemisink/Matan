#include "Task2Runner.h"

#include <cmath>
#include <sstream>

#include "DiffCommon.h"
#include "Expression.h"

#ifdef _OPENMP
#include <omp.h>
#endif

namespace matan {

namespace {

void fillSample(DerivativeSample& sample, int i, double x, double fx, double d_true,
                double d_est) {
  sample.i = i;
  sample.x = x;
  sample.fx = fx;
  sample.d_true = d_true;
  sample.d_est = d_est;
  sample.err = d_est - d_true;
}

}

Task2RmseRow computeRmseRow(const std::string& f_str, double a, double b, double h) {
  Expression f(f_str);
  auto grid = buildGrid(f, a, b, h);

  Task2RmseRow row;
  row.h = grid.h;

  const auto& x = grid.x;
  const auto& y = grid.y;
  int n = static_cast<int>(x.size());

  double sum_right = 0.0;
  double sum_left = 0.0;
  double sum_central = 0.0;

  for (int i = 0; i < n; ++i) {
    double d_true = f.derivative(x[i]);

    double d_right = 0.0;
    if (i == n - 1) {
      d_right = rightBoundaryDerivative(y, grid.h);
    } else {
      d_right = (y[i + 1] - y[i]) / grid.h;
    }

    double d_left = 0.0;
    if (i == 0) {
      d_left = leftBoundaryDerivative(y, grid.h);
    } else {
      d_left = (y[i] - y[i - 1]) / grid.h;
    }

    double d_central = 0.0;
    if (i == 0) {
      d_central = leftBoundaryDerivative(y, grid.h);
    } else if (i == n - 1) {
      d_central = rightBoundaryDerivative(y, grid.h);
    } else {
      d_central = (y[i + 1] - y[i - 1]) / (2.0 * grid.h);
    }

    double err = d_right - d_true;
    sum_right += err * err;
    err = d_left - d_true;
    sum_left += err * err;
    err = d_central - d_true;
    sum_central += err * err;
  }

  double denom = static_cast<double>(n);
  row.right = std::sqrt(sum_right / denom);
  row.left = std::sqrt(sum_left / denom);
  row.central = std::sqrt(sum_central / denom);
  return row;
}

Task2Results runAllDifferences(const std::string& f_str, double a, double b, double h) {
  Expression f(f_str);
  auto grid = buildGrid(f, a, b, h);

  const auto& x = grid.x;
  const auto& y = grid.y;
  int n = static_cast<int>(x.size());

  Task2Results results;
  results.right.method = "right";
  results.left.method = "left";
  results.central.method = "central";
  results.right.h = grid.h;
  results.left.h = grid.h;
  results.central.h = grid.h;
  results.right.samples.resize(x.size());
  results.left.samples.resize(x.size());
  results.central.samples.resize(x.size());

  double sum_right = 0.0;
  double sum_left = 0.0;
  double sum_central = 0.0;

  for (int i = 0; i < n; ++i) {
    double xi = x[i];
    double fx = y[i];
    double d_true = f.derivative(xi);

    double d_right = 0.0;
    if (i == n - 1) {
      d_right = rightBoundaryDerivative(y, grid.h);
    } else {
      d_right = (y[i + 1] - y[i]) / grid.h;
    }

    double d_left = 0.0;
    if (i == 0) {
      d_left = leftBoundaryDerivative(y, grid.h);
    } else {
      d_left = (y[i] - y[i - 1]) / grid.h;
    }

    double d_central = 0.0;
    if (i == 0) {
      d_central = leftBoundaryDerivative(y, grid.h);
    } else if (i == n - 1) {
      d_central = rightBoundaryDerivative(y, grid.h);
    } else {
      d_central = (y[i + 1] - y[i - 1]) / (2.0 * grid.h);
    }

    fillSample(results.right.samples[i], i, xi, fx, d_true, d_right);
    fillSample(results.left.samples[i], i, xi, fx, d_true, d_left);
    fillSample(results.central.samples[i], i, xi, fx, d_true, d_central);

    double err = d_right - d_true;
    sum_right += err * err;
    err = d_left - d_true;
    sum_left += err * err;
    err = d_central - d_true;
    sum_central += err * err;
  }

  double denom = static_cast<double>(n);
  results.right.rmse = std::sqrt(sum_right / denom);
  results.left.rmse = std::sqrt(sum_left / denom);
  results.central.rmse = std::sqrt(sum_central / denom);

  return results;
}

std::vector<Task2RmseRow> runRmseSweep(const std::string& f_str, double a, double b, double h0,
                                       int steps) {
  if (steps <= 0) {
    return {};
  }
  std::vector<Task2RmseRow> all;
  all.reserve(static_cast<std::size_t>(steps));
  double h = h0;
  for (int i = 0; i < steps; ++i) {
    all.push_back(computeRmseRow(f_str, a, b, h));
    h *= 0.5;
  }
  return all;
}

}
