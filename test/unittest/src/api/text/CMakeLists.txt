# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term-unittest PRIVATE
        CharTest.cpp
        FontAccessTest.cpp
        FontTest.cpp
        ParagraphIndentsTest.cpp
        ParagraphOptionsTest.cpp
        ParagraphRenderTest.cpp
        StringConversionTest.cpp
        StringTest.cpp
        StringViewTest.cpp
        TextOptionsTest.cpp
        TextRenderTest.cpp
        TextStyleTest.cpp
        TextStyleTestSupport.hpp
        TextTest.cpp
)
