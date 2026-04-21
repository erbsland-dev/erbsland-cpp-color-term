# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term-unittest PRIVATE
        BufferTestHelper.hpp
        CursorBufferTestBase.hpp
        StringTestHelper.hpp
        TerminalTestBackend.hpp
        TerminalTestCursorMove.hpp
        TerminalTestHelper.hpp
        TestHelper.hpp
        main.cpp
)
