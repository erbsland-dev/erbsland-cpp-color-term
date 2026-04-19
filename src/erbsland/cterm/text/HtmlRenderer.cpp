// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "HtmlRenderer.hpp"

#include "impl/HtmlParser.hpp"
#include "impl/TextNodeRenderer.hpp"

namespace erbsland::cterm::text {

auto HtmlRenderer::renderString() const -> String {
    const auto document = impl::HtmlParser{_html}.parse();
    return impl::TextNodeRenderer{document, _style}.renderString();
}

void HtmlRenderer::renderTo(const CursorWriterPtr &cursorWriterPtr) const {
    const auto document = impl::HtmlParser{_html}.parse();
    impl::TextNodeRenderer{document, _style}.renderTo(cursorWriterPtr);
}

auto HtmlRenderer::parse(const std::string_view html) -> TextNodePtr {
    return impl::HtmlParser{html}.parse();
}

}
