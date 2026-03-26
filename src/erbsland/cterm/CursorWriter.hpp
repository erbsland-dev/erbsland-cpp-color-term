// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "MoveMode.hpp"
#include "ParagraphOptions.hpp"
#include "Position.hpp"
#include "Size.hpp"
#include "String.hpp"

#include "impl/TypeTraits.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>


namespace erbsland::cterm {


class CursorWriter;
using CursorWriterPtr = std::shared_ptr<CursorWriter>;
class ReadableBuffer;


/// The shared interface for buffers/terminals that support cursor-based output.
class CursorWriter {
public:
    virtual ~CursorWriter() = default;

public: // cursor state
    /// Get the current color.
    /// @return The currently tracked terminal color state.
    [[nodiscard]] virtual auto color() const noexcept -> Color = 0;
    /// Get the current character attributes.
    /// @return The currently tracked character attribute state.
    [[nodiscard]] virtual auto charAttributes() const noexcept -> CharAttributes = 0;
    /// Get the current combined text style.
    /// @return The currently tracked terminal style.
    [[nodiscard]] auto style() const noexcept -> CharStyle { return CharStyle{color(), charAttributes()}; }
    /// Set foreground and background color.
    /// @note `Inherited` colors are converted to `Default` colors.
    /// @param color The new combined color.
    virtual void setColor(Color color) noexcept = 0;
    /// Set all character attributes.
    /// Unspecified attributes are treated as disabled.
    /// @param attributes The new character attributes.
    virtual void setCharAttributes(CharAttributes attributes) noexcept = 0;
    /// Set the full terminal style.
    /// Inherited colors are converted to defaults and unspecified attributes are treated as disabled.
    /// @param style The new combined style.
    void setStyle(const CharStyle style) noexcept {
        setColor(style.color());
        setCharAttributes(style.attributes());
    }
    /// Set foreground and background color.
    /// @note `Inherited` colors are converted to `Default` colors.
    /// @param foregroundColor The new foreground color.
    /// @param backgroundColor The new background color.
    void setColor(const Foreground foregroundColor, const Background backgroundColor) noexcept {
        setColor(Color{foregroundColor, backgroundColor});
    }
    /// Set the foreground color.
    /// @note `Inherited` colors are converted to `Default` colors.
    /// @param color The new foreground color.
    virtual void setForeground(Foreground color) noexcept = 0;
    /// Set the background color.
    /// @note `Inherited` colors are converted to `Default` colors.
    /// @param color The new background color.
    virtual void setBackground(Background color) noexcept = 0;
    /// Set the terminal default foreground and background colors.
    void setDefaultColor() noexcept { setColor(Color::reset()); }
    /// Get the character attributes supported by this writer.
    /// @return The supported character attributes.
    [[nodiscard]] virtual auto supportedCharAttributes() const noexcept -> CharAttributes = 0;
    /// Enable or disable the bold attribute.
    /// @param enabled `true` to enable bold, `false` to disable it.
    void setBold(const bool enabled) noexcept {
        setCharAttributes(charAttributes().withFlag(CharAttributes::Bold, enabled));
    }
    /// Enable or disable the dim attribute.
    /// @param enabled `true` to enable dim, `false` to disable it.
    void setDim(const bool enabled) noexcept {
        setCharAttributes(charAttributes().withFlag(CharAttributes::Dim, enabled));
    }
    /// Enable or disable the italic attribute.
    /// @param enabled `true` to enable italic, `false` to disable it.
    void setItalic(const bool enabled) noexcept {
        setCharAttributes(charAttributes().withFlag(CharAttributes::Italic, enabled));
    }
    /// Enable or disable the underline attribute.
    /// @param enabled `true` to enable underline, `false` to disable it.
    void setUnderline(const bool enabled) noexcept {
        setCharAttributes(charAttributes().withFlag(CharAttributes::Underline, enabled));
    }
    /// Enable or disable the blink attribute.
    /// @param enabled `true` to enable blink, `false` to disable it.
    void setBlink(const bool enabled) noexcept {
        setCharAttributes(charAttributes().withFlag(CharAttributes::Blink, enabled));
    }
    /// Enable or disable the reverse attribute.
    /// @param enabled `true` to enable reverse, `false` to disable it.
    void setReverse(const bool enabled) noexcept {
        setCharAttributes(charAttributes().withFlag(CharAttributes::Reverse, enabled));
    }
    /// Enable or disable the hidden attribute.
    /// @param enabled `true` to enable hidden, `false` to disable it.
    void setHidden(const bool enabled) noexcept {
        setCharAttributes(charAttributes().withFlag(CharAttributes::Hidden, enabled));
    }
    /// Enable or disable the strikethrough attribute.
    /// @param enabled `true` to enable strikethrough, `false` to disable it.
    void setStrikethrough(const bool enabled) noexcept {
        setCharAttributes(charAttributes().withFlag(CharAttributes::Strikethrough, enabled));
    }
    /// Move the cursor to the left.
    /// If the resulting position is out of bounds, the result is undefined.
    /// @param count The number of terminal cells to move.
    virtual void moveLeft(const Coordinate count) noexcept { moveCursor(Position{-count, 0}, MoveMode::Relative); }
    /// Move the cursor to the right.
    /// If the resulting position is out of bounds, the result is undefined.
    /// @param count The number of terminal cells to move.
    virtual void moveRight(const Coordinate count) noexcept { moveCursor(Position{count, 0}, MoveMode::Relative); }
    /// Move the cursor up.
    /// If the resulting position is out of bounds, the result is undefined.
    /// @param count The number of terminal cells to move.
    virtual void moveUp(const Coordinate count) noexcept { moveCursor(Position{0, -count}, MoveMode::Relative); }
    /// Move the cursor down.
    /// If the resulting position is out of bounds, the result is undefined.
    /// @param count The number of terminal cells to move.
    virtual void moveDown(const Coordinate count) noexcept { moveCursor(Position{0, count}, MoveMode::Relative); }
    /// Move the cursor to the given position.
    /// If the resulting position is out of bounds, the result is undefined.
    /// @param pos The position to move the cursor to.
    virtual void moveTo(const Position pos) noexcept { moveCursor(pos, MoveMode::Absolute); }
    /// Moves the cursor to the home position.
    virtual void moveHome() noexcept { moveCursor(Position{0, 0}, MoveMode::Absolute); }
    /// Move the cursor absolute or relative.
    /// If the resulting position is out of bounds, the result is undefined.
    /// @param posOrDelta The absolute position or delta for the move.
    /// @param mode The move mode, either absolute or relative.
    virtual void moveCursor(Position posOrDelta, MoveMode mode) noexcept = 0;
    /// Try to get the current cursor position.
    /// Not all implementations support retrieving the cursor position.
    /// @return The current cursor position, or `std::nullopt` if it cannot be determined.
    virtual auto cursorPosition() noexcept -> std::optional<Position> { return std::nullopt; }
    /// Enabled/disable auto-wrap.
    /// Auto wrap controls if the cursor automatically wraps to the next line when reaching the right margin.
    /// This is a feature that can be enabled or disabled.
    /// Do not confuse this with line wrapping, which is a different feature.
    /// @param enabled Whether to enable or disable auto-wrap.
    virtual void setAutoWrap(bool enabled) noexcept = 0;
    /// Make the cursor visible/invisible.
    /// Not all implementations support changing the cursor visibility.
    /// @param visible Whether to make the cursor visible or invisible.
    virtual void setCursorVisible([[maybe_unused]] bool visible) noexcept {}

public: // screen handling
    /// Get the size of the screen/writing area.
    [[nodiscard]] virtual auto size() const noexcept -> Size = 0;
    /// Clears the screen/writing area.
    virtual void clearScreen() noexcept = 0;

public: // write
    /// Write a character at the current cursor position.
    /// Inherited color components resolve against the currently active color.
    /// Overwrites the character under the cursor.
    /// @param character The character to write.
    virtual void write(const Char &character) noexcept = 0;
    /// Write a string at the current cursor position.
    /// Inherited color components in each character resolve against the currently active color.
    /// Overwrites the characters under the cursor.
    /// @param str The string to write.
    virtual void write(const String &str) noexcept = 0;
    /// @overload
    void write(std::string_view text) noexcept { write(String(text)); }
    /// Write a buffer at the current cursor position.
    /// This will not perform any additional formatting, clipping, or processing.
    /// Each line of the buffer will be written, and a line-break added after each line.
    /// @param buffer The buffer to write.
    virtual void write(const ReadableBuffer &buffer) noexcept = 0;
    /// Write a line-break.
    /// This will move the cursor to the beginning of the next line.
    virtual void writeLineBreak() noexcept = 0;
    /// Print elements at the cursor position.
    /// @param args The arguments to print.
    template <PrintableArg... Args>
    void print(Args... args) noexcept {
        (printLinePart(args), ...);
    }
    /// Print elements at the cursor position and add a line break.
    /// @param args The arguments to print.
    template <PrintableArg... Args>
    void printLine(Args... args) noexcept {
        (printLinePart(args), ...);
        writeLineBreak();
    }
    /// Print a word-wrapped paragraph at the cursor position.
    /// @param paragraph The paragraph text to write. Can use line breaks and tabs, see documentation.
    /// @param options The paragraph options to use.
    /// @return The number of lines written (including empty lines).
    auto printParagraph(
        const String &paragraph, const ParagraphOptions &options = ParagraphOptions::defaultOptions()) noexcept -> int {
        return printParagraphImpl(paragraph, options);
    }
    /// @overload
    auto printParagraph(
        const std::string &paragraph, const ParagraphOptions &options = ParagraphOptions::defaultOptions()) noexcept
        -> int {
        return printParagraphImpl(String{paragraph}, options);
    }

protected:
    /// Implementation hook for paragraph printing.
    /// Derived classes render the paragraph with their own cursor and wrapping behavior.
    /// @param paragraph The paragraph text to print.
    /// @param options The paragraph layout settings to apply.
    /// @return The number of rendered lines.
    virtual auto printParagraphImpl(const String &paragraph, const ParagraphOptions &options) noexcept -> int = 0;

