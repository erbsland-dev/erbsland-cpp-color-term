// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Backend.hpp"
#include "Char.hpp"
#include "Color.hpp"
#include "MoveMode.hpp"
#include "Size.hpp"
#include "String.hpp"
#include "TerminalFlags.hpp"
#include "UpdateSettings.hpp"
#include "WritableBuffer.hpp"

#include "impl/InputBackend.hpp"
#include "impl/LineBuffer.hpp"
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
    /// The minimum supported size of a terminal.
    constexpr static auto cMinimumSize = Size{1, 1};
    /// The maximum supported size of a terminal.
    constexpr static auto cMaximumSize = Size{2048, 2048};

public:
    /// Callback invoked when the drawable terminal size changes or is initialized.
    using ScreenSizeChangedCallback = std::function<void(Size)>;

    /// Screen clearing strategy used between rendered frames.
    enum class RefreshMode : uint8_t {
        /// Do not emit cursor or clear-screen control sequences automatically.
        Keep,
        /// Clear the full screen before rendering the next frame.
        Clear,
        /// Move the cursor to the top-left corner before rendering the next frame.
        Overwrite,
    };

    /// The output mode for the terminal.
    enum class OutputMode : uint8_t {
        /// Only output plain text with no colors and no cursor control
        Text,
        /// Full control over the terminal, including colors and cursor control.
        FullControl,
    };

public:
    /// Create a new terminal instance with default values.
    explicit Terminal();
    /// Create a new terminal instance.
    /// @param flags The terminal flags to use.
    explicit Terminal(TerminalFlags flags = {});
    /// Create a new terminal instance.
    /// The size is automatically bounded to the minimum and maximum supported sizes.
    /// @param size The fallback terminal size used when automatic detection is unavailable.
    /// @param flags The terminal flags to use.
    explicit Terminal(Size size = {80, 25}, TerminalFlags flags = {});
    /// Create a new terminal instance with a custom backend.
    /// The size is automatically bounded to the minimum and maximum supported sizes.
    /// @param backend The backend to use for the terminal.
    /// @param size The fallback terminal size used when automatic detection is unavailable.
    explicit Terminal(BackendPtr backend, Size size = {80, 25});

public: // settings
    /// Get the currently configured or detected drawable terminal size.
    [[nodiscard]] auto size() const noexcept -> Size { return _size; }
    /// Modify the size of the terminal.
    /// The size is automatically bounded to the minimum and maximum supported sizes.
    /// If size detection is enabled, the terminal size will be automatically detected and updated.
    /// @param size The new terminal size.
    void setSize(Size size) noexcept;
    /// Get the refresh mode.
    [[nodiscard]] auto refreshMode() const noexcept -> RefreshMode { return _refreshMode; }
    /// Set the refresh mode.
    /// @param mode The screen refresh strategy to use.
    void setRefreshMode(const RefreshMode mode) noexcept { _refreshMode = mode; }
    /// Get the current output mode for the terminal.
    [[nodiscard]] auto outputMode() const noexcept -> OutputMode { return _outputMode; }
    /// Set the output mode.
    /// Switching to `OutputMode::Text` disables size detection, refresh modes, and back-buffer updates.
    /// @param outputMode The output mode to set.
    void setOutputMode(OutputMode outputMode) noexcept;
    /// Check whether dynamic terminal size detection is enabled.
    [[nodiscard]] auto sizeDetectionEnabled() const noexcept -> bool;
    /// Set if dynamic terminal size detection is enabled.
    /// Can only be enabled while the output mode is `OutputMode::FullControl`.
    /// @param enabled `true` to enable automatic size detection.
    void setSizeDetectionEnabled(bool enabled) noexcept;
    /// Check whether line buffering is enabled for incremental writes.
    /// @return `true` if text output is collected until a newline or `flush()`.
    [[nodiscard]] auto lineBufferEnabled() const noexcept -> bool;
    /// Enable or disable line buffering for incremental writes.
    /// When enabled, output is accumulated until a newline or `flush()` is reached.
    /// Line buffering can only be enabled if the backend supports both color and cursor ANSI codes.
    /// @param enabled `true` to enable buffered writes.
    void setLineBufferEnabled(bool enabled) noexcept;
    /// Check whether the compatibility safe margin is enabled.
    /// @return `true` if one column and one row are reserved from the detected terminal size.
    [[nodiscard]] auto safeMarginEnabled() const noexcept -> bool;
    /// Enable or disable the compatibility safe margin.
    /// When enabled, the reported drawable size is reduced by one column and one row.
    /// Disable this only when the terminal should use its full detected size and newline-free screen updates.
    /// @param enabled `true` to reserve one column and one row from the terminal size.
    void setSafeMarginEnabled(bool enabled) noexcept;
    /// Check whether the optional back buffer is enabled for smart overwrite updates.
    /// @return `true` if `updateScreen()` keeps the previous rendered frame for diff-based updates.
    [[nodiscard]] auto backBufferEnabled() const noexcept -> bool;
    /// Enable or disable the optional back buffer used by smart overwrite updates.
    /// Enabling the back buffer forces the next `updateScreen()` call to redraw the full frame once.
    /// Can only be enabled while the output mode is `OutputMode::FullControl`.
    /// @param enabled `true` to enable the back buffer.
    void setBackBufferEnabled(bool enabled) noexcept;
    /// Set a custom backend for the terminal.
    /// @param backend The backend to use for the terminal. If `nullptr` is passed, the default backend is restored.
    void setBackend(BackendPtr backend) noexcept;

