// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "StyleListKind.hpp"
#include "StyleRole.hpp"

#include <optional>
#include <span>
#include <string>


namespace erbsland::cterm::text {


/// Additional node metadata used while resolving selectors.
class StyleMatchContext final {
public:
    /// Create one match context for selector resolution.
    /// @param role The semantic role of the current node.
    /// @param level The optional heading or list nesting level of the current node.
    /// @param listKind The list kind of the current node.
    /// @param styleTokens Parsed style tokens from `TextNode::style()`.
    StyleMatchContext(
        const StyleRole role = StyleRole::Paragraph,
        const std::optional<int> level = std::nullopt,
        const StyleListKind listKind = StyleListKind::Any,
        const std::span<const std::string> styleTokens = {}) noexcept :
        _role{role}, _level{level}, _listKind{listKind}, _styleTokens{styleTokens} {}

public:
    /// Access the semantic role of the current node.
    [[nodiscard]] auto role() const noexcept -> StyleRole { return _role; }
    /// Access the optional heading or list nesting level of the current node.
    [[nodiscard]] auto level() const noexcept -> const std::optional<int> & { return _level; }
    /// Access the list kind of the current node.
    [[nodiscard]] auto listKind() const noexcept -> StyleListKind { return _listKind; }
    /// Access the parsed style tokens from `TextNode::style()`.
    [[nodiscard]] auto styleTokens() const noexcept -> std::span<const std::string> { return _styleTokens; }

    /// Replace the semantic role of the current node.
    /// @param role The new semantic role.
    void setRole(const StyleRole role) noexcept { _role = role; }
    /// Replace the optional heading or list nesting level of the current node.
    /// @param level The new optional level.
    void setLevel(std::optional<int> level) noexcept { _level = level; }
    /// Replace the list kind of the current node.
    /// @param listKind The new list kind.
    void setListKind(const StyleListKind listKind) noexcept { _listKind = listKind; }

private:
    StyleRole _role{StyleRole::Paragraph};       ///< The semantic role of the current node.
    std::optional<int> _level;                   ///< The optional heading or list nesting level.
    StyleListKind _listKind{StyleListKind::Any}; ///< The list kind of the current node.
    std::span<const std::string> _styleTokens;   ///< Parsed style tokens from `TextNode::style()`.
};


}
