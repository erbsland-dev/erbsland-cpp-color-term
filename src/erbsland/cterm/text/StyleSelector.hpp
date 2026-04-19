// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "StyleListKind.hpp"
#include "StyleRole.hpp"

#include <initializer_list>
#include <optional>
#include <string>
#include <vector>

namespace erbsland::cterm::text {

/// A selector used to define or request one style rule.
class StyleSelector final {
public:
    /// Create a paragraph selector.
    constexpr StyleSelector() noexcept = default;

    /// Create a selector for the given role.
    /// @param styleRole The semantic role to match.
    constexpr explicit StyleSelector(const StyleRole styleRole) noexcept : _styleRole{styleRole} {}

    /// Create a selector with optional level and list-kind qualifiers.
    /// @param styleRole The semantic role to match.
    /// @param level The optional heading or list nesting level.
    /// @param listKind The optional list kind qualifier.
    constexpr StyleSelector(
        const StyleRole styleRole,
        const std::optional<int> level,
        const StyleListKind listKind = StyleListKind::Any) noexcept :
        _styleRole{styleRole}, _level{level}, _listKind{listKind} {}

    /// Create a selector with required style tokens.
    /// @param styleRole The semantic role to match.
    /// @param requiredStyleTokens Required style tokens parsed from `TextNode::style()`.
    StyleSelector(const StyleRole styleRole, const std::initializer_list<std::string> requiredStyleTokens) :
        _styleRole{styleRole}, _requiredStyleTokens{requiredStyleTokens} {}

    /// Create a selector with optional level, list kind, and required style tokens.
    /// @param styleRole The semantic role to match.
    /// @param level The optional heading or list nesting level.
    /// @param listKind The optional list kind qualifier.
    /// @param requiredStyleTokens Required style tokens parsed from `TextNode::style()`.
    StyleSelector(
        const StyleRole styleRole,
        const std::optional<int> level,
        const StyleListKind listKind,
        const std::initializer_list<std::string> requiredStyleTokens) :
        _styleRole{styleRole}, _level{level}, _listKind{listKind}, _requiredStyleTokens{requiredStyleTokens} {}

