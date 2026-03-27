cmake_minimum_required(VERSION 3.28)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(erbsland_color_term_add_doc_reference target_name)
    add_executable(${target_name} "${target_name}.cpp")
    target_compile_features(${target_name} PRIVATE cxx_std_20)
    target_link_libraries(${target_name} PRIVATE erbsland-color-term::core)
    erbsland_set_required_compiler_options(${target_name})
    erbsland_enable_debug_warnings(${target_name})
    set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/doc-tools"
            FOLDER "DocTools"
    )
endfunction()

erbsland_color_term_add_doc_reference(paragraph-options-reference)
erbsland_color_term_add_doc_reference(drawing-reference)
erbsland_color_term_add_doc_reference(drawing-text-reference)
erbsland_color_term_add_doc_reference(font-reference)
erbsland_color_term_add_doc_reference(geometry-reference)
