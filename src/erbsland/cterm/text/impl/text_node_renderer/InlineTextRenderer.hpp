// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "StyleResolver.hpp"
#include "WhitespaceMode.hpp"

#include "../../../impl/StringBuilder.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

class StringBuilderWithWhitespaceState;

/// Renders one inline node subtree into styled text.
class InlineTextRenderer final {
public:
    using StringBuilder = cterm::impl::StringBuilder;

public:
    /// Create the inline text renderer.
    /// @param styleResolver The resolver used for inline style rules.
    explicit InlineTextRenderer(StyleResolver &styleResolver) noexcept : _styleResolver{styleResolver} {}

    // defaults
    ~InlineTextRenderer() = default;
    InlineTextRenderer(const InlineTextRenderer &) = delete;
    InlineTextRenderer(InlineTextRenderer &&) = delete;
    auto operator=(const InlineTextRenderer &) -> InlineTextRenderer & = delete;
    auto operator=(InlineTextRenderer &&) -> InlineTextRenderer & = delete;

public:
    /// Render one node subtree into styled text.
    /// @param node The node subtree to render.
    /// @param style The active inline text style.
    /// @param whitespaceMode The whitespace handling strategy for this render pass.
    /// @return The rendered text.
    [[nodiscard]] auto
    render(const TextNode &node, const CharStyle &style, WhitespaceMode whitespaceMode = WhitespaceMode::Preserve)
        -> String;
    /// Append one node subtree to an existing output buffer.
    /// @param node The node subtree to render.
    /// @param style The active inline text style.
    /// @param text The destination text buffer.
    /// @param whitespaceMode The whitespace handling strategy for this render pass.
    void appendTo(
        const TextNode &node,
        const CharStyle &style,
        StringBuilderWithWhitespaceState &text,
        WhitespaceMode whitespaceMode);

private:
    void appendChildren(
        const TextNode &node,
        const CharStyle &style,
        StringBuilderWithWhitespaceState &text,
        WhitespaceMode whitespaceMode);

private:
    StyleResolver &_styleResolver; ///< The resolver used for inline style rules.
};

}
