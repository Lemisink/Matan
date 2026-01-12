#pragma once

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>

namespace matan {

enum class TaskKind : int { Minimize = 1, Differentiate = 2 };

enum class Task1Method { Dichotomy, Golden };

enum class Task2Method { Right, Left, Central };

inline std::string toLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return s;
}

inline TaskKind parseTaskKind(int value) {
  switch (value) {
    case 1:
      return TaskKind::Minimize;
    case 2:
      return TaskKind::Differentiate;
    default:
      throw std::runtime_error("Unknown task: " + std::to_string(value));
  }
}

inline TaskKind parseTaskKind(const std::string& value) {
  std::string v = toLower(value);
  if (v == "1" || v == "task1" || v == "minimize" || v == "minimization") {
    return TaskKind::Minimize;
  }
  if (v == "2" || v == "task2" || v == "differentiate" || v == "differentiation" ||
      v == "derivative" || v == "diff") {
    return TaskKind::Differentiate;
  }
  throw std::runtime_error("Unknown task: " + value);
}

inline Task1Method parseTask1Method(const std::string& value) {
  std::string v = toLower(value);
  if (v == "dichotomy") {
    return Task1Method::Dichotomy;
  }
  if (v == "golden") {
    return Task1Method::Golden;
  }
  throw std::runtime_error("Unknown task1 method: " + value);
}

inline Task2Method parseTask2Method(const std::string& value) {
  std::string v = toLower(value);
  if (v == "right") {
    return Task2Method::Right;
  }
  if (v == "left") {
    return Task2Method::Left;
  }
  if (v == "central") {
    return Task2Method::Central;
  }
  throw std::runtime_error("Unknown task2 method: " + value);
}

inline std::string toString(TaskKind value) {
  switch (value) {
    case TaskKind::Minimize:
      return "minimize";
    case TaskKind::Differentiate:
      return "differentiate";
    default:
      return "unknown";
  }
}

inline std::string toString(Task1Method value) {
  switch (value) {
    case Task1Method::Dichotomy:
      return "dichotomy";
    case Task1Method::Golden:
      return "golden";
    default:
      return "unknown";
  }
}

inline std::string toString(Task2Method value) {
  switch (value) {
    case Task2Method::Right:
      return "right";
    case Task2Method::Left:
      return "left";
    case Task2Method::Central:
      return "central";
    default:
      return "unknown";
  }
}

}  // namespace matan
