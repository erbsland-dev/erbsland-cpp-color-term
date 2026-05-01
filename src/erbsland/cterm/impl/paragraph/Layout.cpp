// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Layout.hpp"

#include "LineBuilder.hpp"

#include <utility>

namespace erbsland::cterm::impl::paragraph {

Layout::Layout(
    const StringView &text,
    const int width,
    const ParagraphOptions &options,
    const LayoutNewlineMode newlineMode) noexcept :
    _context{text, width, options, options.alignment().isLeft()},
    _newlineMode{newlineMode},
    _wordSeparators{_context.options().wordSeparatorSet()} {
}

auto Layout::build() -> LayoutResult {
    if (_context.width() <= 0) {
        return LayoutResult::invalid();
    }
    auto result = LayoutResult::create();
    const auto sourceLines = splitIntoSourceLines();
    if (_newlineMode == LayoutNewlineMode::HardLineBreak) {
        if (!layoutParagraph(sourceLines, result.lines())) {
            return LayoutResult::invalid();
        }
        return result;
    }
    auto isFirstParagraph = true;
    for (const auto sourceLine : sourceLines) {
        if (!isFirstParagraph && _context.options().paragraphSpacing() == ParagraphSpacing::DoubleLine) {
            result.appendEmptyLine();
        }
        if (!layoutSourceLine(sourceLine, result.lines())) {
            return LayoutResult::invalid();
        }
        isFirstParagraph = false;
    }
    return result;
}

auto Layout::splitIntoSourceLines() const -> std::vector<IndexRange> {
    const auto &text = _context.text();
    if (text.empty()) {
        return {};
    }
    auto result = std::vector<IndexRange>{};
    auto lineStartIndex = std::size_t{0};
    for (auto index = std::size_t{0}; index < text.size(); ++index) {
        if (text[index] == U'\n') {
            result.push_back(IndexRange{lineStartIndex, index - lineStartIndex});
            lineStartIndex = index + 1;
        }
    }
    if (lineStartIndex < text.size()) {
        result.push_back(IndexRange{lineStartIndex, text.size() - lineStartIndex});
    }
    return result;
}

auto Layout::layoutParagraph(const std::vector<IndexRange> &sourceLines, std::vector<LayoutLine> &lines) -> bool {
    if (sourceLines.empty()) {
        return true;
    }
    for (const auto &sourceLine : sourceLines) {
        if (!layoutSourceLine(sourceLine, lines)) {
            return false;
        }
    }
    return true;
}

auto Layout::layoutSourceLine(const IndexRange sourceLine, std::vector<LayoutLine> &lines) -> bool {
    const auto preparedLine = prepareSourceLine(sourceLine);
    if (preparedLine.empty()) {
        lines.emplace_back();
        return true;
    }
    auto lineBuilder = LineBuilder{_context, preparedLine};
    return lineBuilder.appendLinesTo(lines);
}

auto Layout::prepareSourceLine(const IndexRange sourceLine) const -> LayoutPreparedSourceLine {
    auto result = LayoutPreparedSourceLine{sourceLine};
    const auto leftAligned = _context.leftAligned();
    const auto &text = _context.text();
    auto currentWordStartIndex = std::size_t{0};
    auto currentWordLength = std::size_t{0};
    auto currentWordWidth = 0;
    auto finishCurrentWord = [&]() -> void {
        result.appendWord(currentWordStartIndex, currentWordLength, currentWordWidth);
        currentWordStartIndex = 0;
        currentWordLength = 0;
        currentWordWidth = 0;
    };
    for (auto index = sourceLine.startIndex(); index < sourceLine.startIndex() + sourceLine.length(); ++index) {
        const auto &character = text[index];
        const auto codePoint = character.singleCodePoint();
        if (leftAligned && codePoint == U'\t') {
            finishCurrentWord();
            result.appendPendingTab(index);
            continue;
        }
        if (_wordSeparators->contains(codePoint)) {
            finishCurrentWord();
            result.appendPendingSeparatorSpace(index);
            continue;
        }
        if (currentWordLength == 0) {
            currentWordStartIndex = index;
        }
        currentWordLength += 1;
        currentWordWidth += character.displayWidth();
    }
    finishCurrentWord();
    result.finish();
    return result;
}

}
