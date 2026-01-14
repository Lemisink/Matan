#pragma once

#include <string>
#include <vector>

namespace matan {

class Expression;

struct MinimizationContext {
  const Expression& f;
  double a = 0.0;
  double b = 0.0;
  double eps = 0.0;
};

struct IterationState {
  int k = 0;
  double a = 0.0;
  double b = 0.0;
  double y = 0.0;
  double z = 0.0;
  double fy = 0.0;
  double fz = 0.0;
  double x_star = 0.0;
  double length = 0.0;
};

struct MinimizationResult {
  double x_min = 0.0;
  double f_min = 0.0;
  std::string method;
  std::vector<IterationState> iterations;
};

class Minimizer {
 public:
  virtual ~Minimizer() = default;
  virtual MinimizationResult minimize(const MinimizationContext& ctx) const = 0;

 protected:
  explicit Minimizer(std::string method_name);
  void logIteration(MinimizationResult& result, int k, double a, double b, double y, double z,
                    double fy, double fz) const;

 private:
  std::string method_name_;
};

}
