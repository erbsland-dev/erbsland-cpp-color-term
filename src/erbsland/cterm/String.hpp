// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Char.hpp"
#include "EncodingErrors.hpp"
#include "IndexRange.hpp"
#include "ParagraphSpacing.hpp"

#include "geometry/Size.hpp"
#include "impl/TypeTraits.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace erbsland::cterm {

class StringView;
class TextOptions;

namespace impl {
class StringBuilder;
class StringData;
}

/// A terminal string represented as a sequence of `Char` values.
///
/// `String` is implicitly shared. Copying a string shares its backing data, and a deep copy is only made when one
/// instance is modified through a mutating API.
class String {
public:
    using Storage = std::vector<Char>;                              ///< Storage container for the characters.
    using iterator = Storage::iterator;                             ///< Mutable forward iterator.
    using const_iterator = Storage::const_iterator;                 ///< Immutable forward iterator.
    using reverse_iterator = Storage::reverse_iterator;             ///< Mutable reverse iterator.
    using const_reverse_iterator = Storage::const_reverse_iterator; ///< Immutable reverse iterator.
    using size_type = Storage::size_type;                           ///< Unsigned size type.
    using difference_type = Storage::difference_type;               ///< Signed distance type.
    using value_type = Storage::value_type;                         ///< Stored value type.
    using reference = Storage::reference;                           ///< Mutable element reference.
    using const_reference = Storage::const_reference;               ///< Immutable element reference.
    using pointer = Storage::pointer;                               ///< Mutable element pointer.
    using const_pointer = Storage::const_pointer;                   ///< Immutable element pointer.

    /// No valid position.
    static constexpr std::size_t npos = IndexRange::npos;

public:
    /// Create an empty terminal string.
    String() noexcept;
    /// Create a terminal string from UTF-8 text.
    /// @param str The UTF-8 text to split into terminal characters.
    /// @param encodingErrors How UTF-8 encoding errors are handled.
    /// Control codes are ignored except for tab and newline.
    /// Use `EncodingErrors::Throw` to make malformed UTF-8 explicit at the string-construction boundary.
    /// `EncodingErrors::Replace` replaces each malformed UTF-8 byte with one Unicode replacement character.
    /// @throws std::invalid_argument If `encodingErrors` is `EncodingErrors::Throw` and the text is not valid UTF-8,
    /// or if the text contains an unsupported character sequence.
    explicit String(std::string_view str, EncodingErrors encodingErrors = EncodingErrors::Replace);
    /// Create a terminal string from UTF-8 text with a uniform style.
    /// @param str The UTF-8 text to split into terminal characters.
    /// @param style The style to use for the characters.
    /// @param encodingErrors How UTF-8 encoding errors are handled.
    /// Control codes are ignored except for tab and newline.
    /// Use `EncodingErrors::Throw` to make malformed UTF-8 explicit at the string-construction boundary.
    /// `EncodingErrors::Replace` replaces each malformed UTF-8 byte with one Unicode replacement character.
    /// @throws std::invalid_argument If `encodingErrors` is `EncodingErrors::Throw` and the text is not valid UTF-8,
    /// or if the text contains an unsupported character sequence.
    explicit String(std::string_view str, CharStyle style, EncodingErrors encodingErrors = EncodingErrors::Replace);
    /// Create a terminal string from UTF-32 text.
    /// @param str The UTF-32 text to split into terminal characters.
    /// Control codes are ignored except for tab and newline.
    /// @throws std::invalid_argument If the text contains an unsupported character sequence.
    explicit String(std::u32string_view str);
    /// Create a terminal string from UTF-32 text with a uniform style.
    /// @param str The UTF-32 text to split into terminal characters.
    /// @param style The style to use for the characters.
    /// Control codes are ignored except for tab and newline.
    /// @throws std::invalid_argument If the text contains an unsupported character sequence.
    explicit String(std::u32string_view str, CharStyle style);
    /// Create a terminal string repeating the same `Char`.
    /// @param count The repetition count. Limited to 10'000'000.
    /// @param character The character to repeat.
    explicit String(std::size_t count, Char character) noexcept;
    /// Create a terminal string by materializing one read-only view.
    /// @param view The string view to copy into an owned string.
    explicit String(const StringView &view);

