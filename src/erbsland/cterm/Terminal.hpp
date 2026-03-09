// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Buffer.hpp"
#include "Char.hpp"
#include "Color.hpp"
#include "Input.hpp"
#include "Size.hpp"
#include "String.hpp"
#include "UpdateSettings.hpp"

#include "impl/TypeTraits.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>


namespace erbsland::cterm {


class Terminal;
/// Shared pointer to a terminal instance.
using TerminalPtr = std::shared_ptr<Terminal>;


/// High-level terminal interface for screen control, color output, and key input.
class Terminal {
public: // public types
    /// Callback invoked when the terminal size changes or is initialized.
    using ScreenSizeChangedCallback = std::function<void(Size)>;

public: // public types
    /// Screen clearing strategy used between rendered frames.
    enum class RefreshMode : uint8_t {
        /// Do not emit cursor or clear-screen control sequences automatically.
        Keep,
        /// Clear the full screen before rendering the next frame.
        Clear,
        /// Move the cursor to the top-left corner before rendering the next frame.
        Overwrite,
    };

public: // ctors/dtor/assign/move
    /// Create a new terminal instance.
    /// @param size The fallback terminal size used when automatic detection is unavailable.
    explicit Terminal(Size size = {80, 25});

public: // accessors
    /// Get the currently configured or detected terminal size.
    [[nodiscard]] auto size() const noexcept -> Size { return _size; }
    /// Access the input interface.
    /// @return The platform-specific input backend owned by this terminal.
    [[nodiscard]] auto input() noexcept -> Input & { return *_input; }
    /// Get the refresh mode.
    [[nodiscard]] auto refreshMode() const noexcept -> RefreshMode { return _refreshMode; }
    /// Test if colors are enabled.
    [[nodiscard]] auto colorEnabled() const noexcept -> bool { return _colorEnabled; }
    /// Check whether dynamic terminal size detection is enabled.
    [[nodiscard]] auto sizeDetectionEnabled() const noexcept -> bool { return _sizeDetectionEnabled; }
    /// Check whether line buffering is enabled for incremental writes.
    /// @return `true` if text output is collected until a newline or `flush()`.
    [[nodiscard]] auto lineBufferEnabled() const noexcept -> bool { return _lineBufferEnabled; }
    /// Check whether the optional back buffer is enabled for smart overwrite updates.
    /// @return `true` if `updateScreen()` keeps the previous rendered frame for diff-based updates.
    [[nodiscard]] auto backBufferEnabled() const noexcept -> bool { return _backBuffer.has_value(); }

public: // modifiers
    /// Modify the size of the terminal.
    /// If the terminal is not yet initialized, the size will be set when `initializeScreen()` is called.
    /// This method is only used when the terminal size is manually set.
    /// If size detection is enabled, the terminal size will be automatically detected and updated.
    /// @param size The new terminal size.
    void setSize(Size size) noexcept;
    /// Set if dynamic terminal size detection is enabled.
    /// @param enabled `true` to enable automatic size detection.
    void setSizeDetectionEnabled(const bool enabled) noexcept { _sizeDetectionEnabled = enabled; }
    /// Enable or disable line buffering for incremental writes.
    /// When enabled, output is accumulated until a newline or `flush()` is reached.
    /// @param enabled `true` to enable buffered writes.
    void setLineBufferEnabled(bool enabled) noexcept;
    /// Enable or disable the optional back buffer used by smart overwrite updates.
    /// Enabling the back buffer forces the next `updateScreen()` call to redraw the full frame once.
    /// @param enabled `true` to enable the back buffer.
    void setBackBufferEnabled(bool enabled) noexcept;
    /// Set a callback called when a detected terminal size is submitted.
    /// @param callback The callback invoked from `initializeScreen()` and `testScreenSize()`.
    void setScreenSizeChangedCallback(ScreenSizeChangedCallback callback) noexcept {
        _screenSizeChangedCallback = std::move(callback);
    }
    /// Set if colors are enabled.
    /// @param enabled `true` to emit ANSI color and cursor control sequences.
    void setColorEnabled(const bool enabled) noexcept { _colorEnabled = enabled; }
    /// Set the refresh mode.
    /// @param mode The screen refresh strategy to use.
    void setRefreshMode(const RefreshMode mode) noexcept { _refreshMode = mode; }

public: // tools
    /// Initialize the console once before the application starts.
    /// Applies platform-specific setup, optionally clears the screen, and submits the initial size.
    void initializeScreen() noexcept;
    /// Detect terminal resize changes and submit them through the callback.
    void testScreenSize() noexcept;
    /// Clear the screen before a new one is displayed.
    /// The emitted control sequence depends on the configured refresh mode.
    void clearScreen() noexcept;
    /// Restore terminal settings when the application is quit.
    void restoreScreen() noexcept;

public: // cursor state
    /// Get the current color.
    /// @return The currently tracked terminal color state.
    [[nodiscard]] auto color() const noexcept -> Color;
    /// Set foreground and background color.
    /// @note `Inherited` colors are converted to `Default` colors.
    /// @param color The new combined terminal color.
    void setColor(Color color) noexcept;
    /// Set foreground and background color.
    /// @note `Inherited` colors are converted to `Default` colors.
    /// @param foregroundColor The new foreground color.
    /// @param backgroundColor The new background color.
    void setColor(Foreground foregroundColor, Background backgroundColor) noexcept;
    /// Set the foreground color.
    /// @note `Inherited` colors are converted to `Default` colors.
    /// @param color The new foreground color.
    void setForeground(Foreground color) noexcept;
    /// Set the background color.
    /// @note `Inherited` colors are converted to `Default` colors.
    /// @param color The new background color.
    void setBackground(Background color) noexcept;
    /// Set the terminal default foreground and background colors.
    void setDefaultColor() noexcept;
    /// Move the cursor to the left.
    /// @param count The number of terminal cells to move.
    void moveLeft(int count);
    /// Flush the buffer.
    void flush() noexcept;
    /// Render a buffer onto the terminal.
    /// The buffer is clipped to the visible area of the terminal and optionally annotated with crop marks.
    /// If the terminal is smaller than the configured minimum size, only the minimum-size marker is rendered.
    /// @param buffer The buffer to render.
    /// @param settings Additional rendering settings for crop marks and minimum terminal size handling.
    void updateScreen(const Buffer &buffer, const UpdateSettings &settings = {}) noexcept;

public: // writing tools
    /// Write a character to the console.
    /// @param character The character to write.
    void write(const Char &character) noexcept;
    /// Write a string to the console.
    /// @param str The string to write.
    void write(const String &str) noexcept;
    /// Write text to the console.
    /// Please use `print` and `printLine` for the high-level API.
    /// @param text The text to display.
    void write(std::string_view text) noexcept;
    /// Write a buffer to the console.
    /// This will not perform any additional formatting, clipping, or processing.
    /// Each line of the buffer will be written to the console.
    /// @note For interactive apps, use `updateScreen`.
    /// @param buffer The buffer to write.
    void write(const Buffer &buffer) noexcept;
    /// Add a line-break.
    void lineBreak() noexcept;
    /// Print elements on the screen.
    /// @param args The arguments to display.
    template <PrintableArg... Args>
    void print(Args... args) noexcept {
        (printLinePart(args), ...);
    }
    /// Print elements on the screen adding a line break.
    /// @param args The arguments to display.
    template <PrintableArg... Args>
    void printLine(Args... args) noexcept {
        (printLinePart(args), ...);
        lineBreak();
    }

private:
    static void appendClearSequence(std::string &output) noexcept;
    static void appendResizeClearSequence(std::string &output) noexcept;
    static void appendCursorHomeSequence(std::string &output) noexcept;
    static void appendCursorMoveSequence(std::string &output, Position pos) noexcept;
    void appendDefaultColorSequence(std::string &output, Color &currentColor) const noexcept;
    void appendCharacterOutput(std::string &output, const Char &character, Color &currentColor) const noexcept;
    void appendFullFrameOutput(
        std::string &output,
        const Buffer &buffer,
        Size terminalSize,
        const UpdateSettings &settings) const noexcept;
    void appendDiffFrameOutput(
        std::string &output,
        const Buffer &previousFrame,
        const Buffer &buffer,
        Size terminalSize,
        const UpdateSettings &settings) const noexcept;
    void appendRefreshSequence(std::string &output, bool forceFullClear) const noexcept;
    void emitBufferedOutput(std::string_view text) noexcept;
    void emitDirectOutput(std::string_view text, bool flushConsole = false) noexcept;
    void flushLineBuffer() noexcept;
    void invalidateBackBuffer() noexcept;
    [[nodiscard]] static auto isMinimumSizeOnly(Size terminalSize, const UpdateSettings &settings) noexcept -> bool;
    [[nodiscard]] static auto renderedFrameSize(
        const Buffer &buffer,
        Size terminalSize,
        const UpdateSettings &settings) noexcept -> Size;
    [[nodiscard]] static auto
    getCropped(const Buffer &buffer, Size terminalSize, const UpdateSettings &settings, Position pos) -> const Char &;
    void updateBackBuffer(const Buffer &buffer, Size terminalSize, const UpdateSettings &settings) noexcept;
    [[nodiscard]] auto changedCharacterCount(
        const Buffer &previousFrame,
        const Buffer &buffer,
        Size terminalSize,
        const UpdateSettings &settings) const noexcept -> int;
    void printLinePart(const Color color) noexcept { setColor(color); }
    void printLinePart(const Foreground color) noexcept { setForeground(color); }
    void printLinePart(const Foreground::Hue color) noexcept { setForeground(color); }
    void printLinePart(const Background color) noexcept { setBackground(color); }
    void printLinePart(const Background::Hue color) noexcept { setBackground(color); }
    void printLinePart(const Char &charStr) noexcept { write(charStr); }
    void printLinePart(const String &str) noexcept { write(str); }
    void printLinePart(const std::string &text) noexcept { write(text); }
    void printLinePart(const std::string_view text) noexcept { write(text); }
    void printLinePart(const char text[]) noexcept { write(text); }
    void initializePlatform() noexcept;
    void restorePlatform() noexcept;
    [[nodiscard]] auto detectScreenSize() const noexcept -> std::optional<Size>;
    void submitScreenSize(Size size, bool fromTestScreenSize) noexcept;

private:
    bool _colorEnabled{true};
    bool _sizeDetectionEnabled{true};
    bool _lineBufferEnabled{true};
    bool _clearScreenAfterResize{false};
    RefreshMode _refreshMode{RefreshMode::Overwrite};
    Size _size;
    Color _color{Foreground::Default, Background::Default};
    std::string _lineBuffer;
    std::optional<Buffer> _backBuffer;
    ScreenSizeChangedCallback _screenSizeChangedCallback;
    std::unique_ptr<Input> _input;
    // states
    [[maybe_unused]] bool _cursorStateSaved{false};
    [[maybe_unused]] bool _cursorVisible{true};
};


}
