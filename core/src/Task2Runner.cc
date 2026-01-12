#include "Task2Runner.h"

#include <cmath>
#include <sstream>

#include "DiffCommon.h"
#include "Expression.h"

#ifdef _OPENMP
#include <omp.h>
#endif

#ifdef MATAN_HAS_GINAC
#include <ginac/ginac.h>
#include <ginac/parser.h>
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

}  // namespace

std::string deriveSymbolicDerivative(const std::string& f_str) {
#ifndef MATAN_HAS_GINAC
  throw std::runtime_error(
      "Symbolic derivative unavailable: GiNaC not built in this configuration. "
      "Provide analytic derivative manually or rebuild with GiNaC enabled.");
#else
  GiNaC::symtab symbols;
  GiNaC::symbol x("x");
  symbols["x"] = x;

  GiNaC::parser p(symbols);
  GiNaC::ex f = p(f_str);
  GiNaC::ex df = f.diff(x);
  std::ostringstream oss;
  oss << df;
  return oss.str();
#endif
}

namespace {

Task2RmseRow computeRmseRow(const std::string& f_str, const std::string& df_str, double a, double b,
                            double h) {
  Expression f(f_str);
  Expression df(df_str);
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
    double d_true = df.eval(x[i]);

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

}  // namespace

Task2Results runAllDifferences(const std::string& f_str, double a, double b, double h) {
#ifndef MATAN_HAS_GINAC
  throw std::runtime_error(
      "Task2 requires symbolic derivative (GiNaC). Rebuild with MATAN_EMBED_GINAC=ON or provide "
      "GiNaC on the system.");
#endif
  std::string derivative_expr = deriveSymbolicDerivative(f_str);
  Expression f(f_str);
  Expression df(derivative_expr);
  auto grid = buildGrid(f, a, b, h);

  const auto& x = grid.x;
  const auto& y = grid.y;
  int n = static_cast<int>(x.size());

  Task2Results results;
  results.right.method = "right";
  results.left.method = "left";
  results.central.method = "central";
  results.derivative_expr = derivative_expr;
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
    double d_true = df.eval(xi);

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
#ifndef MATAN_HAS_GINAC
  throw std::runtime_error(
      "RMSE sweep requires symbolic derivative (GiNaC). Rebuild with MATAN_EMBED_GINAC=ON or "
      "provide GiNaC on the system.");
#endif
  if (steps <= 0) {
    return {};
  }
  std::string derivative_expr = deriveSymbolicDerivative(f_str);
  std::vector<Task2RmseRow> all;
  all.reserve(static_cast<std::size_t>(steps));
  double h = h0;
  for (int i = 0; i < steps; ++i) {
    all.push_back(computeRmseRow(f_str, derivative_expr, a, b, h));
    h *= 0.5;
  }
  return all;
}

}  // namespace matan
