// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "StyleMatchContext.hpp"
#include "StyleRule.hpp"
#include "StyleSelector.hpp"

#include "../CharStyle.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace erbsland::cterm::text {

class Style;
/// Shared pointer to a mutable text style.
using StylePtr = std::shared_ptr<Style>;
/// Shared pointer to an immutable text style.
using StyleConstPtr = std::shared_ptr<const Style>;

/// A selector-driven style configuration for rich text rendering.
class Style final {
public:
    /// Shared predefined style variants.
    enum class Predefined : uint8_t {
        Plain,  ///< The plain built-in rich-text style.
        Simple, ///< A compact style inspired by the HTML viewer demo.
        Styled, ///< An extended decorative style inspired by the HTML viewer demo.
    };

    using Role = StyleRole;                 ///< Alias for the standalone style role type.
    using ListKind = StyleListKind;         ///< Alias for the standalone list-kind type.
    using MatchContext = StyleMatchContext; ///< Alias for the standalone match-context type.

public:
    /// Create the default rich-text style.
    Style();

    // defaults
    ~Style() = default;
    Style(const Style &) = default;
    Style(Style &&) = default;
    auto operator=(const Style &) -> Style & = default;
    auto operator=(Style &&) -> Style & = default;

public:
    /// Create a mutable copy of this style.
    /// @return A shared pointer to a cloned style instance.
    [[nodiscard]] auto clone() const -> StylePtr;

    /// Access the base text style used before selector-specific overlays.
    /// @return The configured base text style.
    [[nodiscard]] auto baseTextStyle() const noexcept -> const CharStyle &;
    /// Replace the base text style used before selector-specific overlays.
    /// @param style The new base text style.
    void setBaseTextStyle(const CharStyle &style) noexcept;

    /// Access the base block layout used before selector-specific overrides.
    /// @return The configured base block layout.
    [[nodiscard]] auto baseBlockLayout() const noexcept -> const ParagraphIndents &;
    /// Replace the base block layout used before selector-specific overrides.
    /// @param layout The new base block layout.
    void setBaseBlockLayout(const ParagraphIndents &layout) noexcept;

    /// Access one exact stored rule definition.
    /// @param selector The selector to query.
    /// @return The stored rule, if present.
    [[nodiscard]] auto definition(const StyleSelector &selector) const noexcept
        -> std::optional<std::reference_wrapper<const StyleRule>>;
    /// Create or access one exact stored rule definition.
    /// New definitions are initialized from the currently resolved rule for the same selector.
    /// @param selector The selector to edit.
    /// @return A mutable reference to the stored rule.
    auto edit(const StyleSelector &selector) -> StyleRule &;
    /// Remove one exact stored rule definition.
    /// @param selector The selector to erase.
    void erase(const StyleSelector &selector) noexcept;
    /// Resolve the best matching rule for one selector and match context.
    /// @param selector The requested selector. Explicit selector fields override the context.
    /// @param context The current node context.
    /// @return The resolved rule.
    [[nodiscard]] auto resolve(const StyleSelector &selector, const StyleMatchContext &context) const -> StyleRule;

    /// Access one shared predefined default style instance.
    /// @param predefined The predefined style variant to return.
    /// @return The lazily constructed shared style instance.
    [[nodiscard]] static auto defaultStyle(Predefined predefined = Predefined::Plain) noexcept -> const StyleConstPtr &;
    /// Access the shared plain default style instance.
    /// @return The lazily constructed shared plain style instance.
    [[nodiscard]] static auto defaultPlain() noexcept -> const StyleConstPtr &;
    /// Access the shared simple default style instance.
    /// @return The lazily constructed shared simple style instance.
    [[nodiscard]] static auto defaultSimple() noexcept -> const StyleConstPtr &;
    /// Access the shared styled default style instance.
    /// @return The lazily constructed shared styled style instance.
    [[nodiscard]] static auto defaultStyled() noexcept -> const StyleConstPtr &;

private:
    struct Entry final {
        StyleSelector selector;
        StyleRule rule;
    };

    [[nodiscard]] static auto createSimpleDefaultStyle() -> StyleConstPtr;
    [[nodiscard]] static auto createStyledDefaultStyle() -> StyleConstPtr;
    [[nodiscard]] static auto normalizedSelector(StyleSelector selector) -> StyleSelector;
    [[nodiscard]] auto defaultRuleFor(Role role, ListKind listKind, std::optional<int> level) const noexcept
        -> StyleRule;
    [[nodiscard]] static auto mergeSelectorIntoContext(const StyleSelector &selector, const StyleMatchContext &context)
        -> StyleMatchContext;
    [[nodiscard]] static auto
    combineSelectorAndContextTokens(const StyleSelector &selector, std::span<const std::string> contextTokens)
        -> std::vector<std::string>;
    [[nodiscard]] static auto
    tokensMatch(std::span<const std::string> requiredTokens, std::span<const std::string> contextTokens) noexcept
        -> bool;
    [[nodiscard]] auto findEntry(const StyleSelector &selector) noexcept -> std::vector<Entry>::iterator;
    [[nodiscard]] auto findEntry(const StyleSelector &selector) const noexcept -> std::vector<Entry>::const_iterator;

private:
    CharStyle _baseTextStyle;          ///< Base text style.
    ParagraphIndents _baseBlockLayout; ///< Base block layout.
    std::vector<Entry> _definitions;   ///< Explicit selector definitions.
};

}
