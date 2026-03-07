// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Char.hpp"


#include "impl/UnicodeWidth.hpp"


namespace erbsland::cterm {


Char::Char(const std::string_view charStr) : Char{charStr, Color{}} {
}


Char::Char(const std::string_view charStr, const Foreground fgColor, const Background bgColor) :
    Char{charStr, {fgColor, bgColor}} {
}


Char::Char(const std::string_view charStr, const Color color) :
    _charStr{charStr}, _color{color}, _displayWidth{static_cast<uint8_t>(impl::calculateDisplayWidth(charStr))} {
}


auto Char::withColor(const Color color) const -> Char {
    auto newColor = _color;
    if (color.fg() != Foreground::Default) {
        newColor.setFg(color.fg());
    }
    if (color.bg() != Background::Default) {
        newColor.setBg(color.bg());
    }
    return Char{_charStr, newColor};
}


auto Char::isSpacing() const noexcept -> bool {
    return _charStr == " " || _charStr == "\t" || _charStr == "\n" || _charStr == "\r";
}


}
