// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Char.hpp"


#include "impl/U8Buffer.hpp"
#include "impl/UnicodeWidth.hpp"

#include <stdexcept>


namespace erbsland::cterm {


Char::Char(const std::string_view charStr) : Char{charStr, Color{}} {
}

Char::Char(const std::u32string_view charStr) : Char{charStr, Color{}} {
}

Char::Char(const char32_t *charStr) : Char{charStr, Color{}} {
}

Char::Char(const std::string_view charStr, const Foreground fgColor, const Background bgColor) :
    Char{charStr, {fgColor, bgColor}} {
}

Char::Char(const std::string_view charStr, const Foreground::Hue fgColor, const Background::Hue bgColor) :
    Char{charStr, {fgColor, bgColor}} {
}

Char::Char(const std::string_view charStr, const Foreground fgColor) : Char{charStr, {fgColor, Background::Inherited}} {
}

Char::Char(const std::string_view charStr, const Foreground::Hue fgColor) :
    Char{charStr, {fgColor, Background::Inherited}} {
}

Char::Char(const std::string_view charStr, const Background bgColor) : Char{charStr, {Foreground::Inherited, bgColor}} {
}

Char::Char(const std::string_view charStr, const Background::Hue bgColor) :
    Char{charStr, {Foreground::Inherited, bgColor}} {
}

Char::Char(const std::string_view charStr, const Color color) : _codePoints{decodeUtf8(charStr)}, _color{color} {
}

Char::Char(const std::u32string_view charStr, const Foreground fgColor, const Background bgColor) :
    Char{charStr, {fgColor, bgColor}} {
}

Char::Char(const std::u32string_view charStr, const Foreground::Hue fgColor, const Background::Hue bgColor) :
    Char{charStr, {fgColor, bgColor}} {
}

Char::Char(const std::u32string_view charStr, const Foreground fgColor) :
    Char{charStr, {fgColor, Background::Inherited}} {
}

Char::Char(const std::u32string_view charStr, const Foreground::Hue fgColor) :
    Char{charStr, {fgColor, Background::Inherited}} {
}

Char::Char(const std::u32string_view charStr, const Background bgColor) :
    Char{charStr, {Foreground::Inherited, bgColor}} {
}

Char::Char(const std::u32string_view charStr, const Background::Hue bgColor) :
    Char{charStr, {Foreground::Inherited, bgColor}} {
}

Char::Char(const std::u32string_view charStr, const Color color) : _codePoints{decodeUtf32(charStr)}, _color{color} {
}

Char::Char(const char32_t *charStr, const Foreground fgColor, const Background bgColor) :
    Char{charStr, {fgColor, bgColor}} {
}

Char::Char(const char32_t *charStr, const Foreground::Hue fgColor, const Background::Hue bgColor) :
    Char{charStr, {fgColor, bgColor}} {
}

Char::Char(const char32_t *charStr, const Foreground fgColor) : Char{charStr, {fgColor, Background::Inherited}} {
}

Char::Char(const char32_t *charStr, const Foreground::Hue fgColor) : Char{charStr, {fgColor, Background::Inherited}} {
}

Char::Char(const char32_t *charStr, const Background bgColor) : Char{charStr, {Foreground::Inherited, bgColor}} {
}

Char::Char(const char32_t *charStr, const Background::Hue bgColor) : Char{charStr, {Foreground::Inherited, bgColor}} {
}

Char::Char(const char32_t *charStr, const Color color) :
    Char{charStr != nullptr ? std::u32string_view{charStr} : std::u32string_view{}, color} {
}


auto Char::charStr() const -> std::string {
    auto result = std::string{};
    result.reserve(byteCount());
    appendTo(result);
    return result;
}


auto Char::displayWidth() const noexcept -> int {
    if (_displayWidthCache != cNoDisplayWidth) {
        return static_cast<int>(_displayWidthCache);
    }
    auto result = 0;
    for (const auto codePoint : _codePoints) {
        if (codePoint == 0) {
            break;
        }
        result += static_cast<int>(impl::consoleCharacterWidth(codePoint));
    }
    _displayWidthCache = static_cast<uint8_t>(result);
    return result;
}


auto Char::byteCount() const noexcept -> std::size_t {
    auto result = std::size_t{0};
    for (const auto codePoint : _codePoints) {
        if (codePoint == 0) {
            break;
        }
        result += utf8ByteCount(codePoint);
    }
    return result;
}


void Char::appendTo(std::string &buffer) const noexcept {
    buffer.reserve(buffer.size() + byteCount());
    for (const auto codePoint : _codePoints) {
        if (codePoint == 0) {
            break;
        }
        impl::U8Buffer<const char>::encodeChar(buffer, codePoint);
    }
}


auto Char::withCombining(const char32_t codePoint) const -> Char {
    if (codePointCount() == 0) {
        throw std::invalid_argument{"Char needs a base code point before combining code points can be appended."};
    }
    if (codePoint == 0) {
        throw std::invalid_argument{"Char does not support the Unicode code point U+0000."};
    }
    if (isControlCode(codePoint)) {
        throw std::invalid_argument{"Char combining code points must not be control codes."};
    }
    if (impl::consoleCharacterWidth(codePoint) != 0) {
        throw std::invalid_argument{"Char combining code points must have zero display width."};
    }
    auto result = *this;
    const auto index = countCodePoints(result._codePoints);
    if (index >= result._codePoints.size()) {
        throw std::invalid_argument{"Char supports at most three Unicode code points."};
    }
    result._codePoints[index] = codePoint;
    return result;
}


auto Char::withColorOverlay(const Color color) const -> Char {
    auto newColor = _color;
    if (color.fg() != Foreground::Inherited) {
        newColor.setFg(color.fg());
    }
    if (color.bg() != Background::Inherited) {
        newColor.setBg(color.bg());
    }
    auto result = *this;
    result._color = newColor;
    return result;
}


auto Char::withColorReplaced(const Color color) const noexcept -> Char {
    auto result = *this;
    result._color = color;
    return result;
}


auto Char::withBaseColor(const Color color) const noexcept -> Char {
    return withColorReplaced(color.overlayWith(_color));
}


auto Char::isSpacing() const noexcept -> bool {
    if (codePointCount() != 1) {
        return false;
    }
    return _codePoints[0] == U' ' || _codePoints[0] == U'\t' || _codePoints[0] == U'\n' || _codePoints[0] == U'\r';
}


auto Char::renderedEquals(const Char &other, const bool colorEnabled) const noexcept -> bool {
    if (codePoints() != other.codePoints()) {
        return false;
    }
    if (!colorEnabled) {
        return true;
    }
    return color().fg().ansiCode() == other.color().fg().ansiCode() &&
           color().bg().ansiCode() == other.color().bg().ansiCode();
}


auto Char::decodeUtf8(const std::string_view charStr) -> std::array<char32_t, 3> {
    auto result = std::array<char32_t, 3>{};
    auto index = std::size_t{0};
    impl::U8Buffer{charStr}.decodeAll([&](const char32_t codePoint) -> void {
        if (codePoint == 0) {
            throw std::invalid_argument{"Char does not support the Unicode code point U+0000."};
        }
        if (isControlCode(codePoint) && codePoint != U'\t' && codePoint != U'\n' && codePoint != U'\r') {
            throw std::invalid_argument{"Char does not support control codes except tab, newline, and carriage return."};
        }
        if (index >= result.size()) {
            throw std::invalid_argument{"Char supports at most three Unicode code points."};
        }
        result[index++] = codePoint;
    });
    return result;
}


auto Char::decodeUtf32(const std::u32string_view charStr) -> std::array<char32_t, 3> {
    auto result = std::array<char32_t, 3>{};
    if (charStr.size() > result.size()) {
        throw std::invalid_argument{"Char supports at most three Unicode code points."};
    }
    for (std::size_t index = 0; index < charStr.size(); ++index) {
        if (charStr[index] == 0) {
            throw std::invalid_argument{"Char does not support the Unicode code point U+0000."};
        }
        if (isControlCode(charStr[index]) && charStr[index] != U'\t' && charStr[index] != U'\n' &&
            charStr[index] != U'\r') {
            throw std::invalid_argument{"Char does not support control codes except tab, newline, and carriage return."};
        }
        result[index] = charStr[index];
    }
    return result;
}


}
