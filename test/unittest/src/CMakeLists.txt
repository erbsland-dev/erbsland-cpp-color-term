cmake_minimum_required(VERSION 3.28)

set(_erbsland_color_term_unittest_target erbsland-color-term-unittest)

target_sources(${_erbsland_color_term_unittest_target} PRIVATE
        ApiWrapperDispatchTest.cpp
        AlignmentTest.cpp
        AnchorTest.cpp
        BackendTest.cpp
        BitmapTest.cpp
        BitmapDrawOptionsTest.cpp
        BlockBufferTest.cpp
        BufferConvenienceTest.cpp
        BufferViewTest.cpp
        Char16StyleTest.cpp
        CharCombinationStyleTest.cpp
        CharTest.cpp
        ColorParsingTest.cpp
        ColorSequenceTest.cpp
        ColorTest.cpp
        CropEdgesTest.cpp
        DirectionTest.cpp
        FontTest.cpp
        FontAccessTest.cpp
        FrameDrawOptionsTest.cpp
        HashHelperTest.cpp
        InputDefinitionTest.cpp
        KeyPressTest.cpp
        main.cpp
        MarginsTest.cpp
        ParagraphOptionsTest.cpp
        PositionTest.cpp
        RectangleTest.cpp
        RemappedBufferTest.cpp
        SizeTest.cpp
        StringConversionTest.cpp
        TerminalTest.cpp
        TerminalFlagsTest.cpp
        TerminalTestSupport.hpp
        TermStringTest.cpp
        TerminalConvenienceTest.cpp
        TestHelper.hpp
        TextRenderTest.cpp
        Tile9StyleTest.cpp
        UnicodeWidthTest.cpp
        UpdateSettingsTest.cpp
        UpdateScreenModesStateTest.cpp
)
