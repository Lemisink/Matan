#include "TaskFactory.h"

#include <stdexcept>

#include "Task1.h"
#include "Task2.h"

namespace matan {

std::unique_ptr<Task> createTask(const Config& cfg) {
  switch (cfg.general.task) {
    case TaskKind::Minimize:
      switch (cfg.task1.method) {
        case Task1Method::Dichotomy:
          return std::make_unique<Task1Dichotomy>();
        case Task1Method::Golden:
          return std::make_unique<Task1Golden>();
        default:
          throw std::runtime_error("Unknown method for task1");
      }
    case TaskKind::Differentiate:
      switch (cfg.task2.method) {
        case Task2Method::Right:
          return std::make_unique<Task2Right>();
        case Task2Method::Left:
          return std::make_unique<Task2Left>();
        case Task2Method::Central:
          return std::make_unique<Task2Central>();
        default:
          throw std::runtime_error("Unknown method for task2");
      }
    default:
      break;
  }

  throw std::runtime_error("Unknown task");
}

}  // namespace matan
