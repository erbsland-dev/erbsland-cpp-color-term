# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

add_subdirectory(text_node_renderer)

target_sources(erbsland-color-term PRIVATE
        BufferedTokenizer.cpp
        BufferedTokenizer.hpp
        Generator.hpp
        HtmlParser.cpp
        HtmlParser.hpp
        HtmlToken.hpp
        HtmlTokenizer.cpp
        HtmlTokenizer.hpp
        HtmlTokenType.hpp
        Tokenizer.cpp
        Tokenizer.hpp
        TextNodeRenderer.cpp
        TextNodeRenderer.hpp
)
