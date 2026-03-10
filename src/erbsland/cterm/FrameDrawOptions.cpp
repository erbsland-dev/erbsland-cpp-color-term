// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "FrameDrawOptions.hpp"


namespace erbsland::cterm {


FrameDrawOptions::FrameDrawOptions(ColorSequence frameColor, const FrameColorMode frameColorMode) :
    _frameColor{std::move(frameColor)}, _frameColorMode{frameColorMode} {
}

auto FrameDrawOptions::frameColor() const noexcept -> const ColorSequence & {
    return _frameColor;
}

void FrameDrawOptions::setFrameColor(const Color frameColor) noexcept {
    _frameColor = {frameColor};
}

void FrameDrawOptions::setFrameColor(const Foreground foreground) noexcept {
    _frameColor = {Color{foreground, bg::Inherited}};
}

void FrameDrawOptions::setFrameColor(const Foreground::Hue foreground) noexcept {
    _frameColor = {Color{foreground, bg::Inherited}};
}

void FrameDrawOptions::setFrameColor(const Foreground foreground, const Background background) noexcept {
    _frameColor = {Color{foreground, background}};
}

void FrameDrawOptions::setFrameColor(const Background background) noexcept {
    _frameColor = {Color{fg::Inherited, background}};
}

void FrameDrawOptions::setFrameColor(const Background::Hue background) noexcept {
    _frameColor = {Color{fg::Inherited, background}};
}

void FrameDrawOptions::setFrameColorSequence(ColorSequence frameColor, const FrameColorMode frameColorMode) noexcept {
    _frameColor = std::move(frameColor);
    _frameColorMode = frameColorMode;
}

auto FrameDrawOptions::fillColor() const noexcept -> const ColorSequence & {
    return _fillColor;
}

void FrameDrawOptions::setFillColor(const Color fillColor) noexcept {
    _fillColor = {fillColor};
}

void FrameDrawOptions::setFillColor(const Foreground foreground) noexcept {
    _fillColor = {Color{foreground, bg::Inherited}};
}

void FrameDrawOptions::setFillColor(const Foreground::Hue foreground) noexcept {
    _fillColor = {Color{foreground, bg::Inherited}};
}

void FrameDrawOptions::setFillColor(const Foreground foreground, const Background background) noexcept {
    _fillColor = {Color{foreground, background}};
}

void FrameDrawOptions::setFillColor(const Background background) noexcept {
    _fillColor = {Color{fg::Inherited, background}};
}

void FrameDrawOptions::setFillColor(const Background::Hue background) noexcept {
    _fillColor = {Color{fg::Inherited, background}};
}

void FrameDrawOptions::setFillColorSequence(ColorSequence fillColor, const FrameColorMode fillColorMode) noexcept {
    _fillColor = std::move(fillColor);
    _fillColorMode = fillColorMode;
}

auto FrameDrawOptions::fillBlock() const noexcept -> const Char & {
    return _fillBlock;
}

void FrameDrawOptions::setFillBlock(Char fillBlock) noexcept {
    _fillBlock = std::move(fillBlock);
}

auto FrameDrawOptions::style() const noexcept -> FrameStyle {
    return _style;
}

void FrameDrawOptions::setStyle(const FrameStyle style) noexcept {
    _style = style;
}

auto FrameDrawOptions::char16Style() const noexcept -> const Char16StylePtr & {
    return _char16Style;
}

void FrameDrawOptions::setChar16Style(Char16StylePtr char16Style) noexcept {
    _char16Style = std::move(char16Style);
}

auto FrameDrawOptions::tile9Style() const noexcept -> const Tile9StylePtr & {
    return _tile9Style;
}

void FrameDrawOptions::setTile9Style(Tile9StylePtr tile9Style) noexcept {
    _tile9Style = std::move(tile9Style);
}

auto FrameDrawOptions::combinationStyle() const noexcept -> const CharCombinationStylePtr & {
    return _combinationStyle;
}

void FrameDrawOptions::setCombinationStyle(CharCombinationStylePtr combinationStyle) noexcept {
    _combinationStyle = std::move(combinationStyle);
}

auto FrameDrawOptions::animationOffset() const noexcept -> std::size_t {
    return _animationOffset;
}

void FrameDrawOptions::setAnimationOffset(const std::size_t offset) noexcept {
    _animationOffset = offset;
}

auto FrameDrawOptions::frameColorMode() const noexcept -> FrameColorMode {
    return _frameColorMode;
}

void FrameDrawOptions::setFrameColorMode(const FrameColorMode frameColorMode) noexcept {
    _frameColorMode = frameColorMode;
}

auto FrameDrawOptions::fillColorMode() const noexcept -> FrameColorMode {
    return _fillColorMode;
}

void FrameDrawOptions::setFillColorMode(const FrameColorMode fillColorMode) noexcept {
    _fillColorMode = fillColorMode;
}

auto FrameDrawOptions::defaultOptions() noexcept -> const FrameDrawOptions & {
    static const FrameDrawOptions options;
    return options;
}


}
