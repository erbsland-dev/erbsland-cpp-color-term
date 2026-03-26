cmake_minimum_required(VERSION 3.28)

add_executable(paragraph-options-reference
        paragraph-options-reference.cpp
)
target_compile_features(paragraph-options-reference PRIVATE cxx_std_20)
target_link_libraries(paragraph-options-reference PRIVATE erbsland-color-term::core)
erbsland_set_required_compiler_options(paragraph-options-reference)
erbsland_enable_debug_warnings(paragraph-options-reference)
set_target_properties(paragraph-options-reference PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/doc-tools"
)

add_executable(drawing-reference
        drawing-reference.cpp
)
target_compile_features(drawing-reference PRIVATE cxx_std_20)
target_link_libraries(drawing-reference PRIVATE erbsland-color-term::core)
erbsland_set_required_compiler_options(drawing-reference)
erbsland_enable_debug_warnings(drawing-reference)
set_target_properties(drawing-reference PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/doc-tools"
)

add_executable(drawing-text-reference
        drawing-text-reference.cpp
)
target_compile_features(drawing-text-reference PRIVATE cxx_std_20)
target_link_libraries(drawing-text-reference PRIVATE erbsland-color-term::core)
erbsland_set_required_compiler_options(drawing-text-reference)
erbsland_enable_debug_warnings(drawing-text-reference)
set_target_properties(drawing-text-reference PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/doc-tools"
)

add_executable(font-reference
        font-reference.cpp
)
target_compile_features(font-reference PRIVATE cxx_std_20)
target_link_libraries(font-reference PRIVATE erbsland-color-term::core)
erbsland_set_required_compiler_options(font-reference)
erbsland_enable_debug_warnings(font-reference)
set_target_properties(font-reference PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/doc-tools"
)

add_executable(geometry-reference
        geometry-reference.cpp
)
target_compile_features(geometry-reference PRIVATE cxx_std_20)
target_link_libraries(geometry-reference PRIVATE erbsland-color-term::core)
erbsland_set_required_compiler_options(geometry-reference)
erbsland_enable_debug_warnings(geometry-reference)
set_target_properties(geometry-reference PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/doc-tools"
)
