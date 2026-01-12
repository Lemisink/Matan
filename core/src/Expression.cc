#include "Expression.h"

#include <muParser.h>

#include <cmath>
#include <stdexcept>
#include <utility>

namespace matan {

Expression::Expression(std::string expr)
    : expr_(std::move(expr)), parser_(std::make_unique<mu::Parser>()) {
  InitParser();
}

Expression::Expression(const Expression& other)
    : expr_(other.expr_), parser_(std::make_unique<mu::Parser>()) {
  InitParser();
}

Expression& Expression::operator=(const Expression& other) {
  if (this != &other) {
    expr_ = other.expr_;
    x_ = 0.0;
    parser_ = std::make_unique<mu::Parser>();
    InitParser();
  }
  return *this;
}

Expression::Expression(Expression&& other) noexcept
    : expr_(std::move(other.expr_)), parser_(std::make_unique<mu::Parser>()) {
  InitParser();
}

Expression& Expression::operator=(Expression&& other) noexcept {
  if (this != &other) {
    expr_ = std::move(other.expr_);
    x_ = 0.0;
    parser_ = std::make_unique<mu::Parser>();
    InitParser();
  }
  return *this;
}

Expression::~Expression() = default;

void Expression::InitParser() {
  try {
    parser_->DefineVar("x", &x_);
    parser_->SetExpr(expr_);
  } catch (const mu::Parser::exception_type& ex) {
    throw std::runtime_error(std::string("muParser error: ") + ex.GetMsg());
  }
}

double Expression::eval(double x) const {
  x_ = x;
  try {
    double v = parser_->Eval();
    if (!std::isfinite(v)) {
      throw std::runtime_error("Function is not finite at x=" + std::to_string(x_) +
                               ". Adjust the interval to avoid poles/singularities.");
    }
    return v;
  } catch (const mu::Parser::exception_type& ex) {
    throw std::runtime_error(std::string("muParser error: ") + ex.GetMsg());
  }
}

}  // namespace matan
