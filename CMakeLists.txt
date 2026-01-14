cmake_minimum_required(VERSION 3.16)
project(matan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(MATAN_BUILD_EXPR_DEMO "Build expression parser demo" ON)

set(MATAN_CORE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/core")
set(MATAN_DIST_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dist/bin")
file(MAKE_DIRECTORY "${MATAN_DIST_BIN_DIR}")

if (MSVC)
  set(MATAN_WARN_FLAGS /W4 /permissive- /bigobj)
else()
  set(MATAN_WARN_FLAGS -Wall -Wextra -Wpedantic)
endif()

find_package(OpenMP)

function(matan_set_common target)
  target_include_directories(${target} PUBLIC "${MATAN_CORE_DIR}/include")
  target_compile_options(${target} PRIVATE ${MATAN_WARN_FLAGS})
endfunction()

add_library(matan_expr
  ${MATAN_CORE_DIR}/src/Expression.cc
)
target_include_directories(matan_expr PUBLIC
  "${MATAN_CORE_DIR}/include"
  "${MATAN_CORE_DIR}/external"
)
target_compile_options(matan_expr PRIVATE ${MATAN_WARN_FLAGS})

add_library(matan_minimize
  ${MATAN_CORE_DIR}/src/Minimizer.cc
  ${MATAN_CORE_DIR}/src/DichotomyMinimizer.cc
  ${MATAN_CORE_DIR}/src/GoldenSectionMinimizer.cc
)
matan_set_common(matan_minimize)
target_link_libraries(matan_minimize PUBLIC matan_expr)

add_library(matan_diff
  ${MATAN_CORE_DIR}/src/Differentiator.cc
  ${MATAN_CORE_DIR}/src/RightDifference.cc
  ${MATAN_CORE_DIR}/src/LeftDifference.cc
  ${MATAN_CORE_DIR}/src/CentralDifference.cc
  ${MATAN_CORE_DIR}/src/Task2Runner.cc
)
matan_set_common(matan_diff)
target_link_libraries(matan_diff PUBLIC matan_expr)
if (OpenMP_CXX_FOUND)
  target_link_libraries(matan_diff PRIVATE OpenMP::OpenMP_CXX)
endif()

add_library(matan_tasks
  ${MATAN_CORE_DIR}/src/Task1.cc
  ${MATAN_CORE_DIR}/src/Task2.cc
  ${MATAN_CORE_DIR}/src/TaskFactory.cc
)
matan_set_common(matan_tasks)
target_link_libraries(matan_tasks PUBLIC matan_minimize matan_diff)

add_library(matan_config
  ${MATAN_CORE_DIR}/src/Config.cc
)
matan_set_common(matan_config)
target_include_directories(matan_config PRIVATE "${MATAN_CORE_DIR}/external/simpleini")

add_library(matan_io
  ${MATAN_CORE_DIR}/src/ResultWriter.cc
)
matan_set_common(matan_io)
target_link_libraries(matan_io PRIVATE matan_expr)

add_executable(matan_app
  ${MATAN_CORE_DIR}/src/main.cc
)
target_compile_options(matan_app PRIVATE ${MATAN_WARN_FLAGS})
target_link_libraries(matan_app PRIVATE matan_config matan_tasks matan_io)
set_target_properties(matan_app PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${MATAN_DIST_BIN_DIR}"
)
foreach(cfg DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
  set_target_properties(matan_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_${cfg} "${MATAN_DIST_BIN_DIR}"
  )
endforeach()

if (MATAN_BUILD_EXPR_DEMO)
  add_executable(expr_demo ${MATAN_CORE_DIR}/src/expr_demo.cc)
  target_link_libraries(expr_demo PRIVATE matan_expr)
  target_compile_options(expr_demo PRIVATE ${MATAN_WARN_FLAGS})
endif()
