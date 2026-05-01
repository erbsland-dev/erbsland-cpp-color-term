# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

add_subdirectory(event)
add_subdirectory(impl)
add_subdirectory(layout)
add_subdirectory(page)
add_subdirectory(surface)

target_sources(erbsland-color-term PRIVATE
        AbstractSurfaceContainer.hpp
        Action.cpp
        Action.hpp
        ActionTriggerContext.cpp
        ActionTriggerContext.hpp
        Actions.cpp
        Actions.hpp
        Application.cpp
        Application.hpp
        ButtonAction.cpp
        ButtonAction.hpp
        DimensionPolicy.hpp
        Display.cpp
        Display.hpp
        ExclusiveSurfaceManager.cpp
        ExclusiveSurfaceManager.hpp
        FocusChange.hpp
        HelpData.cpp
        HelpData.hpp
        HelpFormat.hpp
        HelpVisibility.hpp
        Layout.cpp
        Layout.hpp
        LayoutContext.cpp
        LayoutContext.hpp
        LayoutData.hpp
        LayoutDimension.cpp
        LayoutDimension.hpp
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
        Surface.hpp
        SurfaceContainer.cpp
        SurfaceContainer.hpp
        SurfaceFlags.cpp
        SurfaceFlags.hpp
        SurfaceManager.cpp
        SurfaceManager.hpp
        SurfacePtr.hpp
        SurfaceThemeAttributes.cpp
        SurfaceThemeAttributes.hpp
        ThemeContext.cpp
        ThemeContext.hpp
        ui_namespace.hpp
)