    // defaults
    ~String() = default;
    String(const String &) = default;
    String(String &&) = default;
    auto operator=(const String &) -> String & = default;
    auto operator=(String &&) -> String & = default;

public: // operators
    /// Compare two strings.
    /// Two strings are only equal, if all characters and styles are equal.
    auto operator==(const String &other) const noexcept -> bool;
    auto operator!=(const String &other) const noexcept -> bool { return !operator==(other); }
    /// Access one character without bounds checking.
    /// @param index The character index.
    /// @return A copy of the character at `index`, or `Char{}` if `index` is out of bounds.
    [[nodiscard]] auto operator[](std::size_t index) const noexcept -> Char;
    /// Access one character without bounds checking.
    /// @param index The character index.
    /// @return A mutable reference to the character at `index`, or a discarded `Char{}` if `index` is out of bounds.
    [[nodiscard]] auto operator[](std::size_t index) noexcept -> Char &;
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
    auto operator+=(const String &other) noexcept -> String &;
    /// Append a read-only string view to this string.
    /// @param other The string view to append.
    /// @return Reference to this string.
    auto operator+=(const StringView &other) noexcept -> String &;
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
    auto operator+(const String &other) const noexcept -> String;
    /// Create a new string with an appended read-only view.
    /// @param other The string view to append.
    /// @return The concatenated string.
    auto operator+(const StringView &other) const noexcept -> String;

public: // accessors
    /// Get the number of stored characters.
    [[nodiscard]] auto size() const noexcept -> std::size_t { return _range.length(); }
    /// Get the width of the string in terminal cells.
    /// @return The sum of all character display widths.
    [[nodiscard]] auto displayWidth() const noexcept -> int;
    /// Test if this string is empty.
    [[nodiscard]] auto empty() const noexcept -> bool { return _range.empty(); }
    /// Access one character with bounds checking.
    /// @param index The character index.
    /// @return A copy of the character at `index`.
    [[nodiscard]] auto at(std::size_t index) const -> Char;
    /// Get an iterator to the first character.
    [[nodiscard]] auto begin() noexcept -> iterator;
    /// Get an iterator past the last character.
    [[nodiscard]] auto end() noexcept -> iterator;
    /// Get a const iterator to the first character.
    [[nodiscard]] auto begin() const noexcept -> const_iterator;
    /// Get a const iterator past the last character.
    [[nodiscard]] auto end() const noexcept -> const_iterator;
    /// Get a const iterator to the first character.
    [[nodiscard]] auto cbegin() const noexcept -> const_iterator;
    /// Get a const iterator past the last character.
    [[nodiscard]] auto cend() const noexcept -> const_iterator;
    /// Get a reverse iterator to the last character.
    [[nodiscard]] auto rbegin() noexcept -> reverse_iterator;
    /// Get a reverse iterator past the first character.
    [[nodiscard]] auto rend() noexcept -> reverse_iterator;
    /// Get a const reverse iterator to the last character.
    [[nodiscard]] auto rbegin() const noexcept -> const_reverse_iterator;
    /// Get a const reverse iterator past the first character.
    [[nodiscard]] auto rend() const noexcept -> const_reverse_iterator;
    /// Get a const reverse iterator to the last character.
    [[nodiscard]] auto crbegin() const noexcept -> const_reverse_iterator;
    /// Get a const reverse iterator past the first character.
    [[nodiscard]] auto crend() const noexcept -> const_reverse_iterator;
    /// Count the number of characters with a given color.
    /// @param character The character to count.
    /// @return The number of characters in this string.
    [[nodiscard]] auto count(const Char &character) const noexcept -> std::size_t;
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
    [[nodiscard]] auto indexOf(const Char &character, std::size_t startIndex = 0) const noexcept -> std::size_t;
    /// Get the index of the next matching character.
    /// Ignores the character style.
    /// If startIndex is out of bounds, returns npos.
    /// If no character is found, returns npos.
    /// @param character The character to search for. Only compares single-code-point characters.
    /// @param startIndex The start index to search from. Defaults to 0.
    /// @return The index of the next character or npos if not found.
    [[nodiscard]] auto indexOf(char32_t character, std::size_t startIndex = 0) const noexcept -> std::size_t;
    /// Get the index of the next matching character.
    /// Ignores the character style.
    /// If startIndex is out of bounds, returns npos.
    /// If no character is found, returns npos.
    /// This function matches **any** character in the character set.
    /// @param characterSet The character-set to search for. Only compares single-code-point characters.
    /// @param startIndex The start index to search from. Defaults to 0.
    /// @return The index of the next character or npos if not found.
    [[nodiscard]] auto indexOf(std::u32string_view characterSet, std::size_t startIndex = 0) const noexcept
        -> std::size_t;
    /// Get the index of the next **not** matching character.
    /// Ignores the character style.
    /// If startIndex is out of bounds, returns npos.
    /// If no character is found, returns npos.
    /// This function matches if **no** character in the character set matches the text.
    /// @param characterSet The character-set to search for. Only compares single-code-point characters.
    /// @param startIndex The start index to search from. Defaults to 0.
    /// @return The index of the next character or npos if not found.
    [[nodiscard]] auto indexNotOf(std::u32string_view characterSet, std::size_t startIndex = 0) const noexcept
        -> std::size_t;
    /// Get a substring.
    /// If startIndex is out of bounds, returns an empty string.
    /// If length exceeds the string length, returns the remaining characters.
    /// @param startIndex The start index of the substring.
    /// @param length The number of characters after the start index.
    /// @return The substring or an empty string if startIndex is out of bounds.
    [[nodiscard]] auto substr(std::size_t startIndex, std::size_t length = npos) const noexcept -> String;
    /// Get a substring that fits into the given display width.
    /// If a double-sized character is at the edge, it isn't included in the result.
    /// Therefore, the resulting string may be shorter than the display width.
    /// @param displayWidth The maximum width of the substring in display units.
    /// @param alignment The alignment of the cropped text. Only `Alignment::Left` and `Alignment::Right` are supported.
    /// @return The cropped substring or an empty string if displayWidth is <=0.
    [[nodiscard]] auto croppedToDisplayWidth(Coordinate displayWidth, Alignment alignment) const noexcept -> String;
    /// Return a string with the given characters trimmed from the beginning and end.
    /// Only single-code-point characters are matched.
    /// @param characters The characters to remove from both ends. If empty, removes space, tab, and newline characters.
    /// @return A copy without matching leading and trailing characters.
    [[nodiscard]] auto trimmed(std::u32string_view characters = {}) const noexcept -> String;
    /// Return a normalized string.
    /// Trim the given characters from start/end, and eplace any number of the given characters inside the
    /// string with a single separator.
    /// Only single-code-point characters are matched.
    /// The style of replaced characters is determined by using the first matches character style
    /// as a base for `separator`.
    /// @param characters The characters to match. If empty: space, tab, and newline characters.
    /// @param separator The separator to use.
    [[nodiscard]] auto normalized(std::u32string_view characters = {}, Char separator = Char::space()) const noexcept
        -> String;

public: // tests
    /// Test if this string contains control characters.
    /// Most control codes are filtered on construction, therefore, this test searched for NL and TAB.
    [[nodiscard]] auto containsControlCharacters() const noexcept -> bool;

public: // modifiers
    /// Reserve storage for at least the given number of characters.
    /// @param size The requested capacity.
    void reserve(std::size_t size) noexcept;
    /// Remove all characters from this string.
    void clear() noexcept;
    /// Trim the given characters from the beginning and end of the string.
    /// Only single-code-point characters are matched.
    /// @param characters The characters to remove from both ends. If empty, removes space, tab, and newline characters.
    void trim(std::u32string_view characters = {}) noexcept;
    /// Normalize this string.
    /// Trim the given characters from start/end, and eplace any number of the given characters inside the
    /// string with a single separator.
    /// Only single-code-point characters are matched.
    /// The style of replaced characters is determined by using the first matches character style
    /// as a base for `separator`.
    /// @param characters The characters to match. If empty: space, tab, and newline characters.
    /// @param separator The separator to use.
    void normalize(std::u32string_view characters = {}, Char separator = Char::space()) noexcept;
    /// Replace characters in this string.
    /// If the range is out of bounds, it is clamped to the string's length.
    /// If the range is outside the string or empty, no replacement is performed.
    /// If the operation does not change the string, no replacement is performed.
    /// @param range The range of characters to replace.
    /// @param replacement The replacement for this range.
    void replace(IndexRange range, Char replacement) noexcept;
    /// @overload
    void replace(IndexRange range, const StringView &replacement) noexcept;
    /// Remove a part of the string.
    /// If the range is out of bounds, it is clamped to the string's length.
    /// If the range is outside the string or empty, no replacement is performed.
    void remove(IndexRange range) noexcept;
    /// Set a character in this string.
    /// This call is more efficient than using the index operator.
    void set(std::size_t index, Char character) noexcept;
    /// Append another terminal string with a base style.
    /// If `pos` is out of range, appends the string at the end.
    /// @param pos The position to insert the string at.
    /// @param other The source text view.
    /// @param style The style used as base for inherited components in the appended range.
    void insertWithBaseStyle(std::size_t pos, const StringView &other, CharStyle style) noexcept;
    /// Append text using one uniform style.
    /// @param text The text to append.
    /// @param style The style applied to the appended characters.
    void appendStyled(std::string_view text, CharStyle style) noexcept;
    /// @overload
    void appendStyled(std::u32string_view text, CharStyle style) noexcept;
    /// Append another terminal string with a base style.
    /// @param other The source text view.
    /// @param style The style used as base for inherited components in the appended range.
    void appendStyled(const StringView &other, CharStyle style) noexcept;
    /// Append a repeated character.
    /// @param count The repetition count. Limited to 10'000'000.
    /// @param character The character to repeat.
    void append(std::size_t count, Char character) noexcept;
    /// Append a repeated Unicode code point with a uniform style.
    /// @param count The repetition count. Limited to 10'000'000.
    /// @param character The character to repeat.
    /// @param style The style for every appended character.
    void append(std::size_t count, char32_t character, CharStyle style) noexcept;
    /// Append elements to this string.
    /// This works similar to `Terminal::print()`.
    /// If you add a color, this color is "active" for all following characters *in the same call*.
    /// If you add character attributes, these attributes are active for all following characters *in the same call*.
    /// Appended `Char`, `String`, and `StringView` values with inherited color components or inherited attributes
    /// resolve against the currently active state.
    /// Just adding a color does not change the string.
    /// @param args The arguments to append.
    template <PrintableArg... Args>
    void append(Args... args) noexcept {
        auto style = CharStyle{};
        (appendElement(args, style), ...);
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
    /// Count how many terminal lines this string occupies for a given terminal width.
    /// Newline characters start a new terminal line and printable characters wrap at the given width.
    /// The result is undefined for abstract terminal widths smaller than 10 cells.
    /// @param width The available terminal width in cells. Must be greater than zero.
    /// @return The number of occupied terminal lines.
    [[nodiscard]] auto terminalLines(int width) const noexcept -> int;
    /// Get the natural rectangular size for this text without wrapping.
    /// The returned size is at least 1x1, preserves explicit non-trailing newline characters as separate lines,
    /// and uses terminal cell width for wide and combining characters.
    /// @return The natural text size in terminal cells.
    [[nodiscard]] auto naturalTextSize() const noexcept -> Size;
    /// Calculate the height required to render this text with `WritableBuffer::drawText()`.
    /// The given width is the full target rectangle width, including margins configured in `options`.
    /// @param width The available rectangle width in terminal cells.
    /// @param options The text options used for paragraph layout.
    /// @return The required rectangle height in terminal cells.
    [[nodiscard]] auto wrappedTextHeight(Coordinate width, const TextOptions &options) const noexcept -> Coordinate;
    /// Splits this string into individual lines.
    /// The string is split at the NL character that is not included in the result.
    /// Empty lines are preserved.
    /// A NL at the end of the string does not generate an additional empty line.
    /// @return A sequence of lines.
    [[nodiscard]] auto splitLines() const noexcept -> std::vector<String>;
    /// Create a new string with the given style applied as a base.
    /// @param style The style used as base for the resulting string.
    /// @return A new string with the base style applied.
    [[nodiscard]] auto withBase(CharStyle style) const noexcept -> String;

public: // conversion
    /// Create a new string from a list of lines.
    /// All lines are joined using a new-line character.
    /// @param lines The lines for the string.
    /// @param color The base color to use for each character.
    /// @param attributes The base attributes to use for each character.
    /// Invalid UTF-8 bytes are replaced with the Unicode replacement character.
    /// @return The new string.
    [[nodiscard]] static auto
    fromLines(std::initializer_list<std::string_view> lines, Color color = {}, CharAttributes attributes = {}) noexcept
        -> String;
    /// @overload
    [[nodiscard]] static auto fromLines(std::initializer_list<std::string_view> lines, CharStyle style) noexcept
        -> String;
    /// @overload
    [[nodiscard]] static auto fromLines(
        std::initializer_list<std::u32string_view> lines, Color color = {}, CharAttributes attributes = {}) noexcept
        -> String;
    /// @overload
    [[nodiscard]] static auto fromLines(std::initializer_list<std::u32string_view> lines, CharStyle style) noexcept
        -> String;

public: // legacy
    [[deprecated("Please use appendStyled()")]]
    void appendWithBaseStyle(const StringView &other, CharStyle style) noexcept {
        appendStyled(other, style);
    }
    [[deprecated("Please use append(other.substr(startIndex, length)). Its COW and fast.")]]
    void appendRange(const StringView &other, std::size_t startIndex, std::size_t length = npos) noexcept;
    [[deprecated("Please use appendStyles(other.substr(startIndex, length)). Its COW and fast.")]]
    void appendRangeWithBaseStyle(
        const StringView &other, std::size_t startIndex, std::size_t length, CharStyle style) noexcept;

private:
    friend class StringView;
    friend class impl::StringBuilder;

    explicit String(std::shared_ptr<impl::StringData> data, IndexRange range) noexcept;
    explicit String(Storage chars) noexcept;
    [[nodiscard]] static auto fromStorageWithDisplayWidth(Storage chars, int displayWidth) noexcept -> String;

    [[nodiscard]] static auto splitCharacters(
        std::string_view str,
        Color color = {},
        CharAttributes attributes = {},
        EncodingErrors encodingErrors = EncodingErrors::Replace) -> Storage;
    [[nodiscard]] static auto splitCharacters(std::u32string_view str, Color color = {}, CharAttributes attributes = {})
        -> Storage;
    void appendElement(Color color, CharStyle &style) noexcept;
    void appendElement(Foreground::Hue foreground, CharStyle &style) noexcept;
    void appendElement(Foreground foreground, CharStyle &style) noexcept;
    void appendElement(Background::Hue background, CharStyle &style) noexcept;
    void appendElement(Background background, CharStyle &style) noexcept;
    void appendElement(CharStyle overlayStyle, CharStyle &style) noexcept;
    void appendElement(CharAttributes attributes, CharStyle &style) noexcept;
    void appendElement(const Char &character, CharStyle &style) noexcept;
    void appendElement(const String &other, CharStyle &style) noexcept;
    void appendElement(const StringView &other, CharStyle &style) noexcept;
    void appendElement(std::u32string_view str, CharStyle &style) noexcept;
    void appendElement(std::string_view str, CharStyle &style) noexcept;
    void appendView(const StringView &view, CharStyle style) noexcept;
    void appendViewWithBaseStyle(const StringView &view, CharStyle style) noexcept;
    void detach();
    void syncRangeWithStorage() noexcept;
    [[nodiscard]] auto characterAt(std::size_t index) const noexcept -> const Char &;
    [[nodiscard]] static auto ignoredMutableCharacter() noexcept -> Char &;

private:
    std::shared_ptr<impl::StringData> _data; ///< Shared backing storage.
    IndexRange _range;                       ///< Visible sub-range inside `_data`.
};

/// A sequence of wrapped terminal text lines.
using StringLines = std::vector<String>;

}
