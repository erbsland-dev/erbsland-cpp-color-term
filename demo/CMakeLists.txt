cmake_minimum_required(VERSION 3.28)

add_subdirectory(common)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(erbsland_color_term_add_demo target_name)
    add_executable(${target_name} ${ARGN})
    target_compile_features(${target_name} PRIVATE cxx_std_20)
    target_link_libraries(${target_name}
            PRIVATE
            erbsland-color-term-demo-common
    )
    erbsland_set_required_compiler_options(${target_name})
    erbsland_enable_debug_warnings(${target_name})
    set_target_properties(${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/demo-apps"
            FOLDER "Demos"
    )
endfunction()

add_subdirectory(bitmap-showcase)
add_subdirectory(command-line-help)
add_subdirectory(display-all-attributes)
add_subdirectory(display-all-colors)
add_subdirectory(frame-color-animations)
add_subdirectory(frame-weaver)
add_subdirectory(html-viewer)
add_subdirectory(key-input-demo)
add_subdirectory(log-viewer)
add_subdirectory(minimum-effort)
add_subdirectory(retro-plasma)
add_subdirectory(terminal-chronicle)
add_subdirectory(text-gallery)
add_subdirectory(ui-hello-world)
add_subdirectory(ui-html-viewer)
add_subdirectory(update-screen-modes)
