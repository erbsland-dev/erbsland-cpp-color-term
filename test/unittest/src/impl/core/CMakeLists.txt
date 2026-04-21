# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term-unittest PRIVATE
        CombinedCharTest.cpp
        HashHelperTest.cpp
        HashHelperTestSupport.hpp
        KeyDecoderTest.cpp
        StringBuilderTest.cpp
        U8BufferTest.cpp
        U8BufferTestSupport.hpp
        UnicodeWidthTest.cpp
)
