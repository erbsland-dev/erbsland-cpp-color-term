cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term PRIVATE
        BitmapPainter.cpp
        BitmapPainter.hpp
        CommonBoxFrameStyle.cpp
        FontDefaultAscii.cpp
        FramePainter.cpp
        FramePainter.hpp
        HashHelper.hpp
        InputBackend.cpp
        InputBackend.hpp
        LineBuffer.hpp
        ParagraphLayout.cpp
        ParagraphLayout.hpp
        ParagraphPainter.cpp
        ParagraphPainter.hpp
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
