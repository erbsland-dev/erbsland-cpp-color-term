// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "UpdateSettings.hpp"

#include "BufferView.hpp"

namespace erbsland::cterm {

UpdateSettings::UpdateSettings(
    const Size minimumSize,
    const Char minimumSizeBackground,
    const bool showCropMarks,
    const Char cropMarkRight,
    const Char cropMarkBottom) noexcept :
    _minimumSize{minimumSize},
    _minimumSizeBackground{minimumSizeBackground},
    _showCropMarks{showCropMarks},
    _cropMarkRight{cropMarkRight},
    _cropMarkBottom{cropMarkBottom} {
}

auto UpdateSettings::minimumSize() const noexcept -> Size {
    return _minimumSize;
}

auto UpdateSettings::minimumSizeMark() const noexcept -> const Char & {
    return _minimumSizeBackground;
}

auto UpdateSettings::showCropMarks() const noexcept -> bool {
    return _showCropMarks;
}

auto UpdateSettings::cropMarkRight() const noexcept -> const Char & {
    return _cropMarkRight;
}

auto UpdateSettings::cropMarkBottomRight() const noexcept -> const Char & {
    return _cropMarkBottomRight;
}

auto UpdateSettings::cropMarkBottom() const noexcept -> const Char & {
    return _cropMarkBottom;
}

void UpdateSettings::setMinimumSize(const Size minimumSize) noexcept {
    _minimumSize = minimumSize;
}

auto UpdateSettings::minimumSizeBackground() const noexcept -> const Char & {
    return _minimumSizeBackground;
}

void UpdateSettings::setMinimumSizeBackground(const Char character) noexcept {
    _minimumSizeBackground = character;
}

auto UpdateSettings::minimumSizeMessage() const noexcept -> const String & {
    return _minimumSizeMessage;
}

void UpdateSettings::setMinimumSizeMessage(String message) noexcept {
    _minimumSizeMessage = std::move(message);
}

void UpdateSettings::setMinimumSizeMark(const Char minimumSizeMark) noexcept {
    _minimumSizeBackground = minimumSizeMark;
}

void UpdateSettings::setShowCropMarks(const bool showCropMarks) noexcept {
    _showCropMarks = showCropMarks;
}

void UpdateSettings::setCropMarkRight(const Char cropMarkRight) noexcept {
    _cropMarkRight = cropMarkRight;
}

void UpdateSettings::setCropMarkBottomRight(Char cropMarkBottomRight) noexcept {
    _cropMarkBottomRight = cropMarkBottomRight;
}

void UpdateSettings::setCropMarkBottom(const Char cropMarkBottom) noexcept {
    _cropMarkBottom = cropMarkBottom;
}

auto UpdateSettings::switchToAlternateBuffer() const noexcept -> bool {
    return _switchToAlternateBuffer;
}

void UpdateSettings::setSwitchToAlternateBuffer(const bool switchToAlternateBuffer) noexcept {
    _switchToAlternateBuffer = switchToAlternateBuffer;
}

void UpdateSettings::applyTo(BufferViewBase &view) const noexcept {
    view.setShowCropCharacters(_showCropMarks);
    view.setCropCharacter(Direction::South, _cropMarkBottom);
    view.setCropCharacter(Direction::SouthEast, _cropMarkBottomRight);
    view.setCropCharacter(Direction::East, _cropMarkRight);
}

auto UpdateSettings::defaultSettings() noexcept -> const UpdateSettings & {
    static UpdateSettings settings;
    return settings;
}

}
