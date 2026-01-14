#pragma once

#include <memory>
#include <string>

#include <exprtk.hpp>

namespace matan {

class Expression {
 public:
  explicit Expression(std::string expr);
  Expression(const Expression& other);
  Expression& operator=(const Expression& other);
  Expression(Expression&& other) noexcept;
  Expression& operator=(Expression&& other) noexcept;
  ~Expression();
  double eval(double x) const;
  double derivative(double x) const;

 private:
  void InitParser();
  std::string expr_;
  mutable double x_ = 0.0;
  std::unique_ptr<exprtk::parser<double>> parser_;
  mutable exprtk::symbol_table<double> symbol_table_;
  mutable exprtk::expression<double> exprtk_expr_;
};

}
