# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

add_executable(html-renderer-profile
        src/EmbeddedHtml.cpp
        src/main.cpp
)
target_compile_features(html-renderer-profile PRIVATE cxx_std_20)
target_link_libraries(html-renderer-profile
        PRIVATE
        erbsland-color-term
)
erbsland_set_required_compiler_options(html-renderer-profile)
erbsland_enable_debug_warnings(html-renderer-profile)
set_target_properties(html-renderer-profile PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/profiling-apps"
        FOLDER "Tests/Profiling"
)
