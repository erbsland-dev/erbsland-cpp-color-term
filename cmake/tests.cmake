# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

if(ERBSLAND_COLOR_TERM_ENABLE_TESTS)
    add_subdirectory(test)

    enable_testing()
    add_test(
            NAME erbsland-color-term-unittest
            COMMAND $<TARGET_FILE:erbsland-color-term-unittest> --no-color
    )
endif()
