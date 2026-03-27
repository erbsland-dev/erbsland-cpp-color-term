// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "StyleMarker.hpp"

#include "../CharStyle.hpp"
#include "../Coordinate.hpp"
#include "../ParagraphIndents.hpp"
#include "../String.hpp"

#include <optional>
#include <string_view>


namespace erbsland::cterm::text {


/// One configurable rich-text style rule with convenient fluent setters.
class StyleRule final {
public:
    /// Create an empty style rule.
    StyleRule() = default;

    // defaults
    ~StyleRule() = default;
    StyleRule(const StyleRule &) = default;
    StyleRule(StyleRule &&) = default;
    auto operator=(const StyleRule &) -> StyleRule & = default;
    auto operator=(StyleRule &&) -> StyleRule & = default;

public:
    /// Get the text overlay applied to inline or block content.
    [[nodiscard]] auto textStyle() const noexcept -> const CharStyle & { return _textStyle; }
    /// Replace the text overlay applied to inline or block content.
    /// @param style The new text style.
    /// @return Reference to this rule.
    auto setTextStyle(const CharStyle &style) noexcept -> StyleRule &;
    /// Replace the text overlay applied to inline or block content.
    /// @param color The new text color.
    /// @param attributes The new text attributes.
    /// @return Reference to this rule.
    auto setTextStyle(Color color, CharAttributes attributes = {}) noexcept -> StyleRule &;

    /// Get the full paragraph indentation settings.
    [[nodiscard]] auto indents() const noexcept -> const ParagraphIndents & { return _indents; }
    /// Replace the full paragraph indentation settings.
    /// @param indents The new indentation settings.
    /// @return Reference to this rule.
    auto setIndents(const ParagraphIndents &indents) noexcept -> StyleRule &;
    /// Get the margins around the block.
    [[nodiscard]] auto margins() const noexcept -> const Margins & { return _indents.margins(); }
    /// Replace the margins around the block.
    /// @param margins The new margins.
    /// @return Reference to this rule.
    auto setMargins(const Margins margins) noexcept -> StyleRule &;
    /// Replace the margins around the block.
    /// @param allSides Uniform margin for all sides.
    /// @return Reference to this rule.
    auto setMargins(int allSides) noexcept -> StyleRule &;
    /// Replace the margins around the block.
    /// @param horizontal Margin for left and right.
    /// @param vertical Margin for top and bottom.
    /// @return Reference to this rule.
    auto setMargins(int horizontal, int vertical) noexcept -> StyleRule &;
    /// Replace the margins around the block.
    /// @param top Top margin.
    /// @param right Right margin.
    /// @param bottom Bottom margin.
    /// @param left Left margin.
    /// @return Reference to this rule.
    auto setMargins(int top, int right, int bottom, int left) noexcept -> StyleRule &;
    /// Set the shared line indent.
    /// @param indent The new line indent.
    /// @return Reference to this rule.
    auto setLineIndent(int indent) noexcept -> StyleRule &;
    /// Set the first-line indent.
    /// @param indent The new first-line indent, or `ParagraphIndents::cUseLineIndent`.
    /// @return Reference to this rule.
    auto setFirstLineIndent(int indent) noexcept -> StyleRule &;
    /// Set the wrapped-line indent.
    /// @param indent The new wrapped-line indent, or `ParagraphIndents::cUseLineIndent`.
    /// @return Reference to this rule.
    auto setWrappedLineIndent(int indent) noexcept -> StyleRule &;

    /// Get the optional prefix inserted before the block content.
    [[nodiscard]] auto prefix() const noexcept -> const std::optional<String> & { return _prefix; }
    /// Set the optional prefix inserted before the block content.
    /// @param prefix The prefix string.
    /// @return Reference to this rule.
    auto setPrefix(String prefix) -> StyleRule &;
    /// Set the optional prefix inserted before the block content.
    /// @param prefix The prefix text.
    /// @param style Optional style for the prefix text.
    /// @return Reference to this rule.
    auto setPrefix(std::u32string_view prefix, const CharStyle &style = {}) -> StyleRule &;
    /// Remove any configured block prefix.
    /// @return Reference to this rule.
    auto clearPrefix() noexcept -> StyleRule &;

