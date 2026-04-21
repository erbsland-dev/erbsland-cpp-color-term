# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

add_executable(erbsland-color-term-unittest)
erbsland_set_required_compiler_options(erbsland-color-term-unittest)
target_link_libraries(erbsland-color-term-unittest PRIVATE erbsland-color-term)
target_include_directories(erbsland-color-term-unittest PRIVATE
        ${PROJECT_SOURCE_DIR}/demo
        ${PROJECT_SOURCE_DIR}/src
        src
)

add_subdirectory(src)

erbsland_unittest(
        TARGET erbsland-color-term-unittest
)
