// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "StringData.hpp"

#include "../String.hpp"
#include "../StringView.hpp"

#include <utility>

namespace erbsland::cterm::impl {

/// Unique mutable builder optimized for repeated terminal-string assembly.
class StringBuilder {
public:
    /// Create an empty string builder.
    StringBuilder() = default;

    // defaults
    ~StringBuilder() = default;
    StringBuilder(const StringBuilder &) = delete;
    StringBuilder(StringBuilder &&) noexcept = default;
    auto operator=(const StringBuilder &) -> StringBuilder & = delete;
    auto operator=(StringBuilder &&) noexcept -> StringBuilder & = default;

public:
    /// Reserve storage for at least the given number of characters.
    /// @param size The requested capacity.
    void reserve(const std::size_t size) { _data.reserve(size); }
    /// Remove all characters while keeping the allocated capacity for reuse.
    void clear() noexcept { _data.clear(); }
    /// Test if the builder currently holds no characters.
    /// @return `true` if the builder is empty.
    [[nodiscard]] auto empty() const noexcept -> bool { return _data.size() == 0; }
    /// Get the number of stored characters.
    /// @return The current character count.
    [[nodiscard]] auto size() const noexcept -> std::size_t { return _data.size(); }
    /// Get the cached display width of the built text.
    /// @return The display width in terminal cells.
    [[nodiscard]] auto displayWidth() const noexcept -> int { return _data.displayWidth(); }
    /// Append one terminal character as-is.
    /// @param character The character to append.
    void append(const Char &character) noexcept { _data.append(character); }
    /// Append one styled terminal-string view as-is.
    /// @param view The string view to append.
    void append(const StringView &view) noexcept {
        if (view.empty()) {
            return;
        }
        auto &chars = _data.chars();
        chars.insert(chars.end(), view.begin(), view.end());
        _data.addDisplayWidth(view.displayWidth());
    }
    /// Append one terminal-string view with a resolved base style.
    /// @param view The string view to append.
    /// @param style The base style used for inherited color components or attributes.
    void appendWithBaseStyle(const StringView &view, const CharStyle style) noexcept {
        if (view.empty()) {
            return;
        }
        for (const auto &character : view) {
            const auto resolved = character.withBase(style);
            _data.append(resolved);
        }
    }
    /// Append UTF-32 text using one uniform style.
    /// @param text The UTF-32 text to append.
    /// @param style The style applied to the appended characters.
    void appendStyled(const std::u32string_view text, const CharStyle style) noexcept {
        _data.appendCharacters(text, style.color(), style.attributes());
    }
    /// Append UTF-8 text using one uniform style.
    /// @param text The UTF-8 text to append.
    /// @param style The style applied to the appended characters.
    /// @param encodingErrors How malformed UTF-8 is handled.
    void appendStyled(
        const std::string_view text,
        const CharStyle style,
        const EncodingErrors encodingErrors = EncodingErrors::Replace) noexcept {
        _data.appendCharacters(text, style.color(), style.attributes(), encodingErrors);
    }
    /// Materialize the current builder contents into an owned string copy.
    /// @return A copied terminal string.
    [[nodiscard]] auto toString() const -> String {
        return String::fromStorageWithDisplayWidth(_data.chars(), _data.displayWidth());
    }
    /// Move the built string out and reset the builder for fast reuse.
    /// @return The built terminal string.
    [[nodiscard]] auto takeString() -> String {
        const auto displayWidth = _data.displayWidth();
        auto chars = std::move(_data.chars());
        _data = StringData{};
        return String::fromStorageWithDisplayWidth(std::move(chars), displayWidth);
    }

protected:
    StringData _data; ///< Unique mutable storage for the built characters.
};

}
