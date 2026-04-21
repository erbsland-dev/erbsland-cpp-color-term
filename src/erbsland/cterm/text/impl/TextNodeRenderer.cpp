// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TextNodeRenderer.hpp"

#include "text_node_renderer/CursorBlockTarget.hpp"
#include "text_node_renderer/StringBlockTarget.hpp"
#include "text_node_renderer/TreeRenderer.hpp"

namespace erbsland::cterm::text::impl {

TextNodeRenderer::TextNodeRenderer(TextNodeConstPtr document, const StyleConstPtr &style) :
    _document{std::move(document)}, _style{style != nullptr ? style : Style::defaultStyle()} {
}

auto TextNodeRenderer::renderString() const -> String {
    auto target = text_node_renderer::StringBlockTarget{};
    text_node_renderer::TreeRenderer{_style, target}.render(_document);
    return target.takeString();
}

void TextNodeRenderer::renderTo(const CursorWriterPtr &cursorWriterPtr) const {
    if (!cursorWriterPtr) {
        return;
    }
    auto target = text_node_renderer::CursorBlockTarget{cursorWriterPtr};
    text_node_renderer::TreeRenderer{_style, target}.render(_document);
}

}
