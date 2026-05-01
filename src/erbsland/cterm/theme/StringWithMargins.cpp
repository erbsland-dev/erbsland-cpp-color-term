// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StringWithMargins.hpp"

namespace erbsland::cterm::theme {

void StringWithMargins::join(const StringWithMargins &other) noexcept {
    const auto collapsedInnerMargins = std::max(_margins.right(), other._margins.left());
    _text.append(static_cast<std::size_t>(collapsedInnerMargins), Char::space());
    _text.append(other._text);
    _margins = Margins{
        std::max(_margins.top(), other._margins.top()),
        other._margins.right(),
        std::max(_margins.bottom(), other._margins.bottom()),
        _margins.left()};
}

auto StringWithMargins::joined(const StringWithMargins &other) const noexcept -> StringWithMargins {
    auto text = _text;
    const auto collapsedInnerMargins = std::max(_margins.right(), other._margins.left());
    text.append(static_cast<std::size_t>(collapsedInnerMargins), Char::space());
    text.append(other._text);
    return StringWithMargins{
        text,
        Margins{
            std::max(_margins.top(), other._margins.top()),
            other._margins.right(),
            std::max(_margins.bottom(), other._margins.bottom()),
            _margins.left()}};
}

}
