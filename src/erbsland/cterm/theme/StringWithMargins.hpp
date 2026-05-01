// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../geometry/Margins.hpp"
#include "../String.hpp"

namespace erbsland::cterm::theme {

/// A string with surrounding margins.
/// Padding is part of the string.
/// Only horizontal margins are relevant.
class StringWithMargins final {
public:
    StringWithMargins() = default;
    StringWithMargins(String text, const Margins margins) noexcept :
        _text(std::move(text)), _margins(margins.expandedPositive()) {}

    // defaults
    StringWithMargins(StringWithMargins const &) = default;
    StringWithMargins(StringWithMargins &&) noexcept = default;
    auto operator=(StringWithMargins const &) -> StringWithMargins & = default;
    auto operator=(StringWithMargins &&) noexcept -> StringWithMargins & = default;
    ~StringWithMargins() = default;

public:
    /// Get the text.
    [[nodiscard]] auto text() const noexcept -> String const & { return _text; }
    /// Get the margins.
    /// Margins are always zero or positive.
    [[nodiscard]] auto margins() const noexcept -> Margins const & { return _margins; }
    /// Get the text display width, without margins.
    /// @return The display width of the text, without outer margins.
    [[nodiscard]] auto displayWidth() const noexcept -> Coordinate { return _text.displayWidth(); }
    /// Join this tex with another one.
    /// The inner margins get collapsed and filled with transparent spaces.
    /// The first and last margins are returned in the result.
    /// This can be sightly more efficient than `joined()`.
    /// @param other The other string with margins to join with.
    void join(const StringWithMargins &other) noexcept;
    /// Get the display width if another text is joined with this one.
    /// @param other The other string with margins to join with.
    /// @return The display width of the joined strings (without outer margins).
    [[nodiscard]] auto joinedDisplayWidth(const StringWithMargins &other) const noexcept -> Coordinate {
        return _text.displayWidth() + other._text.displayWidth() + std::max(_margins.right(), other._margins.left());
    }
    /// Return this text, joined with another one.
    /// The inner margins get collapsed and filled with transparent spaces.
    /// The first and last margins are returned in the result.
    /// @param other The other string with margins to join with.
    /// @return The joined string with margins.
    [[nodiscard]] auto joined(const StringWithMargins &other) const noexcept -> StringWithMargins;

private:
    String _text;     ///< The text with padding.
    Margins _margins; ///< The margins around this text.
};

}
