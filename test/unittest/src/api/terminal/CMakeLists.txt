# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term-unittest PRIVATE
        ApiWrapperDispatchTest.cpp
        BackendTest.cpp
        InputDefinitionTest.cpp
        InputDispatchProbe.hpp
        InputTest.cpp
        InputTestProbe.hpp
        KeyModifiersTest.cpp
        KeyTest.cpp
        KeysTest.cpp
        ReadableBufferDispatchProbe.hpp
        TerminalConvenienceTest.cpp
        TerminalFlagsTest.cpp
        TerminalTest.cpp
        WritableBufferDispatchProbe.hpp
)
