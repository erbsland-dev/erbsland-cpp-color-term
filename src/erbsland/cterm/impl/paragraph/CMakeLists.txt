cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term PRIVATE
        LayoutContext.cpp
        LayoutContext.hpp
        LineBuilder.cpp
        LineBuilder.hpp
        Layout.cpp
        Layout.hpp
        LayoutFragment.hpp
        LayoutFragments.hpp
        LayoutLine.hpp
        LayoutLineToken.cpp
        LayoutLineToken.hpp
        LayoutNewlineMode.hpp
        LayoutPreparedSourceLine.hpp
        LayoutResult.cpp
        LayoutResult.hpp
        LayoutSourceLineRange.hpp
        Painter.cpp
        Painter.hpp
        Printer.cpp
        Printer.hpp
        RendererBase.cpp
        RendererBase.hpp
)
