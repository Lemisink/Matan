#pragma once

#include <memory>
#include <string>

namespace mu {
class Parser;
}

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

 private:
  void InitParser();
  std::string expr_;
  mutable double x_ = 0.0;
  std::unique_ptr<mu::Parser> parser_;
};

}  // namespace matan
