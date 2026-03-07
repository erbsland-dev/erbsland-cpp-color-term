cmake_minimum_required(VERSION 3.28)

add_library(erbsland-color-term-demo-common STATIC
        src/ScopedTerminalSession.cpp
        src/ScopedTerminalSession.hpp
)
add_library(erbsland-color-term::demo-common ALIAS erbsland-color-term-demo-common)
target_compile_features(erbsland-color-term-demo-common PUBLIC cxx_std_20)
target_link_libraries(erbsland-color-term-demo-common PUBLIC erbsland-color-term::core)
target_include_directories(erbsland-color-term-demo-common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
erbsland_set_required_compiler_options(erbsland-color-term-demo-common)
erbsland_enable_debug_warnings(erbsland-color-term-demo-common)
