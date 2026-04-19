// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Style.hpp"
#include "../TextNode.hpp"

#include "../../CursorWriter.hpp"

namespace erbsland::cterm::text::impl {

/// Render a `TextNode` tree to plain text or a cursor writer target.
class TextNodeRenderer final {
public:
    /// Create a renderer for a parsed text tree.
    /// @param document The document node to render. `nullptr` produces empty output.
    /// @param style The style to apply. `nullptr` uses `Style::defaultStyle()`.
    explicit TextNodeRenderer(TextNodeConstPtr document, StyleConstPtr style = Style::defaultStyle());

    // defaults
    ~TextNodeRenderer() = default;
    TextNodeRenderer(const TextNodeRenderer &) = delete;
    TextNodeRenderer(TextNodeRenderer &&) = delete;
    auto operator=(const TextNodeRenderer &) -> TextNodeRenderer & = delete;
    auto operator=(TextNodeRenderer &&) -> TextNodeRenderer & = delete;

public:
    /// Render the document to a terminal string.
    [[nodiscard]] auto renderString() const -> String;
    /// Render the document to a cursor writer target.
    /// CSS-like vertical margin collapsing is applied only for this full layout rendering path.
    /// @param cursorWriterPtr The destination writer. `nullptr` is ignored.
    void renderTo(const CursorWriterPtr &cursorWriterPtr) const;

private:
    TextNodeConstPtr _document;
    StyleConstPtr _style;
};

}
