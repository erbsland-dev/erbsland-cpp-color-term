// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TextNodeRenderer.hpp"

#include "text_node_renderer/BlockEmitter.hpp"
#include "text_node_renderer/RenderPlanner.hpp"

#include "../../impl/StringBuilder.hpp"
#include "../../StringView.hpp"

namespace erbsland::cterm::text::impl {

TextNodeRenderer::TextNodeRenderer(TextNodeConstPtr document, StyleConstPtr style) :
    _document{std::move(document)}, _style{style != nullptr ? std::move(style) : Style::defaultStyle()} {
}

auto TextNodeRenderer::renderString() const -> String {
    auto result = cterm::impl::StringBuilder{};
    const auto planner = text_node_renderer::RenderPlanner{_style};
    const auto blocks = planner.build(_document);
    auto firstBlock = true;
    for (const auto &block : blocks) {
        if (!firstBlock) {
            result.append(Char{U'\n'});
        }
        result.append(block.renderString());
        firstBlock = false;
    }
    return result.takeString().trimmed(U"\n\r\t");
}

void TextNodeRenderer::renderTo(const CursorWriterPtr &cursorWriterPtr) const {
    if (!cursorWriterPtr) {
        return;
    }
    const auto planner = text_node_renderer::RenderPlanner{_style};
    text_node_renderer::BlockEmitter{cursorWriterPtr}.render(planner.build(_document));
}

}
