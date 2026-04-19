cmake_minimum_required(VERSION 3.28)

file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../html-viewer/data/demo-tea.html" _ui_html_viewer_default_file)

erbsland_color_term_add_demo(ui-html-viewer
        src/HtmlDocumentPanel.cpp
        src/HtmlDocumentPanel.hpp
        src/main.cpp
        src/UiHtmlViewerApp.cpp
        src/UiHtmlViewerApp.hpp
)

target_compile_definitions(ui-html-viewer
        PRIVATE
        ERBSLAND_COLOR_TERM_UI_HTML_VIEWER_DEFAULT_FILE="${_ui_html_viewer_default_file}"
)
