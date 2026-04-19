// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StyleMarker.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <string>

namespace erbsland::cterm::text {

auto StyleMarker::clear() noexcept -> StyleMarker & {
    _kind = Kind::None;
    _literal = String{};
    _suffix = String{};
    return *this;
}

auto StyleMarker::setStyle(const CharStyle &style) noexcept -> StyleMarker & {
    _style = style;
    return *this;
}

auto StyleMarker::setStyle(const Color color, const CharAttributes attributes) noexcept -> StyleMarker & {
    _style = CharStyle{color, attributes};
    return *this;
}

auto StyleMarker::setLiteral(String literal, const CharStyle &style) -> StyleMarker & {
    _kind = Kind::Literal;
    _style = style;
    _literal = std::move(literal);
    _suffix = String{};
    return *this;
}

auto StyleMarker::setLiteral(const std::u32string_view literal, const CharStyle &style) -> StyleMarker & {
    return setLiteral(String{literal, style}, style);
}

auto StyleMarker::setOrdered(String suffix, const CharStyle &style) -> StyleMarker & {
    _kind = Kind::Ordered;
    _style = style;
    _literal = String{};
    _suffix = std::move(suffix);
    return *this;
}

auto StyleMarker::setOrdered(const std::u32string_view suffix, const CharStyle &style) -> StyleMarker & {
    return setOrdered(String{suffix, style}, style);
}

auto StyleMarker::render(const std::size_t number, const CharStyle &baseStyle, const int targetColumn) const
    -> Rendered {
    const auto markerStyle = baseStyle.withOverlay(_style);
    auto result = Rendered{};

    const auto appendTab = [&](const CharStyle &tabStyle) {
        const auto spaceCount = std::max(targetColumn - result.width, 1);
        result.text += String{static_cast<std::size_t>(spaceCount), Char{U' ', tabStyle}};
        result.width += spaceCount;
    };
    const auto appendCharacters = [&](const String &value) {
        for (const auto &character : value) {
            const auto resolved = character.withBase(markerStyle.color(), markerStyle.attributes());
            if (resolved == U'\t') {
                appendTab(resolved.style());
                result.terminalText += resolved;
                continue;
            }
            result.text += resolved;
            result.terminalText += resolved;
            result.width += resolved.displayWidth();
        }
    };

    switch (_kind) {
    case Kind::None:
        return result;
    case Kind::Literal:
        appendCharacters(_literal);
        return result;
    case Kind::Ordered: {
        auto buffer = std::array<char, 32>{};
        const auto [end, error] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), number);
        if (error == std::errc{}) {
            const auto text = std::string_view{buffer.data(), static_cast<std::size_t>(end - buffer.data())};
            result.text.appendStyled(text, markerStyle);
            result.terminalText.appendStyled(text, markerStyle);
            result.width = static_cast<int>(text.size());
        } else {
            const auto text = std::to_string(number);
            result.text.appendStyled(text, markerStyle);
            result.terminalText.appendStyled(text, markerStyle);
            result.width = static_cast<int>(text.size());
        }
        appendCharacters(_suffix);
        return result;
    }
    }
    return result;
}

}
