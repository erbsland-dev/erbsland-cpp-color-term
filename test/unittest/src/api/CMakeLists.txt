# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

set(_erbsland_color_term_unittest_target erbsland-color-term-unittest)

target_sources(${_erbsland_color_term_unittest_target} PRIVATE
        AlignmentTest.cpp
        AnchorTest.cpp
        ApiWrapperDispatchTest.cpp
        BackendTest.cpp
        BitmapDrawOptionsTest.cpp
        BitmapTest.cpp
        BufferDrawOptionsTest.cpp
        BufferConvenienceTest.cpp
        BufferTest.cpp
        BufferViewTest.cpp
        Char16StyleTest.cpp
        CharAttributesTest.cpp
        CharCombinationStyleTest.cpp
        CharStyleTest.cpp
        CharTest.cpp
        ColorParsingTest.cpp
        ColorSequenceTest.cpp
        ColorTest.cpp
        CropEdgesTest.cpp
        CursorWriterTest.cpp
        CursorBufferTest.cpp
        DirectionTest.cpp
        FontAccessTest.cpp
        FontTest.cpp
        FastCharSetTest.cpp
        FrameDrawOptionsTest.cpp
        InputTest.cpp
        InputDefinitionTest.cpp
        KeyTest.cpp
        MarginsTest.cpp
        OrientationTest.cpp
        ParagraphIndentsTest.cpp
        ParagraphOptionsTest.cpp
        PositionTest.cpp
        RectangleTest.cpp
        RemappedBufferTest.cpp
        SizeTest.cpp
        StringConversionTest.cpp
        StringTest.cpp
        TerminalConvenienceTest.cpp
        TerminalFlagsTest.cpp
        TerminalTest.cpp
        TextOptionsTest.cpp
        TextStyleTest.cpp
        TextTest.cpp
        TextRenderTest.cpp
        Tile9StyleTest.cpp
        UpdateSettingsTest.cpp
)
