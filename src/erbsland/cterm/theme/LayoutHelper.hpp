// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "StringWithMargins.hpp"
#include "ThemeAccessor.hpp"

namespace erbsland::cterm::theme::layout {

/// Apply padding and margins to a string.
/// @param text The text to style and surround with padding.
/// @param themeAccessor The theme accessor to use for styling (selects element, states, and tags).
/// @param textPart Optional part to use for the text styling, padding, and margins.
///    If Part::None is passed, the themeAccessor is used as part theme.
/// @return The styled text with padding applied, and margins returned.
auto stylePaddingAndMargins(
    const StringView &text, const ThemeAccessor &themeAccessor, Part textPart = Part::None) noexcept
    -> StringWithMargins;
/// @overload
auto stylePaddingAndMargins(
    const std::string &text, const ThemeAccessor &themeAccessor, Part textPart = Part::None) noexcept
    -> StringWithMargins;

/// Apply style, padding, margins, and crop to fit to a given width.
/// @param text The text to style and surround with padding.
/// @param displayWidth The maximum width the text, including the ellipsis, will be cropped to.
/// @param themeAccessor The theme accessor to use for styling (selects element, states, and tags).
/// @param textPart Part to use for the text styling, padding, and margins.
///     If Part::None is passed, the themeAccessor is used as part theme.
/// @param ellipsisPart Part to use for the ellipsis block.
///     If Part::None is passed, the themeAccessor is used as part theme.
/// @param textAlignmentForEllipsis The text alignment for the ellipsis block.
///     If the text is left aligned, the ellipsis will be placed at the end of the text.
///     If the text is right aligned, the ellipsis will be placed at the beginning of the text.
///     If the text is center aligned, the ellipsis will be placed in the middle of the text.
///     Only the `Left`, `HCenter`, and `Right` alignments are tested. Vertical alignments are ignored.
auto stylePaddingCropAndMargins(
    const StringView &text,
    Coordinate displayWidth,
    const ThemeAccessor &themeAccessor,
    Part textPart = Part::Text,
    Part ellipsisPart = Part::Ellipsis,
    Alignment textAlignmentForEllipsis = Alignment::Left) noexcept -> StringWithMargins;

/// Enclose a sequence of text parts in brackets using the specified theme accessors.
/// This will create a string using this structure:
/// <code>
/// Elements:
///     LB: left bracket
///     MB: middle bracket (separator)
///     RB: right bracket
///     Text: one of the text elements
/// Attributes:
///     LM: left margin     RM: right margin
///     LP: left padding    RP: right padding
///     (padding always rendered as spaces in the given theme char style)
/// Theme style:
///     [B: ...] bracketPart
///     [T: ...] textPart
///
/// [B:LM] collapsed with [T:LM] -> returned as left margin
/// [B: LB + LP]
///     [T: LP + Text + RP]
/// [B: RP + MB + LP]
///     [T: LP + Text + RP]
/// [B: RP + RB]
/// [B:RM] collapsed with [T:RM] -> returned as right margin
/// </code>
/// @param texts The text to join with surrounding brackets.
/// @param themeAccessor The theme accessor to use for styling (selects element, states and tags).
/// @param bracketPart The part to use for the bracket styling.
/// @param textPart The part to use for the text styling. If set to Part::None, no text styling is applied.
/// @return The joined text with surrounding brackets, styled according to the theme accessor.
auto encloseInBrackets(
    std::span<String> texts, const ThemeAccessor &themeAccessor, Part bracketPart, Part textPart) noexcept
    -> StringWithMargins;
/// @overload
auto encloseInBrackets(
    std::span<StringView> texts, const ThemeAccessor &themeAccessor, Part bracketPart, Part textPart) noexcept
    -> StringWithMargins;
/// @overload
auto encloseInBrackets(
    std::span<std::string> texts, const ThemeAccessor &themeAccessor, Part bracketPart, Part textPart) noexcept
    -> StringWithMargins;
/// @overload
auto encloseInBrackets(
    const std::vector<std::string> &texts, const ThemeAccessor &themeAccessor, Part bracketPart, Part textPart) noexcept
    -> StringWithMargins;
/// @overload
auto encloseInBrackets(
    const StringView &text, const ThemeAccessor &themeAccessor, Part bracketPart, Part textPart) noexcept
    -> StringWithMargins;
/// @overload
auto encloseInBrackets(std::string text, const ThemeAccessor &themeAccessor, Part bracketPart, Part textPart) noexcept
    -> StringWithMargins;

}
