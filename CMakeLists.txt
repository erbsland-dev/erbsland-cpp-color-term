# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

project(erbsland-color-term LANGUAGES CXX)

# Option if unit tests should be enabled in this build.
# - Keep 'OFF' if you use this library.
# - Set of 'ON' if you are working on this library.
option(ERBSLAND_COLOR_TERM_ENABLE_TESTS "Enable unit tests" OFF)
option(ERBSLAND_COLOR_TERM_ENABLE_DEMOS "Build the demo applications" ${PROJECT_IS_TOP_LEVEL})

include(cmake/debug-warnings.cmake)
include(cmake/compiler-options.cmake)

add_library(erbsland-color-term STATIC)
add_library(erbsland-color-term::core ALIAS erbsland-color-term)
erbsland_set_required_compiler_options(erbsland-color-term)
erbsland_enable_debug_warnings(erbsland-color-term)

add_subdirectory(src/erbsland/cterm)

if (ERBSLAND_COLOR_TERM_ENABLE_DEMOS)
    add_subdirectory(demo)
endif ()

# Add the include path for integrations and installations
target_include_directories(erbsland-color-term
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/erbsland/cterm
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set(_erbsland_color_term_install_targets erbsland-color-term)

include(cmake/install.cmake)
include(cmake/tests.cmake)
