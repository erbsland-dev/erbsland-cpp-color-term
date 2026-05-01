cmake_minimum_required(VERSION 3.28)

add_subdirectory(paragraph)

target_sources(erbsland-color-term PRIVATE
        BitmapPainter.cpp
        BitmapPainter.hpp
        CombinedChar.cpp
        CombinedChar.hpp
        CommonBoxFrameCombinationStyle.cpp
        CommonBoxFrameCombinationStyle.hpp
        CommonBoxFrameCombinationStyle_data.cpp
        FontDefaultAscii.cpp
        FrameBorder_data.hpp
        FramePainter.cpp
        FramePainter.hpp
        HashHelper.hpp
        InputBackend.cpp
        InputBackend.hpp
        KeyDecoder.cpp
        KeyDecoder.hpp
        LineBuffer.hpp
        SaturatingMath.hpp
        StringBuilder.hpp
        StringData.cpp
        StringData.hpp
        StringRangeView.cpp
        StringRangeView.hpp
        StringWrapper.cpp
        StringWrapper.hpp
        TextPainter.cpp
        TextPainter.hpp
        TextUtil.hpp
        TypeTraits.hpp
        U8Buffer.hpp
        UnicodeWidth.cpp
        UnicodeWidth.hpp
        UnicodeWidthTable.cpp
)

if (WIN32)
    target_sources(erbsland-color-term PRIVATE
            WindowsBackend.cpp
            WindowsBackend.hpp
            WindowsSignalDispatcher.cpp
            WindowsSignalDispatcher.hpp
    )
else ()
    target_sources(erbsland-color-term PRIVATE
            PosixBackend.cpp
            PosixBackend.hpp
            PosixSignalDispatcher.cpp
            PosixSignalDispatcher.hpp
    )
endif ()
