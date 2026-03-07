// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Char.hpp"

#include <string>
#include <string_view>
#include <vector>


namespace erbsland::cterm {


/// A terminal string represented as a sequence of `Char` values.
class String {
public: // public types
    /// Storage container for the characters.
    using Storage = std::vector<Char>;
    /// Mutable forward iterator.
    using iterator = Storage::iterator;
    /// Immutable forward iterator.
    using const_iterator = Storage::const_iterator;
    /// Mutable reverse iterator.
    using reverse_iterator = Storage::reverse_iterator;
    /// Immutable reverse iterator.
    using const_reverse_iterator = Storage::const_reverse_iterator;
    /// Unsigned size type.
    using size_type = Storage::size_type;
    /// Signed distance type.
    using difference_type = Storage::difference_type;
    /// Stored value type.
    using value_type = Storage::value_type;
    /// Mutable element reference.
    using reference = Storage::reference;
    /// Immutable element reference.
    using const_reference = Storage::const_reference;
    /// Mutable element pointer.
    using pointer = Storage::pointer;
    /// Immutable element pointer.
    using const_pointer = Storage::const_pointer;

public: // ctors/dtor/assign/move
    /// Create an empty terminal string.
    String() = default;
    /// Create a terminal string from UTF-8 text.
    /// @param str The UTF-8 text to split into terminal characters.
    explicit String(std::string_view str);
    /// Copy construct a terminal string.
    String(const String &) = default;
    /// Move construct a terminal string.
    String(String &&) = default;
    /// Copy-assign a terminal string.
    auto operator=(const String &) -> String & = default;
    /// Move-assign a terminal string.
    auto operator=(String &&) -> String & = default;

public: // operators
    /// Access one character without bounds checking.
    /// @param index The character index.
    /// @return A copy of the character at `index`.
    [[nodiscard]] auto operator[](const size_t index) const noexcept -> Char { return _chars[index]; }
    /// Access one character without bounds checking.
    /// @param index The character index.
    /// @return A mutable reference to the character at `index`.
    [[nodiscard]] auto operator[](const size_t index) noexcept -> Char & { return _chars[index]; }
    /// Append one character to this string.
    /// @param character The character to append.
    /// @return Reference to this string.
    auto operator+=(const Char &character) noexcept -> String & {
        append(character);
        return *this;
    }
    /// Append another terminal string to this string.
    /// @param other The string to append.
    /// @return Reference to this string.
    auto operator+=(const String &other) noexcept -> String & {
        _chars.insert(_chars.end(), other._chars.begin(), other._chars.end());
        return *this;
    }
    /// Create a new string with one appended character.
    /// @param character The character to append.
    /// @return The concatenated string.
    auto operator+(const Char &character) const noexcept -> String {
        auto copy = *this;
        copy.append(character);
        return copy;
    }
    /// Create a new string with another appended string.
    /// @param other The string to append.
    /// @return The concatenated string.
    auto operator+(const String &other) const noexcept -> String {
        auto copy = *this;
        copy += other;
        return copy;
    }

public: // accessors
    /// Get the number of stored characters.
    [[nodiscard]] auto size() const noexcept -> size_t { return _chars.size(); }
    /// Get the width of the string in terminal cells.
    /// @return The sum of all character display widths.
    [[nodiscard]] auto displayWidth() const noexcept -> int;
    /// Access one character with bounds checking.
    /// @param index The character index.
    /// @return A copy of the character at `index`.
    [[nodiscard]] auto at(const size_t index) const -> Char { return _chars.at(index); }
    /// Test if this string is empty.
    [[nodiscard]] auto empty() const noexcept -> bool { return _chars.empty(); }
    /// Get an iterator to the first character.
    [[nodiscard]] auto begin() noexcept { return _chars.begin(); }
    /// Get an iterator past the last character.
    [[nodiscard]] auto end() noexcept { return _chars.end(); }
    /// Get a const iterator to the first character.
    [[nodiscard]] auto begin() const noexcept { return _chars.begin(); }
    /// Get a const iterator past the last character.
    [[nodiscard]] auto end() const noexcept { return _chars.end(); }
    /// Get a const iterator to the first character.
    [[nodiscard]] auto cbegin() const noexcept { return _chars.cbegin(); }
    /// Get a const iterator past the last character.
    [[nodiscard]] auto cend() const noexcept { return _chars.cend(); }
    /// Get a reverse iterator to the last character.
    [[nodiscard]] auto rbegin() noexcept { return _chars.rbegin(); }
    /// Get a reverse iterator past the first character.
    [[nodiscard]] auto rend() noexcept { return _chars.rend(); }
    /// Get a const reverse iterator to the last character.
    [[nodiscard]] auto rbegin() const noexcept { return _chars.rbegin(); }
    /// Get a const reverse iterator past the first character.
    [[nodiscard]] auto rend() const noexcept { return _chars.rend(); }
    /// Get a const reverse iterator to the last character.
    [[nodiscard]] auto crbegin() const noexcept { return _chars.crbegin(); }
    /// Get a const reverse iterator past the first character.
    [[nodiscard]] auto crend() const noexcept { return _chars.crend(); }

public: // modifiers
    /// Reserve storage for at least the given number of characters.
    /// @param size The requested capacity.
    void reserve(const size_t size) noexcept { _chars.reserve(size); }
    /// Remove all characters from this string.
    void clear() noexcept { _chars.clear(); }
    /// Append one character to this string.
    /// @param character The character to append.
    void append(const Char &character) noexcept { _chars.emplace_back(character); }

public: // tools
    /// Split the string into words at space, tab, carriage return, or newline characters.
    /// @return A sequence of words.
    [[nodiscard]] auto splitWords() const noexcept -> std::vector<String>;
    /// Wrap this string into lines that have a maximum display width.
    /// Paragraph breaks from CR/LF characters are preserved as blank lines between paragraphs.
    /// @param width The maximum terminal width in cells. Must be greater than zero.
    /// @return A sequence of lines.
    [[nodiscard]] auto wrapIntoLines(int width) const noexcept -> std::vector<String>;

private:
    [[nodiscard]] auto splitParagraphs() const noexcept -> std::vector<String>;
    [[nodiscard]] auto wrapParagraphIntoLines(int width) const noexcept -> std::vector<String>;
    [[nodiscard]] static auto splitCharacters(std::string_view str) -> std::vector<Char>;

private:
    std::vector<Char> _chars;
};


/// A sequence of wrapped terminal text lines.
using BlockStringLines = std::vector<String>;


}
