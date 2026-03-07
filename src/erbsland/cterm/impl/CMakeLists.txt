cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term PRIVATE
        CommonBoxFrameStyle.cpp
        FontDefaultAscii.cpp
        TextUtil.hpp
        U8Buffer.hpp
        UnicodeWidth.cpp
        UnicodeWidth.hpp
        UnicodeWidthTable.cpp
)

if (WIN32)
    target_sources(erbsland-color-term PRIVATE
            InputWin32.cpp
            InputWin32.hpp
            TerminalWin32.cpp
    )
else ()
    target_sources(erbsland-color-term PRIVATE
            InputPosix.cpp
            InputPosix.hpp
            PosixKeyInputSession.cpp
            PosixKeyInputSession.hpp
            PosixKeyInputSessionStorage.cpp
            PosixKeyInputSessionStorage.hpp
            TerminalPosix.cpp
    )
endif ()
