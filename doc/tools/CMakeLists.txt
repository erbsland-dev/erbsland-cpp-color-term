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
