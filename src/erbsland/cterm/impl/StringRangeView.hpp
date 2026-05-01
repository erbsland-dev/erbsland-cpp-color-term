// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "StringData.hpp"

#include "../geometry/Size.hpp"

#include <string_view>
#include <vector>

namespace erbsland::cterm::impl {

/// A non-owning read-only view over one range in `StringData`.
///
/// This type keeps shared range algorithms out of the public `String` and `StringView` API classes. All indexes used
/// by public methods are local to the visible range; returned `IndexRange` values are absolute storage ranges.
class StringRangeView final {
public:
    using Storage = StringData::Storage;                            ///< Storage container for the characters.
    using const_iterator = Storage::const_iterator;                 ///< Immutable forward iterator.
    using const_reverse_iterator = Storage::const_reverse_iterator; ///< Immutable reverse iterator.
    using difference_type = Storage::difference_type;               ///< Signed distance type.

public:
    /// Create a read-only view over a clamped storage range.
    /// @param data The backing storage that must outlive this view.
    /// @param range The absolute storage range to expose.
    explicit constexpr StringRangeView(const StringData &data, const IndexRange range) noexcept :
        _data{data}, _range{range} {}

public:
    /// Get the visible storage range.
    [[nodiscard]] auto range() const noexcept -> IndexRange { return _range; }
    /// Get the number of visible characters.
    [[nodiscard]] auto size() const noexcept -> std::size_t { return _range.length(); }
    /// Test if this range is empty.
    [[nodiscard]] auto empty() const noexcept -> bool { return _range.empty(); }
    /// Convert a local index into an absolute storage index.
    [[nodiscard]] auto storageIndex(std::size_t localIndex) const noexcept -> std::size_t;
    /// Access one character without bounds checking.
    [[nodiscard]] auto characterAt(std::size_t localIndex) const noexcept -> const Char &;
    /// Get a const iterator to the first character.
    [[nodiscard]] auto begin() const noexcept -> const_iterator;
    /// Get a const iterator past the last character.
    [[nodiscard]] auto end() const noexcept -> const_iterator;
    /// Get a const iterator to the first character.
    [[nodiscard]] auto cbegin() const noexcept -> const_iterator;
    /// Get a const iterator past the last character.
    [[nodiscard]] auto cend() const noexcept -> const_iterator;
    /// Get a const reverse iterator to the last character.
    [[nodiscard]] auto rbegin() const noexcept -> const_reverse_iterator;
    /// Get a const reverse iterator past the first character.
    [[nodiscard]] auto rend() const noexcept -> const_reverse_iterator;
    /// Get a const reverse iterator to the last character.
    [[nodiscard]] auto crbegin() const noexcept -> const_reverse_iterator;
    /// Get a const reverse iterator past the first character.
    [[nodiscard]] auto crend() const noexcept -> const_reverse_iterator;
    /// Get the width of the visible range in terminal cells.
    [[nodiscard]] auto displayWidth() const noexcept -> int;
    /// Access one character without bounds checking.
    [[nodiscard]] auto operator[](std::size_t index) const noexcept -> Char;
    /// Access one character with bounds checking.
    /// @param index The local character index.
    /// @param typeName The public type name to use in the exception message.
    /// @return A copy of the character at `index`.
    /// @throws std::out_of_range If `index` is outside the visible range.
    [[nodiscard]] auto at(std::size_t index, std::string_view typeName) const -> Char;
    /// Count fully styled character matches.
    [[nodiscard]] auto count(const Char &character) const noexcept -> std::size_t;
    /// Count one-code-point matches, ignoring style.
    [[nodiscard]] auto count(char32_t character) const noexcept -> std::size_t;
    /// Find the next fully styled character match.
    [[nodiscard]] auto indexOf(const Char &character, std::size_t startIndex = 0) const noexcept -> std::size_t;
    /// Find the next one-code-point match, ignoring style.
    [[nodiscard]] auto indexOf(char32_t character, std::size_t startIndex = 0) const noexcept -> std::size_t;
    /// Find the next one-code-point character that is part of the given set.
    [[nodiscard]] auto indexOf(std::u32string_view characterSet, std::size_t startIndex = 0) const noexcept
        -> std::size_t;
    /// Find the next one-code-point character that is not part of the given set.
    [[nodiscard]] auto indexNotOf(std::u32string_view characterSet, std::size_t startIndex = 0) const noexcept
        -> std::size_t;
    /// Calculate an absolute storage sub-range from local range coordinates.
    [[nodiscard]] auto subRange(std::size_t startIndex, std::size_t length = IndexRange::npos) const noexcept
        -> IndexRange;
    /// Calculate an absolute storage range cropped to a terminal display width.
    [[nodiscard]] auto croppedRange(Coordinate displayWidth, Alignment alignment) const noexcept -> IndexRange;
    /// Calculate an absolute storage range trimmed at both ends.
    [[nodiscard]] auto trimmedRange(std::u32string_view characters = {}) const noexcept -> IndexRange;
    /// Test if this range contains control characters.
    [[nodiscard]] auto containsControlCharacters() const noexcept -> bool;
    /// Split the range into absolute word ranges.
    [[nodiscard]] auto splitWordRanges() const noexcept -> std::vector<IndexRange>;
    /// Split the range into absolute line ranges.
    [[nodiscard]] auto splitLineRanges() const noexcept -> std::vector<IndexRange>;
    /// Count how many terminal lines this range occupies for a given terminal width.
    [[nodiscard]] auto terminalLines(int width) const noexcept -> int;
    /// Get the natural rectangular size for this range without wrapping.
    [[nodiscard]] auto naturalTextSize() const noexcept -> Size;

private:
    const StringData &_data; ///< The backing storage.
    IndexRange _range;       ///< The visible absolute range inside `_data`.
};

}
