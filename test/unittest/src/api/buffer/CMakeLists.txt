# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term-unittest PRIVATE
        BitmapAccessor.hpp
        BitmapDrawOptionsTest.cpp
        BitmapTest.cpp
        BitmapTestSupport.hpp
        BufferConvenienceTest.cpp
        BufferDrawOptionsTest.cpp
        BufferTest.cpp
        BufferViewTest.cpp
        WriteClippedBufferTest.cpp
        CursorBufferTest.cpp
        CursorWriterTest.cpp
        CursorWriterTestProbe.hpp
        FastCharSetTest.cpp
        ReferenceBuffer.hpp
        RemappedBufferTest.cpp
        RemappedBufferTestSupport.hpp
)
