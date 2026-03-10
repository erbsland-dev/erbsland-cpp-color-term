// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "BitmapDrawOptions.hpp"


#include <stdexcept>


namespace erbsland::cterm {


BitmapDrawOptions::BitmapDrawOptions(ColorSequence colorSequence, BitmapColorMode colorMode) :
    _color{colorSequence}, _colorMode{colorMode} {
}

auto BitmapDrawOptions::color() const noexcept -> const ColorSequence & {
    return _color;
}

auto BitmapDrawOptions::colorMode() const noexcept -> BitmapColorMode {
    return _colorMode;
}

void BitmapDrawOptions::setColorMode(BitmapColorMode colorMode) noexcept {
    _colorMode = colorMode;
}

auto BitmapDrawOptions::colorAnimationOffset() const noexcept -> std::size_t {
    return _colorAnimationOffset;
}

auto BitmapDrawOptions::combinationStyle() const noexcept -> const CharCombinationStylePtr & {
    return _combinationStyle;
}

void BitmapDrawOptions::setColor(Color color) noexcept {
    _color = {color};
}

void BitmapDrawOptions::setColor(Foreground foreground) noexcept {
    _color = {Color{foreground, bg::Inherited}};
}

void BitmapDrawOptions::setColor(Foreground::Hue foreground) noexcept {
    _color = {Color{foreground, bg::Inherited}};
}

void BitmapDrawOptions::setColor(Foreground foreground, Background background) noexcept {
    _color = {Color{foreground, background}};
}

void BitmapDrawOptions::setColor(Background background) noexcept {
    _color = {Color{fg::Inherited, background}};
}

void BitmapDrawOptions::setColor(Background::Hue background) noexcept {
    _color = {Color{fg::Inherited, background}};
}

void BitmapDrawOptions::setColorSequence(ColorSequence colorSequence, BitmapColorMode colorMode) noexcept {
    _color = colorSequence;
    _colorMode = colorMode;
}

void BitmapDrawOptions::setColorAnimationOffset(std::size_t offset) noexcept {
    _colorAnimationOffset = offset;
}

auto BitmapDrawOptions::char16Style() const noexcept -> const Char16StylePtr & {
    return _char16Style;
}

void BitmapDrawOptions::setChar16Style(Char16StylePtr char16Style) noexcept {
    _char16Style = char16Style;
}

void BitmapDrawOptions::setCombinationStyle(CharCombinationStylePtr combinationStyle) noexcept {
    _combinationStyle = combinationStyle;
}

auto BitmapDrawOptions::fullBlock() const noexcept -> const Char & {
    return _fullBlock;
}

void BitmapDrawOptions::setFullBlock(Char fullBlock) {
    if (fullBlock.displayWidth() != 1) {
        throw std::invalid_argument("Full block character must have display width of 1");
    }
    _fullBlock = fullBlock;
}

auto BitmapDrawOptions::doubleBlocks() const noexcept -> const String & {
    return _doubleBlocks;
}

void BitmapDrawOptions::setDoubleBlocks(String doubleBlocks) {
    if (doubleBlocks.size() != 2) {
        throw std::invalid_argument("Double blocks string must contain exactly 2 characters");
    }
    for (const auto &character : doubleBlocks) {
        if (character.displayWidth() != 1) {
            throw std::invalid_argument("Double blocks string must contain characters with display width of 1");
        }
    }
    _doubleBlocks = std::move(doubleBlocks);
}

auto BitmapDrawOptions::halfBlocks() const noexcept -> const String & {
    return _halfBlocks;
}

void BitmapDrawOptions::setHalfBlocks(String halfBlocks) {
    if (halfBlocks.size() != 16) {
        throw std::invalid_argument("Half blocks string must contain exactly 16 characters");
    }
    for (const auto &character : halfBlocks) {
        if (character.displayWidth() != 1) {
            throw std::invalid_argument("Half blocks string must contain characters with display width of 1");
        }
    }
    _halfBlocks = std::move(halfBlocks);
}

auto BitmapDrawOptions::scaleMode() const noexcept -> BitmapScaleMode {
    return _scaleMode;
}

void BitmapDrawOptions::setScaleMode(BitmapScaleMode scaleMode) noexcept {
    _scaleMode = scaleMode;
}

auto BitmapDrawOptions::defaultOptions() noexcept -> const BitmapDrawOptions & {
    static const BitmapDrawOptions options;
    return options;
}


}
