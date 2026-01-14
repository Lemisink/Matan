#pragma once

#include "Task.h"

namespace matan {

class Task1Base : public Task {
 public:
  std::string name() const override {
    return "task1";
  }
};

class Task1Dichotomy final : public Task1Base {
 public:
  TaskResult run(const TaskContext& ctx) const override;
};

class Task1Golden final : public Task1Base {
 public:
  TaskResult run(const TaskContext& ctx) const override;
};

}
