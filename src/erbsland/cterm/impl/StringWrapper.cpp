// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StringWrapper.hpp"


namespace erbsland::cterm::impl {


auto StringWrapper::wrapIntoLines(const int width, const ParagraphSpacing paragraphSpacing) const noexcept
    -> std::vector<String> {
    if (width <= 0) {
        return {};
    }
    auto lines = StringLines{};
    auto isFirstParagraph = true;
    for (const auto &paragraph : splitLines()) {
        if (!isFirstParagraph && paragraphSpacing == ParagraphSpacing::DoubleLine) {
            lines.emplace_back();
        }
        const auto paragraphLines = StringWrapper{paragraph}.wrapParagraphIntoLines(width);
        if (paragraphLines.empty()) {
            lines.emplace_back();
        } else {
            lines.insert(lines.end(), paragraphLines.begin(), paragraphLines.end());
        }
        isFirstParagraph = false;
    }
    return lines;
}


auto StringWrapper::splitLines() const noexcept -> std::vector<String> {
    if (_str.empty()) {
        return {};
    }
    auto result = std::vector<String>{};
    result.reserve(_str.count(U'\n') + 1);
    auto lineStartIndex = std::size_t{0};
    while (lineStartIndex < _str.size()) {
        const auto lineEndIndex = _str.indexOf(U'\n', lineStartIndex);
        if (lineEndIndex == String::npos) {
            result.emplace_back(_str.substr(lineStartIndex));
            break;
        }
        result.emplace_back(_str.substr(lineStartIndex, lineEndIndex - lineStartIndex));
        lineStartIndex = lineEndIndex + 1;
    }
    return result;
}


void StringWrapper::clearPendingSpacing(String &pendingSpacing, int &pendingSpacingWidth) noexcept {
    pendingSpacing.clear();
    pendingSpacingWidth = 0;
}


void StringWrapper::finishWrappedLine(
    String &line,
    int &lineWidth,
    String &pendingSpacing,
    int &pendingSpacingWidth,
    std::vector<String> &lines) noexcept {
    if (!line.empty()) {
        lines.emplace_back(std::move(line));
        line.clear();
    }
    lineWidth = 0;
    clearPendingSpacing(pendingSpacing, pendingSpacingWidth);
}


void StringWrapper::appendSpacingToken(
    String &spacing,
    const int spacingWidth,
    const String &line,
    String &pendingSpacing,
    int &pendingSpacingWidth) noexcept {
    if (line.empty()) {
        spacing.clear();
        return;
    }
    if (pendingSpacing.empty()) {
        pendingSpacing = std::move(spacing);
    } else {
        pendingSpacing += spacing;
    }
    pendingSpacingWidth += spacingWidth;
    spacing.clear();
}


void StringWrapper::appendWordToken(
    String &word,
    const int wordWidth,
    const int width,
    String &line,
    int &lineWidth,
    String &pendingSpacing,
    int &pendingSpacingWidth,
    std::vector<String> &lines) noexcept {
    if (line.empty()) {
        clearPendingSpacing(pendingSpacing, pendingSpacingWidth);
        startWrappedLine(word, wordWidth, width, line, lineWidth, lines);
        return;
    }
    if (lineWidth + pendingSpacingWidth + wordWidth <= width) {
        line += pendingSpacing;
        line += word;
        lineWidth += pendingSpacingWidth + wordWidth;
        clearPendingSpacing(pendingSpacing, pendingSpacingWidth);
        return;
    }
    finishWrappedLine(line, lineWidth, pendingSpacing, pendingSpacingWidth, lines);
    startWrappedLine(word, wordWidth, width, line, lineWidth, lines);
}


void StringWrapper::startWrappedLine(
    String &word,
    const int wordWidth,
    const int width,
    String &line,
    int &lineWidth,
    std::vector<String> &lines) noexcept {
    if (wordWidth > width) {
        splitWordIntoWrappedLines(word, wordWidth, width, lines);
        line.clear();
        lineWidth = 0;
        word.clear();
        return;
    }
    line = std::move(word);
    lineWidth = wordWidth;
    word.clear();
}


void StringWrapper::splitWordIntoWrappedLines(
    const String &word, const int wordWidth, const int width, std::vector<String> &lines) noexcept {
    const auto estimatedLineCount = static_cast<std::size_t>((wordWidth + width - 1) / width);
    lines.reserve(lines.size() + estimatedLineCount);
    auto startIndex = std::size_t{0};
    while (startIndex < word.size()) {
        const auto endIndex = findWrappedWordSplitIndex(word, startIndex, width);
        lines.emplace_back(word.substr(startIndex, endIndex - startIndex));
        startIndex = endIndex;
    }
}


auto StringWrapper::findWrappedWordSplitIndex(
    const String &word, const std::size_t startIndex, const int width) noexcept -> std::size_t {
    auto lineWidth = 0;
    auto index = startIndex;
    while (index < word.size()) {
        const auto characterWidth = word[index].displayWidth();
        if (lineWidth > 0 && lineWidth + characterWidth > width) {
            break;
        }
        lineWidth += characterWidth;
        ++index;
        if (lineWidth >= width) {
            break;
        }
    }
    return index;
}


auto StringWrapper::wrapParagraphIntoLines(const int width) const noexcept -> std::vector<String> {
    if (width <= 0) {
        return {};
    }
    auto lines = StringLines{};
    auto line = String{};
    auto lineWidth = 0;
    auto pendingSpacing = String{};
    auto pendingSpacingWidth = 0;
    auto token = String{};
    auto isSpacingToken = false;
    for (const auto &character : _str) {
        const auto currentIsSpacing = character.isSpacing();
        if (token.empty()) {
            token.append(character);
            isSpacingToken = currentIsSpacing;
            continue;
        }
        if (currentIsSpacing == isSpacingToken) {
            token.append(character);
            continue;
        }
        const auto tokenWidth = token.displayWidth();
        if (isSpacingToken) {
            appendSpacingToken(token, tokenWidth, line, pendingSpacing, pendingSpacingWidth);
        } else {
            appendWordToken(token, tokenWidth, width, line, lineWidth, pendingSpacing, pendingSpacingWidth, lines);
        }
        token.clear();
        token.append(character);
        isSpacingToken = currentIsSpacing;
    }
    if (!token.empty()) {
        const auto tokenWidth = token.displayWidth();
        if (isSpacingToken) {
            appendSpacingToken(token, tokenWidth, line, pendingSpacing, pendingSpacingWidth);
        } else {
            appendWordToken(token, tokenWidth, width, line, lineWidth, pendingSpacing, pendingSpacingWidth, lines);
        }
    }
    finishWrappedLine(line, lineWidth, pendingSpacing, pendingSpacingWidth, lines);
    return lines;
}


}
