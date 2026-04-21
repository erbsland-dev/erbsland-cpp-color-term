# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term PRIVATE
        AbstractLine.cpp
        AbstractLine.hpp
        AbstractHelpSection.cpp
        AbstractHelpSection.hpp
        AbstractScrollBar.cpp
        AbstractScrollBar.hpp
        AbstractScrollArea.cpp
        AbstractScrollArea.hpp
        ActionHelp.cpp
        ActionHelp.hpp
        ActionHelpSection.cpp
        ActionHelpSection.hpp
        Button.cpp
        Button.hpp
        DynamicText.cpp
        DynamicText.hpp
        FooterLine.cpp
        FooterLine.hpp
        HeaderLine.cpp
        HeaderLine.hpp
        HtmlHelpSection.cpp
        HtmlHelpSection.hpp
        HorizontalScrollBar.cpp
        HorizontalScrollBar.hpp
        Panel.cpp
        Panel.hpp
        ScrollCorner.cpp
        ScrollCorner.hpp
        ScrollingBufferView.cpp
        ScrollingBufferView.hpp
        TextLine.cpp
        TextLine.hpp
        TextBox.cpp
        TextBox.hpp
        VerticalScrollBar.cpp
        VerticalScrollBar.hpp
)
