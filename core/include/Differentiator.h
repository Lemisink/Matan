#pragma once

#include <string>
#include <vector>

class Expression;

namespace matan {

class Expression;

struct DifferentiationContext {
  const Expression& f;
  const Expression& df;
  double a = 0.0;
  double b = 0.0;
  double h = 0.0;
};

struct DerivativeSample {
  int i = 0;
  double x = 0.0;
  double fx = 0.0;
  double d_true = 0.0;
  double d_est = 0.0;
  double err = 0.0;
};

struct DerivativeResult {
  std::string method;
  double h = 0.0;
  double rmse = 0.0;
  std::vector<DerivativeSample> samples;
};

class Differentiator {
 public:
  virtual ~Differentiator() = default;
  virtual DerivativeResult differentiate(const DifferentiationContext& ctx) const = 0;

 protected:
  explicit Differentiator(std::string method_name);
  void logSample(DerivativeResult& result, int i, double x, double fx, double d_true,
                 double d_est) const;
  void finalize(DerivativeResult& result) const;

 private:
  std::string method_name_;
};

}  // namespace matan
