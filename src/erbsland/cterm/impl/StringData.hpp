// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Char.hpp"
#include "../EncodingErrors.hpp"
#include "../IndexRange.hpp"

#include <memory>
#include <string_view>
#include <vector>

namespace erbsland::cterm::impl {

/// Shared immutable backing storage for `String` and `StringView`.
class StringData final {
public:
    using Storage = std::vector<Char>;
    constexpr static auto cNoCachedValue = -1;

public:
    /// Create one empty storage object.
    StringData() noexcept = default;
    /// Create one shared storage object from a character buffer.
    /// @param chars The characters to store.
    explicit StringData(Storage chars) noexcept;
    /// Create one shared storage object with an already known display width.
    /// @param chars The characters to store.
    /// @param displayWidth The cached display width for the full storage buffer.
    StringData(Storage chars, int displayWidth) noexcept;

    // defaults
    ~StringData() = default;
    StringData(const StringData &) = default;
    StringData(StringData &&) = default;
    auto operator=(const StringData &) -> StringData & = default;
    auto operator=(StringData &&) -> StringData & = default;

public:
    /// Access the stored characters.
    [[nodiscard]] auto chars() const noexcept -> const Storage & { return _chars; }
    /// Access the stored characters for a unique mutable owner.
    [[nodiscard]] auto chars() noexcept -> Storage & { return _chars; }
    /// Get the number of stored characters.
    [[nodiscard]] auto size() const noexcept -> std::size_t { return _chars.size(); }
    /// Get the cached display width for the full stored buffer.
    [[nodiscard]] auto displayWidth() const noexcept -> int { return _displayWidth; }
    /// Test if the cached display width is valid.
    [[nodiscard]] auto hasDisplayWidthCache() const noexcept -> bool { return _displayWidth != cNoCachedValue; }
    /// Reserve storage for at least the given number of characters.
    /// @param size The requested capacity.
    void reserve(std::size_t size);
    /// Clear the stored characters and reset the cached display width.
    void clear() noexcept;
    /// Append one terminal character and update the cached display width.
    /// @param character The character to append.
    void append(const Char &character) noexcept;
    /// Add one known display width contribution to the cached full-buffer width.
    /// @param displayWidth The width contribution to add.
    void addDisplayWidth(int displayWidth) noexcept;
    /// Mark the cached display width as invalid after mutable element access.
    void invalidateDisplayWidth() noexcept { _displayWidth = cNoCachedValue; }
    /// Materialize one character range into a new contiguous buffer.
    /// @param range The range to copy from the stored data.
    /// @return The copied characters.
    [[nodiscard]] auto copyChars(IndexRange range) const -> Storage;
    /// Append one decoded code point to the storage.
    /// @param codePoint The decoded code point.
    /// @param color The base color for newly appended characters.
    /// @param attributes The base attributes for newly appended characters.
    void appendCodePoint(char32_t codePoint, Color color, CharAttributes attributes);
    /// Append UTF-32 text to the storage.
    /// @param text The UTF-32 source text.
    /// @param color The base color for newly appended characters.
    /// @param attributes The base attributes for newly appended characters.
    void appendCharacters(std::u32string_view text, Color color, CharAttributes attributes) noexcept;
    /// Append UTF-8 text to the storage.
    /// @param text The UTF-8 source text.
    /// @param color The base color for newly appended characters.
    /// @param attributes The base attributes for newly appended characters.
    /// @param encodingErrors How malformed UTF-8 is handled.
    void appendCharacters(
        std::string_view text, Color color, CharAttributes attributes, EncodingErrors encodingErrors) noexcept;

public:
    /// Create a shared immutable empty backing store.
    [[nodiscard]] static auto sharedEmpty() -> const std::shared_ptr<StringData> &;
    /// Measure the terminal display width produced by UTF-8 text.
    /// @param text The UTF-8 source text.
    /// @param encodingErrors How malformed UTF-8 is handled.
    /// @return The display width of the accepted terminal characters.
    [[nodiscard]] static auto
    measureDisplayWidth(std::string_view text, EncodingErrors encodingErrors = EncodingErrors::Replace) -> int;
    /// Measure the terminal display width produced by UTF-32 text.
    /// @param text The UTF-32 source text.
    /// @return The display width of the accepted terminal characters.
    [[nodiscard]] static auto measureDisplayWidth(std::u32string_view text) -> int;

private:
    Storage _chars;      ///< The stored characters.
    int _displayWidth{}; ///< Cached display width for the full stored buffer.
};

}
