// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "IndexRange.hpp"
#include "ParagraphSpacing.hpp"
#include "String.hpp"

#include <memory>
#include <vector>

namespace erbsland::cterm {

namespace impl {
class StringData;
}

/// A read-only view onto shared terminal string data.
class StringView final {
public:
    using Storage = std::vector<Char>;                              ///< Storage container for the characters.
    using const_iterator = Storage::const_iterator;                 ///< Immutable forward iterator.
    using const_reverse_iterator = Storage::const_reverse_iterator; ///< Immutable reverse iterator.
    using size_type = Storage::size_type;                           ///< Unsigned size type.
    using difference_type = Storage::difference_type;               ///< Signed distance type.
    using value_type = Storage::value_type;                         ///< Stored value type.
    using const_reference = Storage::const_reference;               ///< Immutable element reference.
    using const_pointer = Storage::const_pointer;                   ///< Immutable element pointer.

    /// No valid position.
    static constexpr std::size_t npos = IndexRange::npos;

public:
    /// Create an empty string view.
    StringView() noexcept;
    /// Create a string view from a string.
    /// This conversion is implicit so APIs can migrate from `String` to `StringView`.
    /// @param string The source string.
    StringView(const String &string) noexcept;

    // defaults
    ~StringView() = default;
    StringView(const StringView &) = default;
    StringView(StringView &&) = default;
    auto operator=(const StringView &) -> StringView & = default;
    auto operator=(StringView &&) -> StringView & = default;

public: // operators
    /// Access one character without bounds checking.
    /// @param index The character index.
    /// @return A copy of the character at `index`.
    [[nodiscard]] auto operator[](std::size_t index) const noexcept -> Char;

public: // accessors
    /// Get the number of stored characters.
    [[nodiscard]] auto size() const noexcept -> std::size_t { return _range.length(); }
    /// Get the width of the string in terminal cells.
    /// @return The sum of all character display widths.
    [[nodiscard]] auto displayWidth() const noexcept -> int;
    /// Test if this string view is empty.
    [[nodiscard]] auto empty() const noexcept -> bool { return _range.empty(); }
    /// Access one character with bounds checking.
    /// @param index The character index.
    /// @return A copy of the character at `index`.
    [[nodiscard]] auto at(std::size_t index) const -> Char;
    /// Get an iterator to the first character.
    [[nodiscard]] auto begin() const noexcept -> const_iterator;
    /// Get an iterator past the last character.
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
    /// Count the number of characters matching a fully styled character.
    /// @param character The character to count.
    /// @return The number of matching characters in this view.
    [[nodiscard]] auto count(const Char &character) const noexcept -> std::size_t;
    /// Count the number of characters matching one code point regardless of style.
    /// @param character The character to count.
    /// @return The number of matching characters in this view.
    [[nodiscard]] auto count(char32_t character) const noexcept -> std::size_t;
    /// Get the index of the next character with a given full style.
    /// @param character The character to search for.
    /// @param startIndex The first local index to inspect.
    /// @return The local index of the next match, or `npos`.
    [[nodiscard]] auto indexOf(const Char &character, std::size_t startIndex = 0) const noexcept -> std::size_t;
    /// Get the index of the next character matching one code point regardless of style.
    /// @param character The character to search for.
    /// @param startIndex The first local index to inspect.
    /// @return The local index of the next match, or `npos`.
    [[nodiscard]] auto indexOf(char32_t character, std::size_t startIndex = 0) const noexcept -> std::size_t;
    /// Get a sub-view.
    /// If startIndex is out of bounds, returns an empty view.
    /// If length exceeds the view length, returns the remaining characters.
    /// @param startIndex The start index of the substring.
    /// @param length The number of characters after the start index.
    /// @return The substring view or an empty view if startIndex is out of bounds.
    [[nodiscard]] auto substr(std::size_t startIndex, std::size_t length = npos) const noexcept -> StringView;
    /// Trim the given characters from the beginning and end of the view.
    /// Only single-code-point characters are matched.
    /// @param characters The characters to remove from both ends.
    /// @return A trimmed string view.
    [[nodiscard]] auto trimmed(std::u32string_view characters) const noexcept -> StringView;

public: // tests
    /// Test if this string view contains control characters.
    /// As most control codes are filtered on construction, this mainly tests for NL and TAB.
    [[nodiscard]] auto containsControlCharacters() const noexcept -> bool;

public: // tools
    /// Split the view into words at space, tab, carriage return, or newline characters.
    /// @return A sequence of word views.
    [[nodiscard]] auto splitWords() const noexcept -> std::vector<StringView>;
    /// Wrap this view into lines that have a maximum display width.
    /// Paragraph breaks from newline characters are preserved using the selected paragraph spacing.
    /// @param width The maximum terminal width in cells. Must be greater than zero.
    /// @param paragraphSpacing The spacing to use between newline-separated paragraphs.
    /// @return A sequence of materialized lines.
    [[nodiscard]] auto
    wrapIntoLines(int width, ParagraphSpacing paragraphSpacing = ParagraphSpacing::SingleLine) const noexcept
        -> StringLines;
    /// Count how many terminal lines this view occupies for a given terminal width.
    /// @param width The available terminal width in cells. Must be greater than zero.
    /// @return The number of occupied terminal lines.
    [[nodiscard]] auto terminalLines(int width) const noexcept -> int;
    /// Split this view into individual lines.
    /// The string is split at the NL character that is not included in the result.
    /// Empty lines are preserved.
    /// A NL at the end of the view does not generate an additional empty line.
    /// @return A sequence of line views.
    [[nodiscard]] auto splitLines() const noexcept -> std::vector<StringView>;

private:
    friend class String;

    StringView(std::shared_ptr<impl::StringData> data, IndexRange range) noexcept;

    [[nodiscard]] auto storageIndex(std::size_t localIndex) const noexcept -> std::size_t {
        return _range.startIndex() + localIndex;
    }
    [[nodiscard]] auto characterAt(std::size_t localIndex) const noexcept -> const Char &;

private:
    std::shared_ptr<impl::StringData> _data; ///< Shared backing storage.
    IndexRange _range;                       ///< Visible sub-range inside `_data`.
};

}
