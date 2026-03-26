// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ParagraphRendererBase.hpp"


namespace erbsland::cterm::impl {


ParagraphRendererBase::ParagraphRendererBase(
    const Alignment alignment,
    const ParagraphLayout::Result &layout,
    const ParagraphBackgroundMode backgroundMode) noexcept :
    _alignment{alignment}, _layout{layout}, _backgroundMode{backgroundMode} {
}

auto ParagraphRendererBase::usesLeftFill() const noexcept -> bool {
    return _backgroundMode == ParagraphBackgroundMode::WrappedLeft ||
        _backgroundMode == ParagraphBackgroundMode::WrappedBoth || _backgroundMode == ParagraphBackgroundMode::FullBoth;
}

auto ParagraphRendererBase::usesRightFill() const noexcept -> bool {
    return _backgroundMode == ParagraphBackgroundMode::WrappedRight ||
        _backgroundMode == ParagraphBackgroundMode::WrappedBoth ||
        _backgroundMode == ParagraphBackgroundMode::FullRight || _backgroundMode == ParagraphBackgroundMode::FullBoth;
}

auto ParagraphRendererBase::usesRightFillForLine(const ParagraphLayout::Line &line) const noexcept -> bool {
    if (_backgroundMode == ParagraphBackgroundMode::FullRight || _backgroundMode == ParagraphBackgroundMode::FullBoth) {
        return true;
    }
    return line.wrapsToNext &&
        (_backgroundMode == ParagraphBackgroundMode::WrappedRight ||
         _backgroundMode == ParagraphBackgroundMode::WrappedBoth);
}

auto ParagraphRendererBase::linePlacement(
    const ParagraphLayout::Line &line, const int x1, const int width) const noexcept -> LinePlacement {
    const auto endMarkWidth = line.endMark.displayWidth();
    const auto availableWidth = width - endMarkWidth;
    const auto textWidth = line.text.displayWidth();
    auto textX = x1;
    switch (_alignment & Alignment::HorizontalMask) {
    case Alignment::Right:
        textX = x1 + availableWidth - textWidth;
        break;
    case Alignment::HCenter:
        textX = x1 + (availableWidth - textWidth) / 2;
        break;
    default:
        break;
    }
    return LinePlacement{
        .textX = textX, .textWidth = textWidth, .endMarkX = x1 + width - endMarkWidth, .endMarkWidth = endMarkWidth};
}


}
