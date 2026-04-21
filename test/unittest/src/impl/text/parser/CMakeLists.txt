# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term-unittest PRIVATE
        BufferedTokenizerTest.cpp
        BufferedTokenizerTestProbe.hpp
        HtmlParserTest.cpp
        HtmlParserTestSupport.hpp
        HtmlTokenizerExpectedToken.hpp
        HtmlTokenizerTest.cpp
        HtmlTokenizerTestSupport.hpp
        TokenizerTest.cpp
        TokenizerTestProbe.hpp
)
