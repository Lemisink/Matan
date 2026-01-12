#include "Config.h"

#include <SimpleIni.h>

#include <stdexcept>
#include <string>

namespace matan {

Config Config::load(const std::string& path) {
  CSimpleIniA ini;
  ini.SetUnicode();
  SI_Error rc = ini.LoadFile(path.c_str());
  if (rc < 0) {
    throw std::runtime_error("Failed to open config file: " + path);
  }

  Config cfg;
  const char* task_value = ini.GetValue("general", "task", nullptr);
  if (task_value) {
    cfg.general.task = parseTaskKind(task_value);
  }
  cfg.general.func = ini.GetValue("general", "func", cfg.general.func.c_str());
  cfg.general.a = ini.GetDoubleValue("general", "a", cfg.general.a);
  cfg.general.b = ini.GetDoubleValue("general", "b", cfg.general.b);

  const char* task1_method = ini.GetValue("task1", "method", nullptr);
  if (task1_method) {
    cfg.task1.method = parseTask1Method(task1_method);
  }
  cfg.task1.eps = ini.GetDoubleValue("task1", "eps", cfg.task1.eps);

  const char* task2_method = ini.GetValue("task2", "method", nullptr);
  if (task2_method) {
    cfg.task2.method = parseTask2Method(task2_method);
  }
  const char* task2_dfunc = ini.GetValue("task2", "dfunc", nullptr);
  if (task2_dfunc) {
    cfg.task2.dfunc = task2_dfunc;
  }
  cfg.task2.h = ini.GetDoubleValue("task2", "h", cfg.task2.h);
  cfg.task2.rmse_sweep = ini.GetBoolValue("task2", "rmse_sweep", cfg.task2.rmse_sweep);

  cfg.output.data_dir = ini.GetValue("output", "data_dir", cfg.output.data_dir.c_str());

  return cfg;
}

}  // namespace matan
