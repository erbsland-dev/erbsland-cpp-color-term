// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Style.hpp"
#include "TextNode.hpp"

#include "../CursorWriter.hpp"

#include <string>
#include <string_view>
#include <utility>

namespace erbsland::cterm::text {

/// A renderer for HTML content to strings or cursor writer targets.
class HtmlRenderer {
public:
    /// Create a new HTML renderer with the given HTML content and style.
    explicit HtmlRenderer(const std::string_view html, StyleConstPtr style = Style::defaultStyle()) :
        _html{html}, _style{std::move(style)} {}

    // defaults
    ~HtmlRenderer() = default;
    HtmlRenderer(const HtmlRenderer &) = delete;
    HtmlRenderer(HtmlRenderer &&) = delete;
    auto operator=(const HtmlRenderer &) -> HtmlRenderer & = delete;
    auto operator=(HtmlRenderer &&) -> HtmlRenderer & = delete;

public:
    /// Render a given HTML fragment into a string.
    /// - Paragraphs are rendered as lines, with one line break at the end of the paragraph.
    /// - Leading/trailing linebreaks, carriage returns, and tabs are trimmed at the beginning and end of the result.
    /// - Block and document margins from the text style are ignored.
    /// - List-item indentation follows the configured `BulletListItem1` … `NumberedListItem8` styles, but margins
    ///   stay ignored.
    /// @note The idea behind this method is not to render structured documents but small text fragments.
    ///   Therefore, the output is not suitable for large documents or complex formatting.
    /// @return A string with the formatted text.
    [[nodiscard]] auto renderString() const -> String;

    /// Render a given HTML fragment/document to a `CursorWriter`.
    /// CSS-like block and document margins are applied when rendering to a cursor writer target.
    /// @param cursorWriterPtr The cursor writer to render the HTML to.
    void renderTo(const CursorWriterPtr &cursorWriterPtr) const;

public:
    /// Parse an HTML document into a text tree.
    /// @param html The HTML code to parse.
    /// @return A text node representing the parsed HTML.
    [[nodiscard]] static auto parse(std::string_view html) -> TextNodePtr;
    /// Escape text for safe use inside an HTML fragment.
    /// @param text The plain text to escape.
    /// @return The escaped HTML text.
    [[nodiscard]] static auto escapeHtml(std::string_view text) -> std::string;

private:
    std::string_view _html;
    StyleConstPtr _style;
};

}
