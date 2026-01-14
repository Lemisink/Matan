#include "Expression.h"

#include <exprtk.hpp>

#include <cmath>
#include <stdexcept>
#include <utility>

namespace matan {

Expression::Expression(std::string expr)
    : expr_(std::move(expr)), parser_(std::make_unique<exprtk::parser<double>>()) {
  InitParser();
}

Expression::Expression(const Expression& other)
    : expr_(other.expr_), parser_(std::make_unique<exprtk::parser<double>>()) {
  InitParser();
}

Expression& Expression::operator=(const Expression& other) {
  if (this != &other) {
    expr_ = other.expr_;
    x_ = 0.0;
    parser_ = std::make_unique<exprtk::parser<double>>();
    InitParser();
  }
  return *this;
}

Expression::Expression(Expression&& other) noexcept
    : expr_(std::move(other.expr_)), parser_(std::make_unique<exprtk::parser<double>>()) {
  InitParser();
}

Expression& Expression::operator=(Expression&& other) noexcept {
  if (this != &other) {
    expr_ = std::move(other.expr_);
    x_ = 0.0;
    parser_ = std::make_unique<exprtk::parser<double>>();
    InitParser();
  }
  return *this;
}

Expression::~Expression() = default;

void Expression::InitParser() {
  symbol_table_.clear();
  symbol_table_.add_variable("x", x_);
  exprtk_expr_.register_symbol_table(symbol_table_);
  bool ok = parser_->compile(expr_, exprtk_expr_);
  if (!ok) {
    throw std::runtime_error("exprtk parse error");
  }
}

double Expression::eval(double x) const {
  x_ = x;
  double v = exprtk_expr_.value();
  if (!std::isfinite(v)) {
    throw std::runtime_error("Function is not finite at x=" + std::to_string(x_) +
                             ". Adjust the interval to avoid poles/singularities.");
  }
  return v;
}

double Expression::derivative(double x) const {
  x_ = x;
  double d =
      exprtk::derivative(exprtk_expr_, "x", std::max(1e-8, std::abs(x_) * 1e-4 + 1e-6));
  if (!std::isfinite(d)) {
    throw std::runtime_error("Derivative is not finite at x=" + std::to_string(x) +
                             ". Adjust the interval to avoid poles/singularities.");
  }
  return d;
}

}
