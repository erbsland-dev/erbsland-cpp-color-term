# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term PRIVATE
        BlockStream.hpp
        BlockTarget.hpp
        BlockKind.hpp
        CursorBlockTarget.cpp
        CursorBlockTarget.hpp
        InlineTextRenderer.cpp
        InlineTextRenderer.hpp
        ListItemLayout.cpp
        ListItemLayout.hpp
        ListPrefix.hpp
        RenderContext.cpp
        RenderContext.hpp
        RenderBlock.cpp
        RenderBlock.hpp
        RenderToState.hpp
        StringBuilderWithWhitespaceState.cpp
        StringBuilderWithWhitespaceState.hpp
        StringBlockTarget.hpp
        StyleResolver.cpp
        StyleResolver.hpp
        TreeRenderer.cpp
        TreeRenderer.hpp
        WhitespaceMode.hpp
)
