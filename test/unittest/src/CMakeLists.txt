cmake_minimum_required(VERSION 3.28)

set(_erbsland_color_term_unittest_target erbsland-color-term-unittest)

target_sources(${_erbsland_color_term_unittest_target} PRIVATE
        AlignmentTest.cpp
        AnchorTest.cpp
        BitmapTest.cpp
        BlockBufferTest.cpp
        Char16StyleTest.cpp
        CharCombinationStyleTest.cpp
        CharTest.cpp
        ColorSequenceTest.cpp
        ColorTest.cpp
        DirectionTest.cpp
        FontTest.cpp
        InputDefinitionTest.cpp
        KeyPressTest.cpp
        main.cpp
        MarginsTest.cpp
        PositionTest.cpp
        RectangleTest.cpp
        SizeTest.cpp
        TermStringTest.cpp
        TerminalUpdateTest.cpp
        TestHelper.hpp
        TextRenderTest.cpp
        Tile9StyleTest.cpp
        UnicodeWidthTest.cpp
)
