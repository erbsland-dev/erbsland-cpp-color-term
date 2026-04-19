// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <cstddef>
#include <limits>

namespace erbsland::cterm {

/// A contiguous range of elements in sequence data.
class IndexRange final {
public:
    /// No valid position or open-ended length marker.
    static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

public:
    /// Create an empty (0,0) range.
    IndexRange() noexcept = default;

    /// Create a new range with a start index and length.
    /// @param startIndex The start index.
    /// @param length The number of elements in the range.
    constexpr IndexRange(const std::size_t startIndex, const std::size_t length) noexcept :
        _startIndex{startIndex}, _length{length} {}

    // defaults
    IndexRange(const IndexRange &) = default;
    auto operator=(const IndexRange &) -> IndexRange & = default;

public: // operators
    auto operator==(const IndexRange &) const noexcept -> bool = default;
    auto operator!=(const IndexRange &) const noexcept -> bool = default;

public:
    /// Get the start index.
    [[nodiscard]] constexpr auto startIndex() const noexcept -> std::size_t { return _startIndex; }
    /// Set the start index.
    void setStartIndex(const std::size_t startIndex) noexcept { _startIndex = startIndex; }
    /// Get the number of elements in the range.
    [[nodiscard]] constexpr auto length() const noexcept -> std::size_t { return _length; }
    /// Set the number of elements in the range.
    void setLength(const std::size_t length) noexcept { _length = length; }
    /// Test if this range contains no elements.
    [[nodiscard]] constexpr auto empty() const noexcept -> bool { return _length == 0; }
    /// Get the exclusive end index for this range.
    /// @return `startIndex + length`, or `npos` if the length is open ended.
    [[nodiscard]] constexpr auto endIndex() const noexcept -> std::size_t {
        if (_length == npos) {
            return npos;
        }
        return _startIndex + _length;
    }

public:
    /// Clamp this range to a valid container size.
    /// @param totalSize The total number of elements in the container.
    /// @return A range that stays fully inside `0..totalSize`.
    [[nodiscard]] constexpr auto clampedTo(const std::size_t totalSize) const noexcept -> IndexRange {
        if (_startIndex >= totalSize) {
            return IndexRange{totalSize, 0};
        }
        if (_length == npos || _length > totalSize - _startIndex) {
            return IndexRange{_startIndex, totalSize - _startIndex};
        }
        return *this;
    }

private:
    std::size_t _startIndex{0}; ///< The first character index in the range.
    std::size_t _length{0};     ///< The number of characters in the range.
};

}
