# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term PRIVATE
        BlockRole.hpp
        Element.hpp
        Identifier.hpp
        Part.hpp
        Properties.cpp
        Properties.hpp
        PropertyEditor.cpp
        PropertyEditor.hpp
        PropertySheet.cpp
        PropertySheet.hpp
        Selector.cpp
        Selector.hpp
        State.hpp
        States.cpp
        States.hpp
        Tag.hpp
        Tags.hpp
        Theme.cpp
        Theme.hpp
        ThemeAccessor.cpp
        ThemeAccessor.hpp
        ThemeBuilder.cpp
        ThemeBuilder.hpp
        ThemePainter.cpp
        ThemePainter.hpp
        impl/ElementBaseManager.cpp
        impl/ElementBaseManager.hpp
)
