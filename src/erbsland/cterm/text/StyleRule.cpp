// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StyleRule.hpp"

namespace erbsland::cterm::text {

auto StyleRule::setTextStyle(const CharStyle &style) noexcept -> StyleRule & {
    _textStyle = style;
    return *this;
}

auto StyleRule::setTextStyle(const Color color, const CharAttributes attributes) noexcept -> StyleRule & {
    _textStyle = CharStyle{color, attributes};
    return *this;
}

auto StyleRule::setIndents(const ParagraphIndents &indents) noexcept -> StyleRule & {
    _indents = indents;
    return *this;
}

auto StyleRule::setMargins(const Margins margins) noexcept -> StyleRule & {
    _indents.setMargins(margins);
    return *this;
}

auto StyleRule::setMargins(const int allSides) noexcept -> StyleRule & {
    return setMargins(Margins{allSides});
}

auto StyleRule::setMargins(const int horizontal, const int vertical) noexcept -> StyleRule & {
    return setMargins(Margins{horizontal, vertical});
}

auto StyleRule::setMargins(const int top, const int right, const int bottom, const int left) noexcept -> StyleRule & {
    return setMargins(Margins{top, right, bottom, left});
}

auto StyleRule::setLineIndent(const int indent) noexcept -> StyleRule & {
    _indents.setLineIndent(indent);
    return *this;
}

auto StyleRule::setFirstLineIndent(const int indent) noexcept -> StyleRule & {
    _indents.setFirstLineIndent(indent);
    return *this;
}

auto StyleRule::setWrappedLineIndent(const int indent) noexcept -> StyleRule & {
    _indents.setWrappedLineIndent(indent);
    return *this;
}

auto StyleRule::setPrefix(String prefix) -> StyleRule & {
    _prefix = std::move(prefix);
    return *this;
}

auto StyleRule::setPrefix(const std::u32string_view prefix, const CharStyle &style) -> StyleRule & {
    _prefix = String{prefix, style};
    return *this;
}

auto StyleRule::clearPrefix() noexcept -> StyleRule & {
    _prefix.reset();
    return *this;
}

auto StyleRule::setSuffix(String suffix) -> StyleRule & {
    _suffix = std::move(suffix);
    return *this;
}

auto StyleRule::setSuffix(const std::u32string_view suffix, const CharStyle &style) -> StyleRule & {
    _suffix = String{suffix, style};
    return *this;
}

auto StyleRule::clearSuffix() noexcept -> StyleRule & {
    _suffix.reset();
    return *this;
}

auto StyleRule::setLineFill(const Char &lineFill) noexcept -> StyleRule & {
    _lineFill = lineFill;
    return *this;
}

auto StyleRule::setLineFill(const char32_t codePoint, const CharStyle &style) noexcept -> StyleRule & {
    _lineFill = Char{codePoint, style};
    return *this;
}

auto StyleRule::clearLineFill() noexcept -> StyleRule & {
    _lineFill.reset();
    return *this;
}

auto StyleRule::setMarker(const StyleMarker &marker) -> StyleRule & {
    _marker = marker;
    return *this;
}

auto StyleRule::setLiteralMarker(String literal, const CharStyle &style) -> StyleRule & {
    _marker.setLiteral(std::move(literal), style);
    return *this;
}

auto StyleRule::setLiteralMarker(const std::u32string_view literal, const CharStyle &style) -> StyleRule & {
    _marker.setLiteral(literal, style);
    return *this;
}

auto StyleRule::setOrderedMarker(String suffix, const CharStyle &style) -> StyleRule & {
    _marker.setOrdered(std::move(suffix), style);
    return *this;
}

auto StyleRule::setOrderedMarker(const std::u32string_view suffix, const CharStyle &style) -> StyleRule & {
    _marker.setOrdered(suffix, style);
    return *this;
}

auto StyleRule::clearMarker() noexcept -> StyleRule & {
    _marker.clear();
    return *this;
}

}
