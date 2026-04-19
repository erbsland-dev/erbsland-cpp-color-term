// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../CharStyle.hpp"
#include "../String.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace erbsland::cterm::text {

/// A visible marker used for bullet and numbered list items.
class StyleMarker final {
public:
    /// Marker rendering mode.
    enum class Kind : uint8_t {
        None,    ///< No visible marker.
        Literal, ///< Use `literal` as-is.
        Ordered, ///< Render the item number followed by `suffix`.
    };

    /// One rendered marker instance.
    struct Rendered final {
        /// The normalized plain-text representation.
        String text;
        /// The terminal representation preserving tab characters.
        String terminalText;
        /// The visible width in terminal cells.
        int width{0};
    };

public:
    /// Create an empty marker.
    StyleMarker() = default;

    // defaults
    ~StyleMarker() = default;
    StyleMarker(const StyleMarker &) = default;
    StyleMarker(StyleMarker &&) = default;
    auto operator=(const StyleMarker &) -> StyleMarker & = default;
    auto operator=(StyleMarker &&) -> StyleMarker & = default;

public:
    /// Get the marker rendering mode.
    [[nodiscard]] auto kind() const noexcept -> Kind { return _kind; }
    /// Get the overlay style applied to the rendered marker.
    [[nodiscard]] auto style() const noexcept -> const CharStyle & { return _style; }
    /// Get the literal marker text.
    [[nodiscard]] auto literal() const noexcept -> const String & { return _literal; }
    /// Get the ordered-list suffix text.
    [[nodiscard]] auto suffix() const noexcept -> const String & { return _suffix; }

    /// Clear the marker so no visible prefix is rendered.
    /// @return Reference to this marker.
    auto clear() noexcept -> StyleMarker &;
    /// Set the overlay style for the marker.
    /// @param style The new marker style.
    /// @return Reference to this marker.
    auto setStyle(const CharStyle &style) noexcept -> StyleMarker &;
    /// Set the overlay style for the marker.
    /// @param color The marker color.
    /// @param attributes The marker attributes.
    /// @return Reference to this marker.
    auto setStyle(Color color, CharAttributes attributes = {}) noexcept -> StyleMarker &;
    /// Use a literal marker string.
    /// @param literal The literal marker text.
    /// @param style Optional marker overlay style.
    /// @return Reference to this marker.
    auto setLiteral(String literal, const CharStyle &style = {}) -> StyleMarker &;
    /// Use a literal marker string.
    /// @param literal The literal marker text.
    /// @param style Optional marker overlay style.
    /// @return Reference to this marker.
    auto setLiteral(std::u32string_view literal, const CharStyle &style = {}) -> StyleMarker &;
    /// Use an ordered marker with the given suffix.
    /// @param suffix The text appended after the number.
    /// @param style Optional marker overlay style.
    /// @return Reference to this marker.
    auto setOrdered(String suffix = String{".\t"}, const CharStyle &style = {}) -> StyleMarker &;
    /// Use an ordered marker with the given suffix.
    /// @param suffix The text appended after the number.
    /// @param style Optional marker overlay style.
    /// @return Reference to this marker.
    auto setOrdered(std::u32string_view suffix, const CharStyle &style = {}) -> StyleMarker &;
    /// Render this marker for one list item number.
    /// Tabs advance to `targetColumn`, or become one space if that column is already passed.
    /// @param number The one-based item number for ordered lists.
    /// @param baseStyle The base style used for the marker text.
    /// @param targetColumn The wrapped content column.
    /// @return The rendered marker text plus visible width information.
    [[nodiscard]] auto render(std::size_t number, const CharStyle &baseStyle, int targetColumn) const -> Rendered;

private:
    Kind _kind{Kind::None}; ///< Marker rendering mode.
    CharStyle _style;       ///< Overlay style applied to rendered markers.
    String _literal;        ///< Marker text for literal markers.
    String _suffix;         ///< Suffix appended after ordered numbers.
};

}
