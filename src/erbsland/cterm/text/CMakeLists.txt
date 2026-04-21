# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

add_subdirectory(impl)

target_sources(erbsland-color-term PRIVATE
        HtmlRenderer.cpp
        HtmlRenderer.hpp
        StyleListKind.hpp
        StyleMarker.cpp
        StyleMarker.hpp
        StyleMatchContext.hpp
        StyleRole.hpp
        Style.cpp
        Style.hpp
        StyleSelector.hpp
        StyleRule.cpp
        StyleRule.hpp
        TextNode.cpp
        TextNode.hpp
        TextNodeType.cpp
        TextNodeType.hpp
)
