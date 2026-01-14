#pragma once

#include <string>
#include <vector>

#include "Differentiator.h"
#include "Minimizer.h"
#include "Task2Runner.h"

namespace matan {

void writeTask1Result(const MinimizationResult& result, const std::string& func_expr, double a,
                      double b, const std::string& data_dir);

void writeTask2Result(const DerivativeResult& result, const std::string& data_dir);

void writeTask2Rmse(const std::vector<Task2RmseRow>& sweep, const std::string& data_dir);

void writeTask2Combined(const Task2Results& results, const std::string& data_dir);

}
