# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

set(_erbsland_color_term_unittest_target erbsland-color-term-unittest)

add_executable(${_erbsland_color_term_unittest_target})
erbsland_set_required_compiler_options(${_erbsland_color_term_unittest_target})
target_link_libraries(${_erbsland_color_term_unittest_target} PRIVATE erbsland-color-term)
target_include_directories(${_erbsland_color_term_unittest_target} PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        src
)

add_subdirectory(src)

erbsland_unittest(
        TARGET ${_erbsland_color_term_unittest_target}
)
