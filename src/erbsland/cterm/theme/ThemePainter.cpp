// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ThemePainter.hpp"

#include <memory>
#include <utility>

namespace erbsland::cterm::theme {

ThemePainter::ThemePainter(WritableBuffer &buffer, ThemeAccessor theme) noexcept :
    _buffer{buffer}, _theme{std::move(theme)} {
}

void ThemePainter::fill(const Rectangle rect) const noexcept {
    _buffer.fill(rect, std::make_shared<Tile9Style>(_theme.tile9Style()));
}

void ThemePainter::drawFrame(const Rectangle rect) const noexcept {
    _buffer.drawFrame(rect, std::make_shared<Tile9Style>(_theme.tile9Style()));
}

}
