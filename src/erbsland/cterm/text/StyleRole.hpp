// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <cstdint>

namespace erbsland::cterm::text {

/// Semantic roles that can be styled.
enum class StyleRole : uint8_t {
    Document,              ///< The root document container.
    Section,               ///< Generic block container such as `section` or `div`.
    Paragraph,             ///< Regular paragraph blocks.
    Heading,               ///< Heading blocks.
    ListContainer,         ///< Bullet or numbered list containers.
    ListItem,              ///< List item content boxes.
    DefinitionList,        ///< Definition list containers.
    DefinitionTerm,        ///< Definition terms.
    DefinitionDescription, ///< Definition descriptions.
    Blockquote,            ///< Blockquote containers.
    CodeBlock,             ///< Code blocks.
    HorizontalRule,        ///< Horizontal separators.
    Emphasis,              ///< Emphasized or italic inline content.
    Strong,                ///< Strong or bold inline content.
    Underline,             ///< Underlined inline content.
    Span,                  ///< Generic inline span containers.
    Link,                  ///< Links or active references.
    Code,                  ///< Inline code.
};

}
