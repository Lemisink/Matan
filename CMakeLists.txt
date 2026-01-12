cmake_minimum_required(VERSION 3.16)
project(matan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    set(MATAN_WARN_FLAGS /W4 /permissive-)
else()
    set(MATAN_WARN_FLAGS -Wall -Wextra -Wpedantic)
endif()

set(MATAN_CORE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/core")

add_library(matan_expr
    ${MATAN_CORE_DIR}/src/Expression.cc
)

target_include_directories(matan_expr PUBLIC
    ${MATAN_CORE_DIR}/include
)

set(ENABLE_SAMPLES OFF CACHE BOOL "" FORCE)
set(ENABLE_OPENMP OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

add_subdirectory(${MATAN_CORE_DIR}/external/muparser EXCLUDE_FROM_ALL)
target_link_libraries(matan_expr PRIVATE muparser)

target_compile_options(matan_expr PRIVATE ${MATAN_WARN_FLAGS})

add_library(matan_minimize
    ${MATAN_CORE_DIR}/src/Minimizer.cc
    ${MATAN_CORE_DIR}/src/DichotomyMinimizer.cc
    ${MATAN_CORE_DIR}/src/GoldenSectionMinimizer.cc
)

target_include_directories(matan_minimize PUBLIC
    ${MATAN_CORE_DIR}/include
)
target_link_libraries(matan_minimize PUBLIC matan_expr)
target_compile_options(matan_minimize PRIVATE ${MATAN_WARN_FLAGS})

find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found: ${OpenMP_CXX_VERSION}")
else()
    message(STATUS "OpenMP not found: building without parallel sections")
endif()

include(ExternalProject)

option(MATAN_EMBED_GINAC "Build GiNaC from source (external)" OFF)
set(MATAN_GINAC_PREFIX "${CMAKE_BINARY_DIR}/external/ginac")
set(MATAN_GINAC_SRC "${MATAN_CORE_DIR}/external/ginac")
set(MATAN_CLN_SRC "${MATAN_CORE_DIR}/external/cln")

if (MATAN_EMBED_GINAC)
    set(CLNN_USE_GMP ON)
    if (WIN32)
        # Avoid GMP dependency on Windows toolchains to simplify setup
        set(CLNN_USE_GMP OFF)
        set(MATAN_MSVC_INLINE_FLAG "/Zc:inline")
    endif()
    set(CLNN_INSTALL_DIR "${MATAN_GINAC_PREFIX}/cln/install")
    ExternalProject_Add(cln_ext
        PREFIX ${MATAN_GINAC_PREFIX}/cln
        SOURCE_DIR ${MATAN_CLN_SRC}
        BINARY_DIR ${MATAN_GINAC_PREFIX}/cln/build
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${CLNN_INSTALL_DIR}
            -DBUILD_SHARED_LIBS=ON
            -DCLN_USE_GMP=${CLNN_USE_GMP}
            $<$<BOOL:${WIN32}>:-DCMAKE_CXX_FLAGS=${MATAN_MSVC_INLINE_FLAG}>
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR>
        INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR>
    )

    set(GINAC_INSTALL_DIR "${MATAN_GINAC_PREFIX}/ginac/install")
    ExternalProject_Add(ginac_ext
        DEPENDS cln_ext
        PREFIX ${MATAN_GINAC_PREFIX}/ginac
        SOURCE_DIR ${MATAN_GINAC_SRC}
        BINARY_DIR ${MATAN_GINAC_PREFIX}/ginac/build
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${GINAC_INSTALL_DIR}
            -DBUILD_SHARED_LIBS=ON
            -DCMAKE_POLICY_VERSION_MINIMUM=3.5
            -DCMAKE_PREFIX_PATH=${CLNN_INSTALL_DIR}
            -DCLN_DIR=${CLNN_INSTALL_DIR}/lib/cmake/cln
            -Dcln_DIR=${CLNN_INSTALL_DIR}/lib/cmake/cln
            -DCLN_INCLUDE_DIR=${CLNN_INSTALL_DIR}/include
            -DCLN_LIBRARY=${CLNN_INSTALL_DIR}/lib/libcln.so
            -DCLN_LIBRARIES=${CLNN_INSTALL_DIR}/lib/libcln.so
            -DCLN_CONFIG_EXECUTABLE=${CLNN_INSTALL_DIR}/bin/cln-config
            $<$<BOOL:${WIN32}>:-DCMAKE_CXX_FLAGS=${MATAN_MSVC_INLINE_FLAG}>
        BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR>
        INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR>
    )
    set(MATAN_EMBED_LIB_DIRS "${GINAC_INSTALL_DIR}/lib;${CLNN_INSTALL_DIR}/lib")
endif()


add_library(matan_diff
    ${MATAN_CORE_DIR}/src/Differentiator.cc
    ${MATAN_CORE_DIR}/src/RightDifference.cc
    ${MATAN_CORE_DIR}/src/LeftDifference.cc
    ${MATAN_CORE_DIR}/src/CentralDifference.cc
    ${MATAN_CORE_DIR}/src/Task2Runner.cc
)

target_include_directories(matan_diff PUBLIC
    ${MATAN_CORE_DIR}/include
)
target_link_libraries(matan_diff PUBLIC matan_expr)
if (MATAN_EMBED_GINAC)
    target_compile_definitions(matan_diff PUBLIC MATAN_HAS_GINAC=1)
endif()
if (MATAN_EMBED_GINAC)
    add_dependencies(matan_diff ginac_ext)
    target_include_directories(matan_diff PRIVATE
        ${GINAC_INSTALL_DIR}/include
        ${CLNN_INSTALL_DIR}/include
    )
target_link_libraries(matan_diff PUBLIC
        ${GINAC_INSTALL_DIR}/lib/libginac.so
        ${CLNN_INSTALL_DIR}/lib/libcln.so
        gmp
    )
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GINAC REQUIRED ginac)
    target_include_directories(matan_diff PRIVATE ${GINAC_INCLUDE_DIRS})
    target_link_libraries(matan_diff PUBLIC ${GINAC_LIBRARIES})
endif()
if (OpenMP_CXX_FOUND)
    target_link_libraries(matan_diff PRIVATE OpenMP::OpenMP_CXX)
endif()
target_compile_options(matan_diff PRIVATE ${MATAN_WARN_FLAGS})

add_library(matan_tasks
    ${MATAN_CORE_DIR}/src/Task1.cc
    ${MATAN_CORE_DIR}/src/Task2.cc
    ${MATAN_CORE_DIR}/src/TaskFactory.cc
)

target_include_directories(matan_tasks PUBLIC
    ${MATAN_CORE_DIR}/include
)
target_link_libraries(matan_tasks PUBLIC matan_minimize matan_diff)
target_compile_options(matan_tasks PRIVATE ${MATAN_WARN_FLAGS})

add_library(matan_config
    ${MATAN_CORE_DIR}/src/Config.cc
)

target_include_directories(matan_config PUBLIC
    ${MATAN_CORE_DIR}/include
)
target_include_directories(matan_config PRIVATE
    ${MATAN_CORE_DIR}/external/simpleini
)
target_compile_options(matan_config PRIVATE ${MATAN_WARN_FLAGS})

add_library(matan_io
    ${MATAN_CORE_DIR}/src/ResultWriter.cc
)

target_include_directories(matan_io PUBLIC
    ${MATAN_CORE_DIR}/include
)
target_link_libraries(matan_io PRIVATE matan_expr)
target_compile_options(matan_io PRIVATE ${MATAN_WARN_FLAGS})

add_executable(matan_app
    ${MATAN_CORE_DIR}/src/main.cc
)

target_link_libraries(matan_app PRIVATE matan_config matan_tasks matan_io)
target_compile_options(matan_app PRIVATE ${MATAN_WARN_FLAGS})

set(MATAN_DIST_BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dist/bin")
file(MAKE_DIRECTORY "${MATAN_DIST_BIN_DIR}")
set_target_properties(matan_app PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${MATAN_DIST_BIN_DIR}"
)
foreach(cfg DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(matan_app PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${cfg} "${MATAN_DIST_BIN_DIR}"
    )
endforeach()
if (MATAN_EMBED_GINAC)
    set_target_properties(matan_app PROPERTIES
        BUILD_RPATH "${MATAN_EMBED_LIB_DIRS}"
    )
endif()


option(MATAN_BUILD_EXPR_DEMO "Build expression parser demo" ON)
if (MATAN_BUILD_EXPR_DEMO)
    add_executable(expr_demo ${MATAN_CORE_DIR}/src/expr_demo.cc)
    target_link_libraries(expr_demo PRIVATE matan_expr)
    if (MATAN_EMBED_GINAC)
        set_target_properties(expr_demo PROPERTIES
            BUILD_RPATH "${MATAN_EMBED_LIB_DIRS}"
        )
    endif()
endif()

option(MATAN_ENABLE_TAURI "Enable Tauri build targets" OFF)
if (MATAN_ENABLE_TAURI)
    set(MATAN_UI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/apps/desktop")
    find_program(NPM_EXECUTABLE npm)
    if (NOT NPM_EXECUTABLE)
        message(WARNING "npm not found; skipping Tauri targets")
    else()
        set(MATAN_UI_INSTALL_STAMP "${MATAN_UI_DIR}/node_modules/.install-stamp")
        add_custom_command(
            OUTPUT "${MATAN_UI_INSTALL_STAMP}"
            COMMAND ${CMAKE_COMMAND} -E chdir "${MATAN_UI_DIR}" "${NPM_EXECUTABLE}" install
            COMMAND ${CMAKE_COMMAND} -E touch "${MATAN_UI_INSTALL_STAMP}"
            COMMENT "Installing UI dependencies"
        )
        add_custom_target(matan_ui_deps DEPENDS "${MATAN_UI_INSTALL_STAMP}")

        add_custom_target(matan_ui_build
            COMMAND ${CMAKE_COMMAND} -E chdir "${MATAN_UI_DIR}" "${NPM_EXECUTABLE}" run build
            DEPENDS matan_ui_deps
            COMMENT "Building UI"
        )

        add_custom_target(matan_tauri_build
            COMMAND ${CMAKE_COMMAND} -E chdir "${MATAN_UI_DIR}" ${CMAKE_COMMAND} -E env NO_STRIP=1 "${NPM_EXECUTABLE}" run tauri build -- --bundles appimage
            DEPENDS matan_app matan_ui_build
            COMMENT "Building Tauri AppImage bundle"
        )

        add_custom_target(matan_tauri_dev
            COMMAND ${CMAKE_COMMAND} -E chdir "${MATAN_UI_DIR}" "${NPM_EXECUTABLE}" run tauri dev
            DEPENDS matan_app matan_ui_deps
            COMMENT "Running Tauri dev server"
        )

        # Aggregate target to build everything with a single cmake --build
        add_custom_target(matan_all ALL
            DEPENDS matan_tauri_build
            COMMENT "Building full stack (C++ core + Tauri AppImage)"
        )
    endif()
endif()
