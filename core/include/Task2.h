#pragma once

#include "Task.h"

namespace matan {

class Task2Base : public Task {
 public:
  std::string name() const override {
    return "task2";
  }
};

class Task2Right final : public Task2Base {
 public:
  TaskResult run(const TaskContext& ctx) const override;
};

class Task2Left final : public Task2Base {
 public:
  TaskResult run(const TaskContext& ctx) const override;
};

class Task2Central final : public Task2Base {
 public:
  TaskResult run(const TaskContext& ctx) const override;
};

}
