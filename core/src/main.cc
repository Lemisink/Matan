#include <iostream>
#include <variant>

#include "Config.h"
#include "ResultWriter.h"
#include "Task2Runner.h"
#include "TaskFactory.h"

int main(int argc, char** argv) {
  const char* config_path = "config.ini";
  if (argc > 1) {
    config_path = argv[1];
  }

  try {
    matan::Config cfg = matan::Config::load(config_path);
    matan::TaskContext ctx;
    ctx.func = cfg.general.func;
    ctx.dfunc = cfg.task2.dfunc;
    ctx.a = cfg.general.a;
    ctx.b = cfg.general.b;
    ctx.eps = cfg.task1.eps;
    ctx.h = cfg.task2.h;

    auto task = matan::createTask(cfg);
    auto result = task->run(ctx);
    if (const auto* res_min = std::get_if<matan::MinimizationResult>(&result)) {
      matan::writeTask1Result(*res_min, ctx.func, ctx.a, ctx.b, cfg.output.data_dir);
    } else if (const auto* res_der = std::get_if<matan::DerivativeResult>(&result)) {
      matan::writeTask2Result(*res_der, cfg.output.data_dir);
      auto combined = matan::runAllDifferences(ctx.func, ctx.a, ctx.b, ctx.h);
      matan::writeTask2Combined(combined, cfg.output.data_dir);
      if (cfg.task2.rmse_sweep) {
        auto sweep = matan::runRmseSweep(ctx.func, ctx.a, ctx.b, ctx.h, 5);
        matan::writeTask2Rmse(sweep, cfg.output.data_dir);
      }
    }
  } catch (const std::exception& ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return 1;
  }

  return 0;
}
