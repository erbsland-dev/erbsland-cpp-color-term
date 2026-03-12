// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Char.hpp"
#include "ParagraphSpacing.hpp"

#include "impl/TypeTraits.hpp"

#include <string>
#include <string_view>
#include <vector>


namespace erbsland::cterm {


/// A terminal string represented as a sequence of `Char` values.
class String {
public:
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
    /// No valid position
    static constexpr std::size_t npos = std::numeric_limits<std::size_t>::max();

public:
    /// Create an empty terminal string.
    String() = default;
    /// Create a terminal string from UTF-8 text.
    /// @param str The UTF-8 text to split into terminal characters.
    /// @param color The color to use for the characters.
    /// Control codes are ignored except for tab and newline.
    /// @throws std::invalid_argument If the text is not valid UTF-8 or contains an unsupported character sequence.
    explicit String(std::string_view str, Color color = {});
    /// Create a terminal string from UTF-32 text.
    /// @param str The UTF-32 text to split into terminal characters.
    /// @param color The color to use for the characters.
    /// Control codes are ignored except for tab and newline.
    /// @throws std::invalid_argument If the text contains an unsupported character sequence.
    explicit String(std::u32string_view str, Color color = {});
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
    [[nodiscard]] auto operator[](const std::size_t index) const noexcept -> Char { return _chars[index]; }
    /// Access one character without bounds checking.
    /// @param index The character index.
    /// @return A mutable reference to the character at `index`.
    [[nodiscard]] auto operator[](const std::size_t index) noexcept -> Char & { return _chars[index]; }
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
    [[nodiscard]] auto size() const noexcept -> std::size_t { return _chars.size(); }
    /// Get the width of the string in terminal cells.
    /// @return The sum of all character display widths.
    [[nodiscard]] auto displayWidth() const noexcept -> int;
    /// Access one character with bounds checking.
    /// @param index The character index.
    /// @return A copy of the character at `index`.
    [[nodiscard]] auto at(const std::size_t index) const -> Char { return _chars.at(index); }
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
    /// Count the number of characters with a given color.
    /// @param character The character to count.
    /// @return The number of characters in this string.
    [[nodiscard]] auto count(Char character) const noexcept -> std::size_t;
    /// Count the number of characters matching any color.
    /// @param character The character to count (1 code-point).
    /// @return The number of characters in this string.
    [[nodiscard]] auto count(char32_t character) const noexcept -> std::size_t;
    /// Get the index of the next character with a given color.
    /// If startIndex is out of bounds, returns npos.
    /// If no character is found, returns npos.
    /// @param character The character to search for. Compares both, character and color!
    /// @param startIndex The start index to search from. Defaults to 0.
    /// @return The index of the next character or npos if not found.
    [[nodiscard]] auto indexOf(Char character, std::size_t startIndex = 0) const noexcept -> std::size_t;
    /// Get the index of the next character matching any color.
    /// If startIndex is out of bounds, returns npos.
    /// If no character is found, returns npos.
    /// @param character The character to search for. Only compares the character (1 code-point).
    /// @param startIndex The start index to search from. Defaults to 0.
    /// @return The index of the next character or npos if not found.
    [[nodiscard]] auto indexOf(char32_t character, std::size_t startIndex = 0) const noexcept -> std::size_t;
    /// Get a substring.
    /// If startIndex is out of bounds, returns an empty string.
    /// If length exceeds the string length, returns the remaining characters.
    /// @param startIndex The start index of the substring.
    /// @param length The number of characters after the start index.
    /// @return The substring or an empty string if startIndex is out of bounds.
    [[nodiscard]] auto substr(std::size_t startIndex, std::size_t length = npos) const noexcept -> String;

public: // modifiers
    /// Reserve storage for at least the given number of characters.
    /// @param size The requested capacity.
    void reserve(const std::size_t size) noexcept { _chars.reserve(size); }
    /// Remove all characters from this string.
    void clear() noexcept { _chars.clear(); }
    /// Append elements to this string.
    /// This works similar to `Terminal::print()`.
    /// If you add a color, this color is "active" for all following characters *in the same call*.
    /// Just adding a color does not change the string.
    /// @param args The arguments to append.
    template <PrintableArg... Args>
    void append(Args... args) noexcept {
        Color currentColor{};
        (appendElement(args, currentColor), ...);
    }

public: // tools
    /// Split the string into words at space, tab, carriage return, or newline characters.
    /// @return A sequence of words.
    [[nodiscard]] auto splitWords() const noexcept -> std::vector<String>;
    /// Wrap this string into lines that have a maximum display width.
    /// Paragraph breaks from newline characters are preserved using the selected paragraph spacing.
    /// @param width The maximum terminal width in cells. Must be greater than zero.
    /// @param paragraphSpacing The spacing to use between newline-separated paragraphs.
    /// @return A sequence of lines.
    [[nodiscard]] auto
    wrapIntoLines(int width, ParagraphSpacing paragraphSpacing = ParagraphSpacing::SingleLine) const noexcept
        -> std::vector<String>;
    /// Splits this string into individual lines.
    /// The string is split at the NL character that is not included in the result.
    /// Empty lines are preserved.
    /// A NL at the end of the string does not generate an additional empty line.
    /// @return A sequence of lines.
    [[nodiscard]] auto splitLines() const noexcept -> std::vector<String>;

public: // conversion
    /// Create a new string from a list of lines.
    /// All lines are joined using a new-line character.
    /// @param lines The lines for the string.
    /// @param color The base color to use for each character.
    /// @return The new string.
    [[nodiscard]] static auto fromLines(std::initializer_list<std::string_view> lines, Color color = {}) noexcept
        -> String;
    /// @overload
    [[nodiscard]] static auto fromLines(std::initializer_list<std::u32string_view> lines, Color color = {}) noexcept
        -> String;

private:
    explicit String(std::vector<Char> &&chars) noexcept : _chars(std::move(chars)) {}
    [[nodiscard]] static auto countCharacters(std::string_view str) -> std::size_t;
    [[nodiscard]] static auto countCharacters(std::u32string_view str) -> std::size_t;
    [[nodiscard]] static auto splitCharacters(std::string_view str, Color color = {}) -> Storage;
    [[nodiscard]] static auto splitCharacters(std::u32string_view str, Color color = {}) -> Storage;
    [[nodiscard]] constexpr static auto isControlCode(const char32_t codePoint) noexcept -> bool {
        return codePoint < 0x20 || (codePoint >= 0x7FU && codePoint <= 0x9FU);
    }
    [[nodiscard]] static auto isStringCharacter(char32_t codePoint) noexcept -> bool;
    static void appendCodePoint(Storage &chars, char32_t codePoint, Color color);
    void appendElement(const Foreground foreground, Color &currentColor) noexcept { currentColor.setFg(foreground); }
    void appendElement(const Background background, Color &currentColor) noexcept { currentColor.setBg(background); }
    void appendElement(const Char &character, [[maybe_unused]] Color &currentColor) noexcept {
        _chars.emplace_back(character);
    }
    void appendElement(const String &other, [[maybe_unused]] Color &currentColor) noexcept {
        _chars.insert(_chars.end(), other._chars.begin(), other._chars.end());
    }
    void appendElement(const std::string_view str, Color &currentColor) noexcept {
        const auto newCharacters = splitCharacters(str, currentColor);
        _chars.insert(_chars.end(), newCharacters.begin(), newCharacters.end());
    }

private:
    std::vector<Char> _chars;
};


/// A sequence of wrapped terminal text lines.
using StringLines = std::vector<String>;


}
