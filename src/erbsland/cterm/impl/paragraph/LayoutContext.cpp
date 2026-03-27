// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "LayoutContext.hpp"

namespace erbsland::cterm::impl::paragraph {


LayoutContext::LayoutContext(
    const String &text, const int width, const ParagraphOptions &options, const bool leftAligned) noexcept :
    _text{text},
    _width{width},
    _options{options},
    _leftAligned{leftAligned},
    _lineBreakStartMarkWidth{options.lineBreakStartMark().displayWidth()},
    _lineBreakEndMarkWidth{options.lineBreakEndMark().displayWidth()},
    _paragraphEllipsisWidth{options.paragraphEllipsisMark().displayWidth()},
    _wordBreakMarkWidth{options.wordBreakMark().displayWidth()} {
}


}
