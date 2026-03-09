// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Char.hpp"
#include "Size.hpp"

#include <utility>


namespace erbsland::cterm {


/// Settings controlling how `Terminal::updateScreen()` renders a buffer.
class UpdateSettings final {
public:
    /// Create default screen update settings.
    UpdateSettings() = default;
    /// Create screen update settings with explicit values.
    /// @param minimumSize The minimum terminal size required for rendering the buffer.
    /// @param minimumSizeMark The marker rendered when the terminal is smaller than `minimumSize`.
    /// @param showCropMarks `true` to render crop marks when the buffer does not fully fit on screen.
    /// @param cropMarkRight The mark rendered in the last visible column when content is cropped on the right.
    /// @param cropMarkBottom The mark rendered in the last visible row when content is cropped at the bottom.
    UpdateSettings(
        Size minimumSize, Char minimumSizeMark, bool showCropMarks, Char cropMarkRight, Char cropMarkBottom) noexcept :
        _minimumSize{minimumSize},
        _minimumSizeMark{std::move(minimumSizeMark)},
        _showCropMarks{showCropMarks},
        _cropMarkRight{std::move(cropMarkRight)},
        _cropMarkBottom{std::move(cropMarkBottom)} {}

public: // accessors
    /// Get the minimum terminal size required for rendering the buffer.
    /// @return The minimum supported terminal size.
    [[nodiscard]] auto minimumSize() const noexcept -> Size { return _minimumSize; }
    /// Get the marker rendered when the terminal is too small.
    /// @return The minimum-size marker.
    [[nodiscard]] auto minimumSizeMark() const noexcept -> const Char & { return _minimumSizeMark; }
    /// Check if crop marks are enabled.
    /// @return `true` if crop marks are rendered for truncated content.
    [[nodiscard]] auto showCropMarks() const noexcept -> bool { return _showCropMarks; }
    /// Get the mark rendered when content is cropped on the right.
    /// @return The right crop mark.
    [[nodiscard]] auto cropMarkRight() const noexcept -> const Char & { return _cropMarkRight; }
    /// Get the mark rendered when content is cropped at the bottom.
    /// @return The bottom crop mark.
    [[nodiscard]] auto cropMarkBottom() const noexcept -> const Char & { return _cropMarkBottom; }

public: // modifiers
    /// Set the minimum terminal size required for rendering the buffer.
    /// @param minimumSize The minimum supported terminal size.
    void setMinimumSize(const Size minimumSize) noexcept { _minimumSize = minimumSize; }
    /// Set the marker rendered when the terminal is too small.
    /// @param minimumSizeMark The minimum-size marker.
    void setMinimumSizeMark(Char minimumSizeMark) noexcept { _minimumSizeMark = std::move(minimumSizeMark); }
    /// Enable or disable crop marks.
    /// @param showCropMarks `true` to render crop marks for truncated content.
    void setShowCropMarks(const bool showCropMarks) noexcept { _showCropMarks = showCropMarks; }
    /// Set the mark rendered when content is cropped on the right.
    /// @param cropMarkRight The right crop mark.
    void setCropMarkRight(Char cropMarkRight) noexcept { _cropMarkRight = std::move(cropMarkRight); }
    /// Set the mark rendered when content is cropped at the bottom.
    /// @param cropMarkBottom The bottom crop mark.
    void setCropMarkBottom(Char cropMarkBottom) noexcept { _cropMarkBottom = std::move(cropMarkBottom); }

private:
    Size _minimumSize{};
    Char _minimumSizeMark{"X", {fg::BrightRed, bg::Black}};
    bool _showCropMarks{false};
    Char _cropMarkRight{"▶︎", {fg::BrightYellow, bg::Black}};
    Char _cropMarkBottom{"▼", {fg::BrightYellow, bg::Black}};
};


}