public: // input handling.
    /// Access the input interface.
    /// @return The platform-specific input backend owned by this terminal.
    [[nodiscard]] auto input() noexcept -> Input &;

public: // initialization
    /// Initialize the console once before the application starts.
    /// Applies platform-specific setup, optionally clears the screen, and tests for the initial screen size.
    /// Also hides the cursor by default, as it is usually only made visible when the user makes input.
    /// Call this at the start of your application.
    void initializeScreen() noexcept;
    /// Detect terminal resize changes.
    /// After calling this method, `size()` returns a safe size for the terminal.
    void testScreenSize() noexcept;
    /// Restore terminal settings when the application is quit.
    /// Call this at the end of your application.
    /// This should restore the terminal to its original state, including cursor visibility and any other
    /// settings that were modified during initialization.
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
    /// Move the cursor to the right.
    /// @param count The number of terminal cells to move.
    void moveRight(int count);
    /// Move the cursor up.
    /// @param count The number of terminal cells to move.
    void moveUp(int count);
    /// Move the cursor down.
    /// @param count The number of terminal cells to move.
    void moveDown(int count);
    /// Move the cursor to the given position.
    /// If you need the cursor moved immediately, call `flush()` after this method.
    void moveTo(Position pos) noexcept;
    /// Moves the cursor to the home position.
    /// If you need the cursor moved immediately, call `flush()` after this method.
    void moveHome() noexcept;
    /// Move the cursor absolute or relative.
    /// @param posOrDelta The absolute position or delta for the move.
    /// @param mode The move mode, either absolute or relative.
    void moveCursor(Position posOrDelta, MoveMode mode) noexcept;
    /// Enabled/disable auto-wrap.
    void setAutoWrap(bool enabled) noexcept;
    /// Make the cursor visible/invisible.
    void setCursorVisible(bool visible) noexcept;

public: // screen handling
    /// Clears the screen.
    /// In `OutputMode::Text`, this method has no effect.
    /// If you need the screen cleared immediately, call `flush()` after this method.
    void clearScreen() noexcept;
    /// Test if the alternate screen is active.
    /// This is no terminal detection, it just returns the internal state.
    [[nodiscard]] auto isAlternateScreenActive() const noexcept -> bool;
    /// Activate or deactivate the alternate screen.
    /// If activated or deactivated, the buffer is immediately flushed to the terminal.
    void setAlternateScreen(bool enabled) noexcept;
    /// Render a buffer onto the terminal.
    /// The buffer is clipped to the drawable area reported by `size()` and optionally annotated with crop marks.
    /// If the terminal is smaller than the configured minimum size, only the minimum-size marker is rendered.
    /// When `switchToAlternateBuffer` is `true` and the alternate screen is not active, this call first
    /// switches to the alternate screen and then renders the buffer.
    /// @param buffer The buffer to render.
    /// @param settings Additional rendering settings for crop marks and minimum terminal size handling.
    void updateScreen(const ReadableBuffer &buffer, const UpdateSettings &settings = {}) noexcept;
    /// Flush the all buffer immediately to the terminal.
    void flush() noexcept;

