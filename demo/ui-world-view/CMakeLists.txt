# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data/world-map.txt" _ui_world_view_default_data_file)

erbsland_color_term_add_demo(ui-world-view
        src/main.cpp
        src/UiWorldViewApp.cpp
        src/UiWorldViewApp.hpp
        src/WorldMapData.cpp
        src/WorldMapData.hpp
        src/WorldMapView.cpp
        src/WorldMapView.hpp
)

target_compile_definitions(ui-world-view
        PRIVATE
        ERBSLAND_COLOR_TERM_UI_WORLD_VIEW_DEFAULT_DATA_FILE="${_ui_world_view_default_data_file}"
)
