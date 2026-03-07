// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Key.hpp"

#include <cstdint>
#include <string>
#include <vector>


namespace erbsland::cterm {


/// Definition of a single key mapping for a specific input mode.
class InputDefinition {
public:
    /// The input modes to which a key definition can apply.
    enum class ForMode : uint8_t {
        /// The key can be used in both input modes.
        Both,
        /// The key only applies when reading a full line.
        ReadLine,
        /// The key only applies when reading single key presses.
        Key,
    };

public: // ctors/dtor/assign/move
    /// Create an invalid input definition.
    InputDefinition() = default;
    /// Create a definition for the given key and input mode.
    /// @param keyPress The key to match.
    /// @param forMode The input mode in which the definition is valid.
    InputDefinition(Key keyPress, ForMode forMode) noexcept;

public: // accessors
    /// Return the represented key press.
    [[nodiscard]] auto keyPress() const noexcept -> const Key & { return _keyPress; }
    /// Return the mode for which this definition is valid.
    [[nodiscard]] auto forMode() const noexcept -> ForMode { return _forMode; }

public: // tests
    /// Check whether this definition contains a valid key press.
    [[nodiscard]] auto valid() const noexcept -> bool { return _keyPress.valid(); }

public: // conversion
    /// Parse the textual representation of a key definition.
    /// @param text The configuration text, optionally prefixed with `>` or `+` for the input mode.
    /// @return The parsed input definition.
    [[nodiscard]] static auto fromString(std::string text) noexcept -> InputDefinition;
    /// Convert the key definition to its configuration text.
    /// The serialized configuration text includes the optional mode prefix.
    /// @return The serialized configuration text.
    [[nodiscard]] auto toString() const -> std::string;

public: // tools
    /// Return a display label for prompts and help texts.
    /// @param useBrackets If `true`, wrap the text in `[` and `]`.
    /// @return A human-readable key label.
    [[nodiscard]] auto toDisplayText(bool useBrackets = true) const -> std::string;
    /// Compatibility wrapper for older code that used `displayText()`.
    /// @return A human-readable key label with stylized brackets.
    [[nodiscard]] auto displayText() const -> std::string { return toDisplayText(); }

private:
    Key _keyPress;
    ForMode _forMode{ForMode::Both};
};


/// A list of input definitions.
using InputDefinitionList = std::vector<InputDefinition>;


}