    /// Get the optional suffix appended after the block content.
    [[nodiscard]] auto suffix() const noexcept -> const std::optional<String> & { return _suffix; }
    /// Set the optional suffix appended after the block content.
    /// @param suffix The suffix string.
    /// @return Reference to this rule.
    auto setSuffix(String suffix) -> StyleRule &;
    /// Set the optional suffix appended after the block content.
    /// @param suffix The suffix text.
    /// @param style Optional style for the suffix text.
    /// @return Reference to this rule.
    auto setSuffix(std::u32string_view suffix, const CharStyle &style = {}) -> StyleRule &;
    /// Remove any configured block suffix.
    /// @return Reference to this rule.
    auto clearSuffix() noexcept -> StyleRule &;

    /// Get the optional fill character for filled headings or custom separators.
    [[nodiscard]] auto lineFill() const noexcept -> const std::optional<Char> & { return _lineFill; }
    /// Set the fill character for filled headings or custom separators.
    /// @param lineFill The fill character.
    /// @return Reference to this rule.
    auto setLineFill(const Char &lineFill) noexcept -> StyleRule &;
    /// Set the fill character for filled headings or custom separators.
    /// @param codePoint The fill character code point.
    /// @param style Optional style for the fill character.
    /// @return Reference to this rule.
    auto setLineFill(char32_t codePoint, const CharStyle &style = {}) noexcept -> StyleRule &;
    /// Set the fill character for filled headings or custom separators.
    /// @param codePoint The fill character code point.
    /// @param color The fill character color.
    /// @param attributes The fill character attributes.
    /// @return Reference to this rule.
    auto setLineFill(char32_t codePoint, Color color, CharAttributes attributes = {}) noexcept -> StyleRule &;
    /// Remove any configured fill character.
    /// @return Reference to this rule.
    auto clearLineFill() noexcept -> StyleRule &;

    /// Get the marker configuration.
    [[nodiscard]] auto marker() noexcept -> StyleMarker & { return _marker; }
    /// Get the marker configuration.
    [[nodiscard]] auto marker() const noexcept -> const StyleMarker & { return _marker; }
    /// Replace the full marker configuration.
    /// @param marker The new marker.
    /// @return Reference to this rule.
    auto setMarker(const StyleMarker &marker) -> StyleRule &;
    /// Configure a literal marker.
    /// @param literal The marker text.
    /// @param style Optional marker overlay style.
    /// @return Reference to this rule.
    auto setLiteralMarker(String literal, const CharStyle &style = {}) -> StyleRule &;
    /// Configure a literal marker.
    /// @param literal The marker text.
    /// @param style Optional marker overlay style.
    /// @return Reference to this rule.
    auto setLiteralMarker(std::u32string_view literal, const CharStyle &style = {}) -> StyleRule &;
    /// Configure an ordered marker.
    /// @param suffix The text appended after the item number.
    /// @param style Optional marker overlay style.
    /// @return Reference to this rule.
    auto setOrderedMarker(String suffix = String{U".\t"}, const CharStyle &style = {}) -> StyleRule &;
    /// Configure an ordered marker.
    /// @param suffix The text appended after the item number.
    /// @param style Optional marker overlay style.
    /// @return Reference to this rule.
    auto setOrderedMarker(std::u32string_view suffix, const CharStyle &style = {}) -> StyleRule &;
    /// Remove any configured list marker.
    /// @return Reference to this rule.
    auto clearMarker() noexcept -> StyleRule &;

private:
    CharStyle _textStyle;          ///< Text overlay applied to inline or block content.
    ParagraphIndents _indents;     ///< Block indentation and margins.
    std::optional<String> _prefix; ///< Optional prefix inserted before the block content.
    std::optional<String> _suffix; ///< Optional suffix appended after the block content.
    std::optional<Char> _lineFill; ///< Optional fill character for filled headings or separators.
    StyleMarker _marker;           ///< Optional list marker.
};


}