    // defaults
    StyleSelector(const StyleSelector &) = default;
    StyleSelector(StyleSelector &&) = default;
    ~StyleSelector() = default;
    auto operator=(const StyleSelector &) -> StyleSelector & = default;
    auto operator=(StyleSelector &&) -> StyleSelector & = default;

public: // operators
    /// Compare two selectors for exact identity.
    [[nodiscard]] constexpr auto operator==(const StyleSelector &other) const noexcept -> bool = default;

public: // accessors
    /// Get the semantic role to match.
    [[nodiscard]] constexpr auto styleRole() const noexcept -> StyleRole { return _styleRole; }
    /// Get the optional heading or list nesting level.
    [[nodiscard]] constexpr auto level() const noexcept -> const std::optional<int> & { return _level; }
    /// Get the optional list kind qualifier.
    [[nodiscard]] constexpr auto listKind() const noexcept -> StyleListKind { return _listKind; }
    /// Get the required style tokens parsed from `TextNode::style()`.
    [[nodiscard]] auto requiredStyleTokens() noexcept -> std::vector<std::string> & { return _requiredStyleTokens; }
    /// Get the required style tokens parsed from `TextNode::style()`.
    [[nodiscard]] auto requiredStyleTokens() const noexcept -> const std::vector<std::string> & {
        return _requiredStyleTokens;
    }

public: // factory methods
    /// Create a selector for the given semantic role.
    /// @param role The role to select.
    /// @return The selector for that role.
    [[nodiscard]] static constexpr auto role(const StyleRole role) noexcept -> StyleSelector {
        return StyleSelector{role};
    }
    /// Create a document selector.
    /// @return The document selector.
    [[nodiscard]] static constexpr auto document() noexcept -> StyleSelector { return role(StyleRole::Document); }
    /// Create a section selector.
    /// @return The section selector.
    [[nodiscard]] static constexpr auto section() noexcept -> StyleSelector { return role(StyleRole::Section); }
    /// Create a paragraph selector.
    /// @return The selector for regular paragraphs.
    [[nodiscard]] static constexpr auto paragraph() noexcept -> StyleSelector { return role(StyleRole::Paragraph); }
    /// Create a heading selector for the given level.
    /// @param level The one-based heading level.
    /// @return The selector for that heading level.
    [[nodiscard]] static constexpr auto heading(const int level) noexcept -> StyleSelector {
        return StyleSelector{StyleRole::Heading, level};
    }
    /// Create a list-container selector.
    /// @param listKind The list kind.
    /// @param level The zero-based list nesting level.
    /// @return The selector for that list container.
    [[nodiscard]] static constexpr auto listContainer(const StyleListKind listKind, const int level) noexcept
        -> StyleSelector {
        return StyleSelector{StyleRole::ListContainer, level, listKind};
    }
    /// Create a list-item selector.
    /// @param listKind The list kind.
    /// @param level The zero-based list nesting level.
    /// @return The selector for that list item.
    [[nodiscard]] static constexpr auto listItem(const StyleListKind listKind, const int level) noexcept
        -> StyleSelector {
        return StyleSelector{StyleRole::ListItem, level, listKind};
    }
    /// Create a definition-list selector.
    /// @return The definition-list selector.
    [[nodiscard]] static constexpr auto definitionList() noexcept -> StyleSelector {
        return role(StyleRole::DefinitionList);
    }
    /// Create a definition-term selector.
    /// @return The definition-term selector.
    [[nodiscard]] static constexpr auto definitionTerm() noexcept -> StyleSelector {
        return role(StyleRole::DefinitionTerm);
    }
    /// Create a definition-description selector.
    /// @return The definition-description selector.
    [[nodiscard]] static constexpr auto definitionDescription() noexcept -> StyleSelector {
        return role(StyleRole::DefinitionDescription);
    }
    /// Create a blockquote selector.
    /// @return The blockquote selector.
    [[nodiscard]] static constexpr auto blockquote() noexcept -> StyleSelector { return role(StyleRole::Blockquote); }
    /// Create a code-block selector.
    /// @return The code-block selector.
    [[nodiscard]] static constexpr auto codeBlock() noexcept -> StyleSelector { return role(StyleRole::CodeBlock); }
    /// Create a horizontal-rule selector.
    /// @return The horizontal-rule selector.
    [[nodiscard]] static constexpr auto horizontalRule() noexcept -> StyleSelector {
        return role(StyleRole::HorizontalRule);
    }
    /// Create an emphasis selector.
    /// @return The emphasis selector.
    [[nodiscard]] static constexpr auto emphasis() noexcept -> StyleSelector { return role(StyleRole::Emphasis); }
    /// Create a strong-text selector.
    /// @return The strong-text selector.
    [[nodiscard]] static constexpr auto strong() noexcept -> StyleSelector { return role(StyleRole::Strong); }
    /// Create an underline selector.
    /// @return The underline selector.
    [[nodiscard]] static constexpr auto underline() noexcept -> StyleSelector { return role(StyleRole::Underline); }
    /// Create a span selector.
    /// @return The generic inline span selector.
    [[nodiscard]] static constexpr auto span() noexcept -> StyleSelector { return role(StyleRole::Span); }
    /// Create a link selector.
    /// @return The link selector.
    [[nodiscard]] static constexpr auto link() noexcept -> StyleSelector { return role(StyleRole::Link); }
    /// Create an inline-code selector.
    /// @return The inline-code selector.
    [[nodiscard]] static constexpr auto code() noexcept -> StyleSelector { return role(StyleRole::Code); }

private:
    StyleRole _styleRole{StyleRole::Paragraph};    ///< The semantic role to match.
    std::optional<int> _level;                     ///< The optional heading or list nesting level.
    StyleListKind _listKind{StyleListKind::Any};   ///< The optional list kind qualifier.
    std::vector<std::string> _requiredStyleTokens; ///< Required style tokens from `TextNode::style()`.
};

}
