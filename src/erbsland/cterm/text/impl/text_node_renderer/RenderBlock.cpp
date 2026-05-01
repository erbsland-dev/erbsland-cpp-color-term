// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "RenderBlock.hpp"

#include "../../../impl/StringBuilder.hpp"
#include "../../../StringView.hpp"

namespace erbsland::cterm::text::impl::text_node_renderer {

auto RenderBlock::renderString() const -> String {
    if (_kind == BlockKind::HorizontalRule) {
        auto result = cterm::impl::StringBuilder{};
        const auto fill = String{8, _fillCharacter.value_or(Char{U'-'})};
        auto reservedSize = fill.size();
        if (_leadingText.has_value()) {
            reservedSize += _leadingText->size();
        }
        if (_trailingText.has_value()) {
            reservedSize += _trailingText->size();
        }
        result.reserve(reservedSize);
        if (_leadingText.has_value()) {
            result.append(*_leadingText);
        }
        result.append(fill);
        if (_trailingText.has_value()) {
            result.append(*_trailingText);
        }
        return result.takeString();
    }
    auto result = cterm::impl::StringBuilder{};
    const auto lines = _text.splitLines();
    auto firstLine = true;
    for (const auto &line : lines) {
        if (!firstLine) {
            result.append(Char{U'\n'});
        }
        const auto indent = firstLine ? _stringFirstLineIndent : _stringWrappedLineIndent;
        for (auto index = 0; index < std::max(indent, 0); ++index) {
            result.append(Char{U' '});
        }
        if (firstLine && _listPrefix.has_value()) {
            result.append(_listPrefix->text());
        }
        result.append(line);
        firstLine = false;
    }
    return result.takeString();
}

}
