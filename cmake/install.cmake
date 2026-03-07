# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

install(TARGETS ${_erbsland_color_term_install_targets}
        EXPORT erbsland-color-term-targets
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include
)
install(DIRECTORY include/
        DESTINATION include
)
install(DIRECTORY src/erbsland/cterm/
        DESTINATION src/erbsland/cterm
        FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.tpp"
)
install(EXPORT erbsland-color-term-targets
        FILE erbsland-color-term-targets.cmake
        NAMESPACE ErbslandDEV::
        DESTINATION lib/cmake/erbsland-color-term
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/erbsland-color-termConfigVersion.cmake"
        VERSION 1.0.0
        COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
        "${CMAKE_CURRENT_LIST_DIR}/erbsland-color-termConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/erbsland-color-termConfig.cmake"
        INSTALL_DESTINATION lib/cmake/erbsland-color-term
)
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/erbsland-color-termConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/erbsland-color-termConfigVersion.cmake"
        DESTINATION lib/cmake/erbsland-color-term
)
