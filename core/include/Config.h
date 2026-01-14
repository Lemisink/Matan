#pragma once

#include <string>

#include "TaskTypes.h"

namespace matan {

struct Config {
  struct General {
    TaskKind task = TaskKind::Minimize;
    std::string func = "sin(x) + x^2";
    double a = -2.0;
    double b = 2.0;
  } general;

  struct Task1 {
    Task1Method method = Task1Method::Golden;
    double eps = 1e-4;
  } task1;

  struct Task2 {
    Task2Method method = Task2Method::Central;
    std::string dfunc;
    double h = 0.1;
    bool rmse_sweep = true;
  } task2;

  struct Output {
    std::string data_dir = "data";
  } output;

  static Config load(const std::string& path);
};

}
