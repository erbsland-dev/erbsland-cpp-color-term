# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

set(_erbsland_color_term_unittest_target erbsland-color-term-unittest)

target_sources(${_erbsland_color_term_unittest_target} PRIVATE
        BufferedTokenizerTest.cpp
        CombinedCharTest.cpp
        HashHelperTest.cpp
        HtmlRendererTest.cpp
        HtmlParserTest.cpp
        HtmlTokenizerTest.cpp
        InlineTextPlannerTest.cpp
        KeyDecoderTest.cpp
        ListBlockPlannerTest.cpp
        LayoutLineTokenTest.cpp
        LayoutTest.cpp
        PlanningContextTest.cpp
        PlanningStyleResolverTest.cpp
        StructuredTextParserTest.cpp
        TextNodePlanningQueriesTest.cpp
        U8BufferTest.cpp
        UnicodeWidthTest.cpp
)
