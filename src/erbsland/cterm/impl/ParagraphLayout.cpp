// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ParagraphLayout.hpp"


#include <algorithm>
#include <utility>


namespace erbsland::cterm::impl {


ParagraphLayout::ParagraphLayout(
    const String &text, const int width, const ParagraphOptions &options, const NewlineMode newlineMode) noexcept :
    _text{text},
    _width{width},
    _options{options},
    _newlineMode{newlineMode},
    _leftAligned{(_options.alignment() & Alignment::HorizontalMask) == Alignment::Left} {
}

auto ParagraphLayout::build() -> Result {
    if (_width <= 0) {
        return {.valid = false, .lines = {}};
    }
    auto result = Result{};
    const auto paragraphs = splitInputIntoParagraphs();
    auto isFirstParagraph = true;
    for (const auto &paragraph : paragraphs) {
        if (!isFirstParagraph && _newlineMode == NewlineMode::ParagraphBreak &&
            _options.paragraphSpacing() == ParagraphSpacing::DoubleLine) {
            result.lines.emplace_back();
        }
        if (!layoutParagraph(paragraph, result.lines)) {
            return {.valid = false, .lines = {}};
        }
        isFirstParagraph = false;
    }
    return result;
}

auto ParagraphLayout::splitInputIntoParagraphs() const -> std::vector<std::vector<String>> {
    if (_text.empty()) {
        return {};
    }
    if (_newlineMode == NewlineMode::HardLineBreak) {
        return {splitIntoSourceLines(_text)};
    }
    auto result = std::vector<std::vector<String>>{};
    for (const auto &paragraph : splitIntoSourceLines(_text)) {
        result.push_back({paragraph});
    }
    return result;
}

auto ParagraphLayout::splitIntoSourceLines(const String &text) -> std::vector<String> {
    return text.splitLines();
}

auto ParagraphLayout::layoutParagraph(const std::vector<String> &sourceLines, std::vector<Line> &lines) -> bool {
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

auto ParagraphLayout::layoutSourceLine(const String &sourceLine, std::vector<Line> &lines) -> bool {
    const auto preparedLine = prepareSourceLine(sourceLine);
    if (preparedLine.words.empty()) {
        lines.emplace_back();
        return true;
    }
    auto state = State{};
    auto wrappedLineCount = 0;
    auto isFirstPhysicalLine = true;
    while (state.wordIndex < preparedLine.words.size()) {
        if (const auto lastLine = tryBuildLastLine(preparedLine, state, isFirstPhysicalLine); lastLine.has_value()) {
            lines.push_back(std::move(*lastLine));
            break;
        }
        if (_options.maximumLineWraps() > 0 && wrappedLineCount >= _options.maximumLineWraps()) {
            const auto truncatedLine = buildTruncatedLine(preparedLine, state, isFirstPhysicalLine);
            if (!truncatedLine.has_value()) {
                return false;
            }
            lines.push_back(std::move(*truncatedLine));
            break;
        }
        const auto wrappedLine = buildWrappedLine(preparedLine, state, isFirstPhysicalLine);
        if (!wrappedLine.has_value()) {
            return false;
        }
        state = wrappedLine->nextState;
        lines.push_back(std::move(wrappedLine->line));
        isFirstPhysicalLine = false;
        wrappedLineCount += 1;
    }
    return true;
}

auto ParagraphLayout::prepareSourceLine(const String &sourceLine) const -> PreparedSourceLine {
    auto result = PreparedSourceLine{};
    auto currentWord = String{};
    auto pendingSpacing = std::vector<SpacingElement>{};
    auto pendingWordSeparator = std::optional<Char>{};
    auto hasWords = false;
    for (const auto &character : sourceLine) {
        if (isSpecialTab(character)) {
            finishCurrentWord(currentWord, pendingSpacing, pendingWordSeparator, result.words, hasWords);
            pendingSpacing.push_back({.type = SpacingElement::Type::Tab, .character = character});
            continue;
        }
        if (isWordSeparator(character)) {
            finishCurrentWord(currentWord, pendingSpacing, pendingWordSeparator, result.words, hasWords);
            if (!pendingWordSeparator.has_value()) {
                pendingWordSeparator = character;
            }
            continue;
        }
        if (pendingWordSeparator.has_value()) {
            if (hasWords) {
                pendingSpacing.push_back(
                    {.type = SpacingElement::Type::Space, .character = spaceFrom(*pendingWordSeparator)});
            }
            pendingWordSeparator.reset();
        }
        currentWord.append(character);
    }
    finishCurrentWord(currentWord, pendingSpacing, pendingWordSeparator, result.words, hasWords);
    return result;
}

void ParagraphLayout::finishCurrentWord(
    String &currentWord,
    std::vector<SpacingElement> &pendingSpacing,
    std::optional<Char> &pendingWordSeparator,
    std::vector<WordItem> &words,
    bool &hasWords) {
    if (currentWord.empty()) {
        if (!hasWords) {
            removeLeadingSpaces(pendingSpacing);
        }
        return;
    }
    if (pendingWordSeparator.has_value() && hasWords) {
        pendingSpacing.push_back({.type = SpacingElement::Type::Space, .character = spaceFrom(*pendingWordSeparator)});
    }
    if (!hasWords) {
        removeLeadingSpaces(pendingSpacing);
    }
    words.push_back({.prefixSpacing = pendingSpacing, .word = std::move(currentWord)});
    currentWord.clear();
    pendingSpacing.clear();
    pendingWordSeparator.reset();
    hasWords = true;
}

void ParagraphLayout::removeLeadingSpaces(std::vector<SpacingElement> &spacing) {
    spacing.erase(
        std::remove_if(
            spacing.begin(),
            spacing.end(),
            [](const SpacingElement &element) -> bool { return element.type == SpacingElement::Type::Space; }),
        spacing.end());
}

auto ParagraphLayout::tryBuildLastLine(
    const PreparedSourceLine &preparedLine, const State &initialState, const bool isFirstPhysicalLine) const
    -> std::optional<Line> {
    const auto builtLine = buildLine(preparedLine, initialState, isFirstPhysicalLine, 0, false, false);
    if (!builtLine.has_value() || !builtLine->consumedAll) {
        return std::nullopt;
    }
    return builtLine->line;
}

auto ParagraphLayout::buildWrappedLine(
    const PreparedSourceLine &preparedLine, const State &initialState, const bool isFirstPhysicalLine) const
    -> std::optional<LineBuild> {
    return buildLine(
        preparedLine, initialState, isFirstPhysicalLine, _options.lineBreakEndMark().displayWidth(), true, false);
}

auto ParagraphLayout::buildTruncatedLine(
    const PreparedSourceLine &preparedLine, const State &initialState, const bool isFirstPhysicalLine) const
    -> std::optional<Line> {
    const auto builtLine = buildLine(
        preparedLine, initialState, isFirstPhysicalLine, _options.paragraphEllipsisMark().displayWidth(), false, true);
    if (!builtLine.has_value()) {
        return std::nullopt;
    }
    return builtLine->line;
}

auto ParagraphLayout::buildLine(
    const PreparedSourceLine &preparedLine,
    const State &initialState,
    const bool isFirstPhysicalLine,
    const int reservedSuffixWidth,
    const bool addEndMark,
    const bool addEllipsis) const -> std::optional<LineBuild> {
    const auto maximumTextWidth = _width - reservedSuffixWidth;
    auto line = Line{};
    line.wrappedFromPrevious = !isFirstPhysicalLine;
    line.indentWidth = calculateIndentWidth(isFirstPhysicalLine);
    line.text = createIndentText(line.indentWidth);
    if (!isFirstPhysicalLine && _leftAligned && !_options.lineBreakStartMark().empty()) {
        line.text += _options.lineBreakStartMark();
    }
    const auto prefixWidth = line.text.displayWidth();
    auto usedWidth = prefixWidth;
    auto state = initialState;
    auto consumedAnySourceCell = false;
    while (state.wordIndex < preparedLine.words.size()) {
        const auto &wordItem = preparedLine.words[state.wordIndex];
        const auto prefixSpacing = evaluatePrefixSpacing(
            wordItem.prefixSpacing, state.spacingOffset, state.tabStopIndex, usedWidth, usedWidth == prefixWidth);
        if (prefixSpacing.action == PrefixSpacing::Action::LineBreak) {
            state.spacingOffset = prefixSpacing.nextSpacingOffset;
            state.tabStopIndex = prefixSpacing.nextTabStopIndex;
            consumedAnySourceCell = true;
            break;
        }
        const auto remainingWord = wordItem.word.substr(state.wordOffset);
        const auto remainingWordWidth = remainingWord.displayWidth();
        const auto availableWidth = maximumTextWidth - usedWidth;
        if (usedWidth + prefixSpacing.width + remainingWordWidth <= maximumTextWidth) {
            line.text += prefixSpacing.text;
            line.text += remainingWord;
            usedWidth += prefixSpacing.width + remainingWordWidth;
            state.wordIndex += 1;
            state.wordOffset = 0;
            state.spacingOffset = 0;
            state.tabStopIndex = prefixSpacing.nextTabStopIndex;
            consumedAnySourceCell = true;
            continue;
        }
        if (usedWidth > prefixWidth) {
            break;
        }
        const auto spacingAvailableWidth = availableWidth - prefixSpacing.width;
        const auto splitWord = buildSplitWord(remainingWord, spacingAvailableWidth, addEndMark);
        if (!splitWord.has_value()) {
            return std::nullopt;
        }
        line.text += prefixSpacing.text;
        line.text += splitWord->text;
        usedWidth += prefixSpacing.width + splitWord->text.displayWidth();
        state.spacingOffset = 0;
        state.tabStopIndex = prefixSpacing.nextTabStopIndex;
        state.wordOffset += splitWord->sourceCharacterCount;
        consumedAnySourceCell = consumedAnySourceCell || splitWord->sourceCharacterCount > 0;
        if (state.wordOffset >= wordItem.word.size()) {
            state.wordIndex += 1;
            state.wordOffset = 0;
            state.spacingOffset = 0;
        }
        break;
    }
    if (!consumedAnySourceCell) {
        return std::nullopt;
    }
    if (addEllipsis && !_options.paragraphEllipsisMark().empty()) {
        line.text += _options.paragraphEllipsisMark();
    }
    if (addEndMark) {
        line.wrapsToNext = true;
        line.endMark = _options.lineBreakEndMark();
    }
    return LineBuild{
        .line = std::move(line), .nextState = state, .consumedAll = state.wordIndex >= preparedLine.words.size()};
}

auto ParagraphLayout::buildSplitWord(const String &word, const int availableWidth, const bool addWordBreakMark) const
    -> std::optional<SplitWordResult> {
    auto markWidth = 0;
    if (addWordBreakMark && _options.wordBreakMark().displayWidth() > 0) {
        markWidth = _options.wordBreakMark().displayWidth();
    }
    if (availableWidth - markWidth <= 0) {
        return std::nullopt;
    }
    auto result = SplitWordResult{};
    auto usedWidth = 0;
    for (std::size_t index = 0; index < word.size(); ++index) {
        const auto characterWidth = word[index].displayWidth();
        if (characterWidth <= 0) {
            continue;
        }
        if (usedWidth + characterWidth > availableWidth - markWidth) {
            break;
        }
        result.text.append(word[index]);
        usedWidth += characterWidth;
        result.sourceCharacterCount = index + 1;
    }
    if (result.sourceCharacterCount == 0) {
        return std::nullopt;
    }
    if (addWordBreakMark && result.sourceCharacterCount < word.size()) {
        result.text.append(_options.wordBreakMark());
    }
    return result;
}

auto ParagraphLayout::evaluatePrefixSpacing(
    const std::vector<SpacingElement> &spacing,
    const std::size_t spacingOffset,
    const std::size_t tabStopIndex,
    int currentColumn,
    const bool isLineStart) const -> PrefixSpacing {
    auto result = PrefixSpacing{
        .text = {},
        .width = 0,
        .nextSpacingOffset = spacingOffset,
        .nextTabStopIndex = tabStopIndex,
        .action = PrefixSpacing::Action::Continue};
    auto ignoreSpaces = isLineStart;
    for (auto index = spacingOffset; index < spacing.size(); ++index) {
        const auto &element = spacing[index];
        if (element.type == SpacingElement::Type::Space) {
            if (ignoreSpaces) {
                result.nextSpacingOffset = index + 1;
                continue;
            }
            result.text.append(spaceFrom(element.character));
            result.width += 1;
            currentColumn += 1;
            result.nextSpacingOffset = index + 1;
            continue;
        }
        ignoreSpaces = false;
        const auto tabStop = resolveTabStop(result.nextTabStopIndex);
        result.nextTabStopIndex += 1;
        result.nextSpacingOffset = index + 1;
        if (tabStop > currentColumn) {
            const auto width = tabStop - currentColumn;
            appendColoredSpaces(result.text, width, element.character.color());
            result.width += width;
            currentColumn = tabStop;
        } else {
            switch (_options.tabOverflowBehavior()) {
            case TabOverflowBehavior::AddSpace:
                result.text.append(spaceFrom(element.character));
                result.width += 1;
                currentColumn += 1;
                break;
            case TabOverflowBehavior::LineBreak:
                if (isLineStart && result.width == 0) {
                    break;
                }
                result.action = PrefixSpacing::Action::LineBreak;
                return result;
            }
        }
    }
    return result;
}

auto ParagraphLayout::resolveTabStop(const std::size_t tabStopIndex) const noexcept -> int {
    if (tabStopIndex >= _options.tabStops().size()) {
        return 0;
    }
    const auto configuredStop = _options.tabStops()[tabStopIndex];
    if (configuredStop == ParagraphOptions::cTabWrappedLineIndent) {
        return _options.wrappedLineIndent();
    }
    return std::max(configuredStop, 0);
}

auto ParagraphLayout::isWordSeparator(const Char &character) const noexcept -> bool {
    return character.codePointCount() == 1 &&
        _options.wordSeparators().find(character.mainCodePoint()) != std::u32string::npos;
}

auto ParagraphLayout::isSpecialTab(const Char &character) const noexcept -> bool {
    return _leftAligned && character == U'\t';
}

auto ParagraphLayout::calculateIndentWidth(const bool isFirstPhysicalLine) const noexcept -> int {
    if (!_leftAligned) {
        return 0;
    }
    return isFirstPhysicalLine ? _options.firstLineIndent() : _options.wrappedLineIndent();
}

auto ParagraphLayout::spaceFrom(const Char &character) -> Char {
    return Char{U' ', character.color()};
}

auto ParagraphLayout::createIndentText(const int indentWidth) -> String {
    auto result = String{};
    appendColoredSpaces(result, indentWidth, Color{Background::Default});
    return result;
}

void ParagraphLayout::appendColoredSpaces(String &text, const int width, const Color color) {
    for (auto i = 0; i < width; ++i) {
        text.append(Char{U' ', color});
    }
}


}
