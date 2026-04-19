# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

set(_erbsland_color_term_unittest_target erbsland-color-term-unittest)

target_sources(${_erbsland_color_term_unittest_target} PRIVATE
        UiKeyPressHandlingTest.cpp
        UiScrollingBufferViewTest.cpp
        UiSchedulerTest.cpp
        UiStatusLineTest.cpp
        UiTextBoxTest.cpp
)
