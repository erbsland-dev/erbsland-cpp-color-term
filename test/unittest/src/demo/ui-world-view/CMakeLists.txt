# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

file(TO_CMAKE_PATH "${PROJECT_SOURCE_DIR}/demo/ui-world-view/data/world-map.txt" _ui_world_view_test_data_file)

target_compile_definitions(erbsland-color-term-unittest PRIVATE
        ERBSLAND_COLOR_TERM_UI_WORLD_VIEW_TEST_DATA_FILE="${_ui_world_view_test_data_file}"
)

target_sources(erbsland-color-term-unittest PRIVATE
        ${PROJECT_SOURCE_DIR}/demo/ui-world-view/src/WorldMapData.cpp
        ${PROJECT_SOURCE_DIR}/demo/ui-world-view/src/WorldMapView.cpp
        WorldMapDataTest.cpp
        WorldMapViewTest.cpp
)
