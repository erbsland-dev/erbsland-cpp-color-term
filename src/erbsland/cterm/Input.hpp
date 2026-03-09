// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Key.hpp"

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>


namespace erbsland::cterm {


/// Reads key presses from the console depending on the configured mode.
class Input {
public:
    /// Supported reading modes for the input backend.
    enum class Mode : uint8_t {
        /// Read one full line from standard input.
        ReadLine,
        /// Read raw key presses from the terminal backend.
        Key,
    };

public: // ctors/dtor/assign/move
    /// Create an input object in `Mode::ReadLine`.
    Input() = default;
    /// Destroy the input object.
    virtual ~Input() = default;

    Input(const Input &) = delete;
    auto operator=(const Input &) -> Input & = delete;
    Input(Input &&) = delete;
    auto operator=(Input &&) -> Input & = delete;

public: // modifiers
    /// Set the current reading mode.
    /// @param mode The new input mode.
    void setMode(Mode mode);

public: // accessors
    /// Get the current reading mode.
    [[nodiscard]] auto mode() const noexcept -> Mode;

public: // tools
    /// Read one key event.
    /// @param timeout Maximum wait time in `Mode::Key`; ignored in `Mode::ReadLine`.
    /// @return The parsed key event, or an invalid key if no supported input was read.
    [[nodiscard]] auto read(std::chrono::milliseconds timeout = {}) const -> Key;

protected:
    virtual void handleModeChange(Mode previousMode, Mode newMode) noexcept;
    [[nodiscard]] static auto readLine() -> std::string;
    [[nodiscard]] virtual auto readKey(std::chrono::milliseconds timeout) const -> Key = 0;

private:
    Mode _mode{Mode::ReadLine};
};


}
