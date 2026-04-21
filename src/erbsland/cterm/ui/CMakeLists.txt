# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

add_subdirectory(event)
add_subdirectory(layout)
add_subdirectory(page)
add_subdirectory(surface)

target_sources(erbsland-color-term PRIVATE
        AbstractSurfaceContainer.hpp
        Action.cpp
        Action.hpp
        Actions.cpp
        Actions.hpp
        ActionTriggerContext.cpp
        ActionTriggerContext.hpp
        Application.cpp
        Application.hpp
        ButtonAction.cpp
        ButtonAction.hpp
        Display.cpp
        Display.hpp
        DimensionPolicy.hpp
        FocusChange.hpp
        HelpData.cpp
        HelpData.hpp
        impl/ScrollMetrics.cpp
        impl/ScrollMetrics.hpp
        LayoutDimension.cpp
        LayoutDimension.hpp
        LayoutContext.cpp
        LayoutContext.hpp
        Layout.cpp
        Layout.hpp
        LayoutData.hpp
        LayoutMetrics.cpp
        LayoutMetrics.hpp
        LayoutMetricsEditor.cpp
        LayoutMetricsEditor.hpp
        LayoutProposal.hpp
        LayoutScope.cpp
        LayoutScope.hpp
        MeasureScope.cpp
        MeasureScope.hpp
        Page.cpp
        Page.hpp
        PaintContext.hpp
        ScrollBarMode.hpp
        SizePolicy.hpp
        Surface.cpp
        SurfaceContainer.cpp
        SurfaceContainer.hpp
        SurfaceFlags.cpp
        SurfaceFlags.hpp
        Surface.hpp
        SurfaceManager.cpp
        SurfaceManager.hpp
        SurfacePtr.hpp
        SurfaceThemeAttributes.cpp
        SurfaceThemeAttributes.hpp
        ThemeContext.cpp
        ThemeContext.hpp
        ui_namespace.hpp
)
