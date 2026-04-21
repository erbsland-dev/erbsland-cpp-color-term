// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../TextNode.hpp"

#include <string>
#include <utility>

namespace erbsland::cterm::text::impl {

/// One frame on the HTML parser element stack.
class HtmlParserFrame final {
public:
    /// Create one parser frame.
    /// @param tagName The lower-case ASCII tag name.
    /// @param node The created node, if this frame owns one.
    /// @param transparent `true` if the tag itself has no matching node.
    /// @param suppressSubtree `true` if all content below this frame is ignored.
    /// @param preserveWhitespace `true` if text below this frame is parsed verbatim.
    HtmlParserFrame(
        std::u32string tagName = {},
        TextNodePtr node = {},
        const bool transparent = false,
        const bool suppressSubtree = false,
        const bool preserveWhitespace = false) :
        _tagName{std::move(tagName)},
        _node{std::move(node)},
        _transparent{transparent},
        _suppressSubtree{suppressSubtree},
        _preserveWhitespace{preserveWhitespace} {}

public:
    /// Access the lower-case ASCII tag name.
    [[nodiscard]] auto tagName() const noexcept -> const std::u32string & { return _tagName; }
    /// Access the created node, if this frame owns one.
    [[nodiscard]] auto node() const noexcept -> const TextNodePtr & { return _node; }
    /// Access the created node, if this frame owns one.
    [[nodiscard]] auto node() noexcept -> TextNodePtr & { return _node; }
    /// Test if the tag itself has no matching node.
    [[nodiscard]] auto transparent() const noexcept -> bool { return _transparent; }
    /// Test if all content below this frame is ignored.
    [[nodiscard]] auto suppressSubtree() const noexcept -> bool { return _suppressSubtree; }
    /// Test if text below this frame is parsed verbatim.
    [[nodiscard]] auto preserveWhitespace() const noexcept -> bool { return _preserveWhitespace; }

private:
    std::u32string _tagName;         ///< The lower-case ASCII tag name.
    TextNodePtr _node;               ///< The created node, if this frame owns one.
    bool _transparent{false};        ///< `true` if the tag itself has no matching node.
    bool _suppressSubtree{false};    ///< `true` if all content below this frame is ignored.
    bool _preserveWhitespace{false}; ///< `true` if text below this frame is parsed verbatim.
};

}
