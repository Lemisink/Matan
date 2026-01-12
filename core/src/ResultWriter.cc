#include "ResultWriter.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>

#include "Expression.h"

namespace matan {

namespace {

std::string methodSuffix(const std::string& method) {
  if (method.empty()) {
    return "unknown";
  }
  return method;
}

void ensureDir(const std::string& dir) {
  if (dir.empty()) {
    throw std::runtime_error("Output directory is empty");
  }
  std::filesystem::create_directories(dir);
}

bool startsWith(const std::string& value, const std::string& prefix) {
  return value.size() >= prefix.size() &&
         value.compare(0, prefix.size(), prefix) == 0;
}

void removeTaskFiles(const std::string& dir, const std::string& prefix) {
  for (const auto& entry : std::filesystem::directory_iterator(dir)) {
    if (!entry.is_regular_file()) {
      continue;
    }
    const std::string name = entry.path().filename().string();
    if (startsWith(name, prefix)) {
      std::filesystem::remove(entry.path());
    }
  }
}

}  // namespace

void writeTask1Result(const MinimizationResult& result, const std::string& func_expr, double a,
                      double b, const std::string& data_dir) {
  ensureDir(data_dir);
  removeTaskFiles(data_dir, "task1_");

  const std::string suffix = methodSuffix(result.method);
  const std::string func_path = data_dir + "/task1_func.dat";
  const std::string points_path = data_dir + "/task1_" + suffix + "_points.dat";
  const std::string interval_path = data_dir + "/task1_" + suffix + "_interval.dat";
  const std::string summary_path = data_dir + "/task1_summary.dat";

  Expression f(func_expr);

  {
    std::ofstream out(func_path);
    if (!out) {
      throw std::runtime_error("Failed to open " + func_path);
    }
    out << std::setprecision(17);
    const int samples = 400;
    const double step = (b - a) / static_cast<double>(samples);
    for (int i = 0; i <= samples; ++i) {
      double x = a + step * static_cast<double>(i);
      out << x << " " << f.eval(x) << "\n";
    }
  }

  {
    std::ofstream out(points_path);
    if (!out) {
      throw std::runtime_error("Failed to open " + points_path);
    }
    out << std::setprecision(17);
    for (const auto& it : result.iterations) {
      double fx = f.eval(it.x_star);
      out << it.k << " " << it.x_star << " " << fx << "\n";
    }
  }

  {
    std::ofstream out(interval_path);
    if (!out) {
      throw std::runtime_error("Failed to open " + interval_path);
    }
    out << std::setprecision(17);
    for (const auto& it : result.iterations) {
      out << it.k << " " << it.a << " " << it.b << " " << it.length << "\n";
    }
  }

  {
    std::ofstream out(summary_path);
    if (!out) {
      throw std::runtime_error("Failed to open " + summary_path);
    }
    out << std::setprecision(17) << result.x_min << " " << result.f_min << "\n";
  }
}

void writeTask2Result(const DerivativeResult& result, const std::string& data_dir) {
  ensureDir(data_dir);
  removeTaskFiles(data_dir, "task2_");

  const std::string suffix = methodSuffix(result.method);
  const std::string path = data_dir + "/task2_" + suffix + ".dat";

  std::ofstream out(path);
  if (!out) {
    throw std::runtime_error("Failed to open " + path);
  }
  out << std::setprecision(17);
  for (const auto& s : result.samples) {
    out << s.x << " " << s.fx << " " << s.d_true << " " << s.d_est << " " << s.err << "\n";
  }
}

void writeTask2Rmse(const std::vector<Task2RmseRow>& sweep, const std::string& data_dir) {
  ensureDir(data_dir);

  const std::string path = data_dir + "/task2_rmse.dat";
  std::ofstream out(path);
  if (!out) {
    throw std::runtime_error("Failed to open " + path);
  }
  out << std::setprecision(17);

  for (const auto& r : sweep) {
    out << r.h << " " << r.right << " " << r.left << " " << r.central << "\n";
  }
}

void writeTask2Combined(const Task2Results& results, const std::string& data_dir) {
  ensureDir(data_dir);

  const std::string path = data_dir + "/task2_all.dat";
  std::ofstream out(path);
  if (!out) {
    throw std::runtime_error("Failed to open " + path);
  }

  const auto& right = results.right.samples;
  const auto& left = results.left.samples;
  const auto& central = results.central.samples;

  if (right.size() != left.size() || right.size() != central.size()) {
    throw std::runtime_error("Mismatched sample sizes in task2 results");
  }

  out << std::setprecision(17);
  for (size_t i = 0; i < right.size(); ++i) {
    out << right[i].x << " " << right[i].fx << " " << right[i].d_true << " " << right[i].d_est
        << " " << left[i].d_est << " " << central[i].d_est << "\n";
  }
}

void writeTask2DerivativeExpr(const std::string& expr, const std::string& data_dir) {
  ensureDir(data_dir);
  const std::string path = data_dir + "/task2_derivative.txt";
  std::ofstream out(path);
  if (!out) {
    throw std::runtime_error("Failed to open " + path);
  }
  out << expr << "\n";
}

}  // namespace matan