public: // writing tools
    /// Write a character to the console.
    /// Inherited color components resolve against the currently active terminal color.
    /// @param character The character to write.
    void write(const Char &character) noexcept;
    /// Write a string to the console.
    /// Inherited color components in each character resolve against the currently active terminal color.
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
    void write(const ReadableBuffer &buffer) noexcept;
    /// Add a line-break.
    void writeLineBreak() noexcept;
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
        writeLineBreak();
    }

public: // backward compatibility.
    [[deprecated("use writeLineBreak()")]]
    void lineBreak() noexcept {
        writeLineBreak();
    }
    [[deprecated("use outputMode()")]] [[nodiscard]] auto colorEnabled() const noexcept -> bool {
        return _outputMode != OutputMode::Text;
    }
    [[deprecated("use setOutputMode()")]]
    void setColorEnabled(bool enabled) noexcept;

private:
    /// Either clears the screen or moves the cursor to the home position.
    /// Depends on the refresh mode set.
    void refreshScreen() noexcept;
    /// Update the screen using a back buffer.
    void updateScreenWithBackBuffer(const ReadableBuffer &buffer, const UpdateSettings &settings);
    /// Update and create a new back buffer.
    void updateScreenAndCreateNewBackBuffer(const ReadableBuffer &buffer, const UpdateSettings &settings);
    /// Update and resize the back buffer.
    void updateScreenAndResizeBackBuffer(const ReadableBuffer &buffer, const UpdateSettings &settings);
    /// Partially update the screen using the difference to the back buffer.
    void updateScreenPartialWithBackBuffer(const ReadableBuffer &view);
    /// Update the screen without a back buffer.
    void updateScreenWithoutBackBuffer(const ReadableBuffer &buffer, const UpdateSettings &settings);
    /// Update the minimum size buffer.
    void updateSizeTooSmallBuffer(const UpdateSettings &settings) noexcept;
    /// The implementation of the write function.
    /// @param buffer The buffer to write to the terminal.
    /// @param withRowMove Whether to move the cursor to the next row after writing (`true`)
    ///                    or use newline for the next row.
    void writeImpl(const ReadableBuffer &buffer, bool withRowMove) noexcept;
    /// Get the safety margins applied to the terminal size.
    [[nodiscard]] auto applySafeMargin(Size terminalSize) const noexcept -> Size;

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

private:
    TerminalFlags _flags;                             ///< Flags for the terminal behaviour.
    BackendPtr _backend;                              ///< The backend that is used by the terminal.
    OutputMode _outputMode{OutputMode::FullControl};  ///< The current output mode.
    bool _sizeDetectionEnabled{true};                 ///< If size detection is enabled.
    bool _safeMarginEnabled{true};                    ///< If the compatibility safe margin is enabled.
    bool _afterResize{false};                         ///< If the screen should be cleared after a resize.
    RefreshMode _refreshMode{RefreshMode::Overwrite}; ///< The refresh mode to use.
    Size _size;                                       ///< The configured size of the terminal that is safe to use.
    Size _terminalSize; ///< The actual, reported size of the terminal. Zero if we have no detected size.
    Color _color{Foreground::Default, Background::Default}; ///< The current terminal colors.
    bool _backBufferEnabled{false};                         ///< If the back buffer feature is enabled.
    bool _isAlternateScreenActive{false};                   ///< If the alternate screen is active or not.
    WritableBufferPtr _sizeTooSmallBuffer;                  ///< Buffer for size too small message.
    WritableBufferPtr _backBuffer;                          ///< The back buffer.
    impl::InputBackend _input;                              ///< The input backend.
    impl::LineBuffer _lineBuffer;                           ///< The line buffer.
};


}
