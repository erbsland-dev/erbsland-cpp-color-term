// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Char.hpp"

#include <algorithm>
#include <ranges>

namespace erbsland::cterm {

auto Char::withCombining(const char32_t codePoint, const EncodingErrors encodingErrors) const -> Char {
    auto result = *this;
    result._character = _character.withCombining(codePoint, encodingErrors);
    return result;
}

auto Char::withOverlay(const CharStyle style) const noexcept -> Char {
    auto result = *this;
    result._style = _style.withOverlay(style);
    return result;
}

auto Char::withColorReplaced(const Color color) const noexcept -> Char {
    auto result = *this;
    result._style.setColor(color);
    return result;
}

auto Char::withStyleReplaced(const CharStyle style) const noexcept -> Char {
    auto result = *this;
    result._style = style;
    return result;
}

auto Char::withAttributes(const CharAttributes attributes) const noexcept -> Char {
    auto result = *this;
    result._style.setAttributes(attributes);
    return result;
}

auto Char::withBase(const CharStyle style) const noexcept -> Char {
    auto result = *this;
    result._style = _style.withBase(style);
    return result;
}

auto Char::withBase(const Char &base) const noexcept -> Char {
    auto result = *this;
    result._style = _style.withBase(base.style());
    return result;
}

auto Char::isSpacing() const noexcept -> bool {
    return _character.isSpacing();
}

auto Char::isControl() const noexcept -> bool {
    return _character.isControl();
}

auto Char::isOneOf(std::u32string_view characters) const noexcept -> bool {
    return std::ranges::any_of(characters, [this](const char32_t character) -> bool { return *this == character; });
}

auto Char::isOneOf(std::initializer_list<char32_t> characters) const noexcept -> bool {
    return std::ranges::any_of(characters, [this](const char32_t character) -> bool { return *this == character; });
}

auto Char::renderedEquals(const Char &other, const bool colorEnabled, const bool attributeEnabled) const noexcept
    -> bool {
    if (_character != other._character) {
        return false;
    }
    if (colorEnabled) {
        if (color().fg().ansiCode() != other.color().fg().ansiCode() ||
            color().bg().ansiCode() != other.color().bg().ansiCode()) {
            return false;
        }
    }
    if (attributeEnabled) {
        const auto resolvedAttributes = attributes().withBase(CharAttributes::reset());
        const auto otherResolvedAttributes = other.attributes().withBase(CharAttributes::reset());
        if (resolvedAttributes != otherResolvedAttributes) {
            return false;
        }
    }
    return true;
}

auto Char::space() noexcept -> const Char & {
    static const Char space{U' '};
    return space;
}

auto Char::emptyBlock(const CharStyle style) noexcept -> Char {
    return Char{impl::CombinedChar{}, style};
}

}
