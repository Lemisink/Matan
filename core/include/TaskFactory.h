#pragma once

#include <memory>

#include "Config.h"
#include "Task.h"

namespace matan {

std::unique_ptr<Task> createTask(const Config& cfg);

}  // namespace matan
