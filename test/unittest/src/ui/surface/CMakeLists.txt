# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term-unittest PRIVATE
        ControlledTimeBackend.hpp
        RecordingSurface.hpp
        TestTextLine.hpp
        UiHelpSectionTest.cpp
        UiButtonTest.cpp
        UiKeyPressHandlingTest.cpp
        UiLayoutMetricsEditorTest.cpp
        UiPaintSystemTest.cpp
        UiScrollBarTest.cpp
        UiScrollingBufferViewTest.cpp
        UiSchedulerManualClock.hpp
        UiSchedulerTest.cpp
        UiTextLineTest.cpp
        UiSurfaceContainerTest.cpp
        UiThemeTest.cpp
        UiTextBoxTest.cpp
)
