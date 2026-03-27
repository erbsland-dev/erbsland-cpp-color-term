// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "RendererBase.hpp"


namespace erbsland::cterm::impl::paragraph {


RendererBase::RendererBase(
    const Alignment alignment,
    const LayoutResult &layout,
    const String &sourceText,
    const ParagraphOptions &options,
    const ParagraphBackgroundMode backgroundMode) noexcept :
    _alignment{alignment},
    _layout{layout},
    _sourceText{sourceText},
    _options{options},
    _backgroundMode{backgroundMode} {
}

auto RendererBase::linePlacement(const LayoutLine &line, const int x1, const int width) const noexcept
    -> LinePlacement {
    const auto endMarkWidth = line.wrapsToNext ? options().lineBreakEndMark().displayWidth() : 0;
    const auto availableWidth = width - endMarkWidth;
    const auto textWidth = line.textWidth();
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
