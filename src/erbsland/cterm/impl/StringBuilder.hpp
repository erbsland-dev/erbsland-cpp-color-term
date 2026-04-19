// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "StringData.hpp"

#include "../String.hpp"

namespace erbsland::cterm::impl {

/// Unique mutable builder optimized for repeated terminal-string assembly.
class StringBuilder final {
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
    void reserve(std::size_t size);
    /// Remove all characters while keeping the allocated capacity for reuse.
    void clear() noexcept;
    /// Test if the builder currently holds no characters.
    /// @return `true` if the builder is empty.
    [[nodiscard]] auto empty() const noexcept -> bool;
    /// Get the number of stored characters.
    /// @return The current character count.
    [[nodiscard]] auto size() const noexcept -> std::size_t;
    /// Get the cached display width of the built text.
    /// @return The display width in terminal cells.
    [[nodiscard]] auto displayWidth() const noexcept -> int;
    /// Append one terminal character as-is.
    /// @param character The character to append.
    void append(const Char &character) noexcept;
    /// Append one styled terminal-string view as-is.
    /// @param view The string view to append.
    void append(const StringView &view) noexcept;
    /// Append one terminal-string view with a resolved base style.
    /// @param view The string view to append.
    /// @param style The base style used for inherited color components or attributes.
    void appendWithBaseStyle(const StringView &view, CharStyle style) noexcept;
    /// Append UTF-32 text using one uniform style.
    /// @param text The UTF-32 text to append.
    /// @param style The style applied to the appended characters.
    void appendStyled(std::u32string_view text, CharStyle style) noexcept;
    /// Append UTF-8 text using one uniform style.
    /// @param text The UTF-8 text to append.
    /// @param style The style applied to the appended characters.
    /// @param encodingErrors How malformed UTF-8 is handled.
    void appendStyled(
        std::string_view text, CharStyle style, EncodingErrors encodingErrors = EncodingErrors::Replace) noexcept;
    /// Materialize the current builder contents into an owned string copy.
    /// @return A copied terminal string.
    [[nodiscard]] auto toString() const -> String;
    /// Move the built string out and reset the builder for fast reuse.
    /// @return The built terminal string.
    [[nodiscard]] auto takeString() -> String;

private:
    StringData _data; ///< Unique mutable storage for the built characters.
};

}
