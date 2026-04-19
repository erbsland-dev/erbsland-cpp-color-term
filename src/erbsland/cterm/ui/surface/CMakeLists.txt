# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term PRIVATE
        AbstractStatusLine.cpp
        AbstractStatusLine.hpp
        Panel.cpp
        Panel.hpp
        ScrollingBufferView.cpp
        ScrollingBufferView.hpp
        StatusLine.cpp
        StatusLine.hpp
        TextBox.cpp
        TextBox.hpp
)