    /// Handle one argument passed to `print()` or `printLine()`.
    virtual void printLinePart(const Color color) noexcept { setColor(color); }
    /// Handle one argument passed to `print()` or `printLine()`.
    virtual void printLinePart(const Foreground color) noexcept { setForeground(color); }
    /// Handle one argument passed to `print()` or `printLine()`.
    virtual void printLinePart(const Foreground::Hue color) noexcept { setForeground(color); }
    /// Handle one argument passed to `print()` or `printLine()`.
    virtual void printLinePart(const Background color) noexcept { setBackground(color); }
    /// Handle one argument passed to `print()` or `printLine()`.
    virtual void printLinePart(const Background::Hue color) noexcept { setBackground(color); }
    /// Handle one argument passed to `print()` or `printLine()`.
    virtual void printLinePart(const CharStyle style) noexcept { setStyle(this->style().withOverlay(style)); }
    /// Handle one argument passed to `print()` or `printLine()`.
    virtual void printLinePart(const CharAttributes attributes) noexcept {
        setCharAttributes(attributes.resolvedWith(charAttributes()));
    }
    /// Handle one argument passed to `print()` or `printLine()`.
    virtual void printLinePart(const Char &charStr) noexcept { write(charStr); }
    /// Handle one argument passed to `print()` or `printLine()`.
    virtual void printLinePart(const String &str) noexcept { write(str); }
    /// Handle one argument passed to `print()` or `printLine()`.
    virtual void printLinePart(const std::string &text) noexcept { write(text); }
    /// Handle one argument passed to `print()` or `printLine()`.
    virtual void printLinePart(const std::string_view text) noexcept { write(text); }
    /// Handle one argument passed to `print()` or `printLine()`.
    virtual void printLinePart(const char text[]) noexcept { write(text); }
};


}
