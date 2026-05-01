// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "LayoutHelper.hpp"

#include <vector>

namespace erbsland::cterm::theme::layout {

auto stylePaddingAndMarginsImpl(const StringView &text, const ThemeAccessor &themeAccessor) noexcept
    -> StringWithMargins {
    const auto margins = themeAccessor.margins();
    const auto padding = themeAccessor.padding();
    if (padding.left() == 0 && padding.right() == 0) {
        return StringWithMargins{String{text}, margins};
    }
    const auto paddingLeftBlock = themeAccessor.block(BlockRole::LeftPadding);
    const auto paddingRightBlock = themeAccessor.block(BlockRole::RightPadding);
    auto result = String{};
    result.append(static_cast<std::size_t>(padding.left()), paddingLeftBlock);
    result.append(text);
    result.append(static_cast<std::size_t>(padding.right()), paddingRightBlock);
    return StringWithMargins{result, margins};
}

auto stylePaddingAndMargins(const StringView &text, const ThemeAccessor &themeAccessor, Part textPart) noexcept
    -> StringWithMargins {
    const auto textTheme = textPart != Part::None ? themeAccessor.forPart(textPart) : themeAccessor;
    return stylePaddingAndMarginsImpl(text.withBase(textTheme.style()), textTheme);
}

auto stylePaddingAndMargins(const std::string &text, const ThemeAccessor &themeAccessor, Part textPart) noexcept
    -> StringWithMargins {
    const auto textTheme = textPart != Part::None ? themeAccessor.forPart(textPart) : themeAccessor;
    return stylePaddingAndMarginsImpl(String{text, textTheme.style()}, textTheme);
}

auto stylePaddingCropAndMargins(
    const StringView &text,
    Coordinate displayWidth,
    const ThemeAccessor &themeAccessor,
    const Part textPart,
    const Part ellipsisPart,
    const Alignment textAlignmentForEllipsis) noexcept -> StringWithMargins {

    const auto textTheme = textPart != Part::None ? themeAccessor.forPart(textPart) : themeAccessor;
    const auto ellipsisTheme = ellipsisPart != Part::None ? themeAccessor.forPart(ellipsisPart) : themeAccessor;
    const auto margins = textTheme.margins();
    if (text.empty() || displayWidth <= 0) {
        return StringWithMargins{{}, margins};
    }
    const auto padding = textTheme.padding();
    const auto displayWidthWithPadding = text.displayWidth() + padding.horizontalExtent();
    if (displayWidthWithPadding <= displayWidth) {
        return stylePaddingAndMarginsImpl(text, textTheme);
    }
    const auto targetWidth = displayWidth - 1 - padding.horizontalExtent();
    if (targetWidth < 0) {
        return StringWithMargins{{}, margins};
    }
    const auto paddingLeftBlock = textTheme.block(BlockRole::LeftPadding);
    const auto paddingRightBlock = textTheme.block(BlockRole::RightPadding);
    const auto ellipsisBlock = ellipsisTheme.block(BlockRole::Main);
    if (targetWidth == 0) {
        auto result = String{};
        result.append(static_cast<std::size_t>(padding.left()), paddingLeftBlock);
        result.append(ellipsisBlock);
        result.append(static_cast<std::size_t>(padding.right()), paddingRightBlock);
        return StringWithMargins{result, margins};
    }
    auto result = String{};
    result.append(static_cast<std::size_t>(padding.left()), paddingLeftBlock);
    if (textAlignmentForEllipsis.isLeft()) {
        result.append(text.croppedToDisplayWidth(targetWidth, textAlignmentForEllipsis));
        result.append(ellipsisBlock);
    } else if (textAlignmentForEllipsis.isRight()) {
        result.append(ellipsisBlock);
        result.append(text.croppedToDisplayWidth(targetWidth, textAlignmentForEllipsis));
    } else {
        const auto leftTargetWidth = targetWidth / 2;
        const auto rightTargetWidth = targetWidth - leftTargetWidth;
        result.append(text.croppedToDisplayWidth(leftTargetWidth, Alignment::Left));
        result.append(ellipsisBlock);
        result.append(text.croppedToDisplayWidth(rightTargetWidth, Alignment::Right));
    }
    result.append(static_cast<std::size_t>(padding.right()), paddingRightBlock);
    return StringWithMargins{result, margins};
}

template <typename tString>
    requires std::is_same_v<std::remove_cvref_t<tString>, std::string> ||
    std::is_same_v<std::remove_cvref_t<tString>, String> || std::is_same_v<std::remove_cvref_t<tString>, StringView>
auto encloseInBracketsImpl(
    std::span<tString> texts, const ThemeAccessor &themeAccessor, const Part bracketPart, const Part textPart) noexcept
    -> StringWithMargins {

    if (texts.empty()) {
        return StringWithMargins{};
    }
    const auto bracketTheme = bracketPart != Part::None ? themeAccessor.forPart(bracketPart) : themeAccessor;
    const auto bracketPadding = bracketTheme.padding();
    const auto textTheme = textPart != Part::None ? themeAccessor.forPart(textPart) : themeAccessor;
    const auto textStyle = textPart != Part::None ? textTheme.style() : CharStyle{};
    const auto textPadding = textPart != Part::None ? textTheme.padding() : Margins{};
    const auto textLeftPaddingBlock = textTheme.block(BlockRole::LeftPadding);
    const auto textRightPaddingBlock = textTheme.block(BlockRole::RightPadding);
    const auto leftBracketBlock = bracketTheme.block(BlockRole::LeftBracket);
    const auto middleBracketBlock = bracketTheme.block(BlockRole::MiddleBracket);
    const auto rightBracketBlock = bracketTheme.block(BlockRole::RightBracket);
    const auto leftBracketPaddingBlock = bracketTheme.block(BlockRole::LeftOuterPadding);
    const auto rightBracketPaddingBlock = bracketTheme.block(BlockRole::RightOuterPadding);
    const auto leftInnerPaddingBlock = bracketTheme.block(BlockRole::LeftInnerPadding);
    const auto rightInnerPaddingBlock = bracketTheme.block(BlockRole::RightInnerPadding);
    auto result = String{};
    result.append(leftBracketBlock);
    result.append(static_cast<std::size_t>(bracketPadding.left()), leftBracketPaddingBlock);
    for (std::size_t i = 0; i < texts.size(); ++i) {
        result.append(static_cast<std::size_t>(textPadding.left()), textLeftPaddingBlock);
        result.appendStyled(texts[i], textStyle);
        result.append(static_cast<std::size_t>(textPadding.right()), textRightPaddingBlock);
        if (i < texts.size() - 1) {
            result.append(static_cast<std::size_t>(bracketPadding.right()), rightInnerPaddingBlock);
            result.append(middleBracketBlock);
            result.append(static_cast<std::size_t>(bracketPadding.left()), leftInnerPaddingBlock);
        }
    }
    result.append(static_cast<std::size_t>(bracketPadding.right()), rightBracketPaddingBlock);
    result.append(rightBracketBlock);
    const auto margins = bracketTheme.margins().expandedWith(textTheme.margins());
    return StringWithMargins{result, margins};
}

auto encloseInBrackets(
    std::span<String> texts, const ThemeAccessor &themeAccessor, const Part bracketPart, const Part textPart) noexcept
    -> StringWithMargins {
    return encloseInBracketsImpl(texts, themeAccessor, bracketPart, textPart);
}

auto encloseInBrackets(
    std::span<StringView> texts,
    const ThemeAccessor &themeAccessor,
    const Part bracketPart,
    const Part textPart) noexcept -> StringWithMargins {
    return encloseInBracketsImpl(texts, themeAccessor, bracketPart, textPart);
}

auto encloseInBrackets(
    std::span<std::string> texts,
    const ThemeAccessor &themeAccessor,
    const Part bracketPart,
    const Part textPart) noexcept -> StringWithMargins {
    return encloseInBracketsImpl(texts, themeAccessor, bracketPart, textPart);
}

auto encloseInBrackets(
    const std::vector<std::string> &texts,
    const ThemeAccessor &themeAccessor,
    const Part bracketPart,
    const Part textPart) noexcept -> StringWithMargins {
    return encloseInBracketsImpl(std::span{texts.begin(), texts.size()}, themeAccessor, bracketPart, textPart);
}

auto encloseInBrackets(
    const StringView &text, const ThemeAccessor &themeAccessor, const Part bracketPart, const Part textPart) noexcept
    -> StringWithMargins {
    return encloseInBracketsImpl(std::span{&text, 1}, themeAccessor, bracketPart, textPart);
}

auto encloseInBrackets(
    std::string text, const ThemeAccessor &themeAccessor, const Part bracketPart, const Part textPart) noexcept
    -> StringWithMargins {
    return encloseInBracketsImpl(std::span{&text, 1}, themeAccessor, bracketPart, textPart);
}

}
