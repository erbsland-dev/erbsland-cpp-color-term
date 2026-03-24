// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Key.hpp"

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>


namespace erbsland::cterm {


class Input;
using InputPtr = std::shared_ptr<Input>;


/// The input interface.
class Input {
public:
    /// Supported reading modes for the input backend.
    enum class Mode : uint8_t {
        /// Read one full line from standard input.
        ReadLine,
        /// Read raw key presses from the terminal backend.
        Key,
    };

public:
    /// Destroy the input object.
    virtual ~Input() = default;

public:
    /// Get the current reading mode.
    [[nodiscard]] virtual auto mode() const noexcept -> Mode = 0;
    /// Set the current reading mode.
    /// @param mode The new input mode.
    virtual void setMode(Mode mode) = 0;
    /// Read one key event.
    /// @param timeout Maximum wait time in `Mode::Key`; ignored in `Mode::ReadLine`.
    /// @return The parsed key event, or an invalid key if no supported input was read.
    [[nodiscard]] auto read(std::chrono::milliseconds timeout = {}) const -> Key { return readImpl(timeout); }
    /// Read a line of text from the terminal.
    [[nodiscard]] virtual auto readLine() -> std::string = 0;

protected:
    /// Implement `read()`.
    /// The public `read()` wrapper forwards to this method.
    /// @param timeout Maximum wait time in `Mode::Key`; ignored in `Mode::ReadLine`.
    /// @return The parsed key event, or an invalid key if no supported input was read.
    [[nodiscard]] virtual auto readImpl(std::chrono::milliseconds timeout) const -> Key = 0;
};


}
