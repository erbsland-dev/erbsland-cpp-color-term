// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Alignment.hpp"
#include "Char.hpp"
#include "ParagraphBackgroundMode.hpp"
#include "ParagraphOnError.hpp"
#include "String.hpp"
#include "TabOverflowBehavior.hpp"

#include <stdexcept>


namespace erbsland::cterm {

/// Options that control paragraph wrapping, indentation, tab handling, and fallback behavior.
///
/// These settings are used by `Terminal::printParagraph()` and by `TextOptions` / `Text` when text is laid out
/// inside a rectangle.
///
/// @note Creating and copying paragraph options is expensive. Please keep and reuse created instances.
class ParagraphOptions {
public:
    /// Special value that makes `firstLineIndent()` or `wrappedLineIndent()` reuse `lineIndent()`.
    static constexpr auto cUseLineIndent = -1;
    /// Special tab-stop value that resolves to the configured `wrappedLineIndent()`.
    static constexpr auto cTabWrappedLineIndent = -1;

public:
    /// Create paragraph options with the given alignment.
    explicit ParagraphOptions(const Alignment alignment) noexcept : _alignment{alignment} {}

    // defaults
    ParagraphOptions() = default;
    ParagraphOptions(const ParagraphOptions &) = default;
    ParagraphOptions(ParagraphOptions &&) = default;
    auto operator=(const ParagraphOptions &) -> ParagraphOptions & = default;
    auto operator=(ParagraphOptions &&) -> ParagraphOptions & = default;

public:
    /// The alignment of the paragraph.
    /// For the `Terminal::printParagraph` calls, vertical alignment is ignored.
    /// For the `drawText(Text)` calls, the vertical alignment is used to align the text in the given rectangle.
    [[nodiscard]] auto alignment() const noexcept -> Alignment { return _alignment; }
    /// Set the alignment of the paragraph.
    void setAlignment(const Alignment alignment) noexcept { _alignment = alignment; }
    /// The line indent for all lines.
    /// Only valid if the alignment is set to `Alignment::Left`.
    /// This indent can be overridden by `firstLineIndent` and `wrappedLineIndent`.
    [[nodiscard]] auto lineIndent() const noexcept -> int { return _lineIndent; }
    /// Set the line indent for all lines.
    /// @param indent The new indent value. `>=0`
    void setLineIndent(const int indent) noexcept { _lineIndent = std::max(indent, 0); }
    /// Get the first line indent.
    /// This is the indent for the first line of the paragraph.
    /// Only valid if the alignment is set to `Alignment::Left`.
    /// @code
    /// 1:| <first indent> A long text that is broken   |
    /// 2:| into multiple lines.                        |
    /// @endcode
    [[nodiscard]] auto firstLineIndent() const noexcept -> int {
        return _firstLineIndent == cUseLineIndent ? _lineIndent : _firstLineIndent;
    }
    /// Set the first line indent.
    /// @param indent The new indent value. `>=0` or `cUseLineIndent` to use `lineIndent`
    void setFirstLineIndent(const int indent) noexcept { _firstLineIndent = std::max(indent, cUseLineIndent); }
    /// Get the indent for wrapped lines.
    /// This is the indent for all lines that are wrapped at the terminal width.
    /// Only valid if the alignment is set to `Alignment::Left`.
    /// @code
    /// 1:| <first indent> A long text that is broken   |
    /// 2:| into multiple lines.                        |
    /// @endcode
    [[nodiscard]] auto wrappedLineIndent() const noexcept -> int {
        return _wrappedLineIndent == cUseLineIndent ? _lineIndent : _wrappedLineIndent;
    }
    /// Set the indent for wrapped lines.
    /// @param indent The new indent value. `>=0` or `cUseLineIndent` to use `lineIndent`
    void setWrappedLineIndent(const int indent) noexcept { _wrappedLineIndent = std::max(indent, cUseLineIndent); }
    /// Get the background mode.
    /// The background mode determines how the background of the paragraph is handled when lines are wrapped.
    /// It also controls how the background is extended for the last line in the paragraph.
    [[nodiscard]] auto backgroundMode() const noexcept -> ParagraphBackgroundMode { return _backgroundMode; }
    /// Set the background mode.
    void setBackgroundMode(const ParagraphBackgroundMode backgroundMode) noexcept { _backgroundMode = backgroundMode; }
    /// Get the line break end mark.
    /// The line break end mark is appended to the end of a line if it is wrapped to the next line.
    /// The line mark is always aligned to the last column of the terminal.
    /// If the mark contains color information, it will override the background color.
    /// @code
    /// 1:| A long text that is broken  <end mark> |
    /// 2:| into multiple lines.                   |
    /// @endcode
    /// @return The current line break end mark. Empty for no line break end mark.
    [[nodiscard]] auto lineBreakEndMark() const noexcept -> const String & { return _lineBreakEndMark; }
    /// Set the line break end mark.
    /// @param mark The new line break end mark. Must not exceed two characters.
    void setLineBreakEndMark(String mark) {
        if (mark.size() > 2) {
            throw std::invalid_argument{"Line break end mark must not exceed two characters."};
        }
        if (mark.containsControlCharacters()) {
            throw std::invalid_argument{"Line break end mark must not contain control characters."};
        }
        _lineBreakEndMark = std::move(mark);
    }
    /// Get the line break start mark.
    /// The line break start mark prepends each wrapped line.
    /// The mark is inserted at the indentation column.
    /// If the mark contains color information, it will override the background color.
    /// @code
    /// 1:| A long text that is broken           |
    /// 2:| <start mark> into multiple lines.    |
    /// @endcode
    /// @return The current line break start mark. Empty for no line break start mark.
    [[nodiscard]] auto lineBreakStartMark() const noexcept -> const String & { return _lineBreakStartMark; }
    /// Set the line break start mark.
    /// @param mark The new line break start mark. Must not exceed two characters.
    void setLineBreakStartMark(String mark) {
        if (mark.size() > 2) {
            throw std::invalid_argument{"Line break start mark must not exceed two characters."};
        }
        if (mark.containsControlCharacters()) {
            throw std::invalid_argument{"Line break start mark must not contain control characters."};
        }
        _lineBreakStartMark = std::move(mark);
    }
    /// The spacing between paragraphs.
    /// The behavior of the paragraph spacing depends on the used interface.
    /// For the `Terminal::printParagraph` calls, newlines in the text are handled as simple line-breaks.
    /// The paragraph spacing is applied at the end of the call. Either just a newline or two newlines.
    /// For the `drawText(Text)` calls, a newline creates a new paragraph.
    [[nodiscard]] auto paragraphSpacing() const noexcept -> ParagraphSpacing { return _paragraphSpacing; }
    /// Set the paragraph spacing.
    void setParagraphSpacing(const ParagraphSpacing spacing) noexcept { _paragraphSpacing = spacing; }
    /// Get the word separators.
    /// Word separators split a source line into words.
    /// Consecutive separators are rendered as a single space between words.
    /// Tabs use special tab-stop handling in left-aligned paragraphs before separator handling is applied.
    [[nodiscard]] auto wordSeparators() const noexcept -> const std::u32string & { return _wordSeparators; }
    /// Set the word separators.
    /// Tabs remain special tab stops in left-aligned paragraphs and act as regular word separators in
    /// other alignments when included here.
    /// @param separators A string of Unicode characters that are used to split words.
    void setWordSeparators(std::u32string separators) noexcept { _wordSeparators = std::move(separators); }
    /// Get the word break mark.
    /// The word break mark is used when a long word had to be split in a paragraph.
    [[nodiscard]] auto wordBreakMark() const noexcept -> const Char & { return _wordBreakMark; }
    /// Set the word break mark.
    void setWordBreakMark(Char mark) noexcept { _wordBreakMark = mark; }
    /// Get the maximum line wraps.
    /// A value >0 limits the maximum line wraps for a paragraph. A line is only wrapped up to the given
    /// number of times. If after this number, the line would need to wrapped further to be fully displayed
    /// on the screen, the `paragraphEllipsisMark` is used to indicate that the line is incomplete.
    /// Embedded line-breaks are not counted as line wraps and reset the wrap count to zero.
    /// @return The maximum number of line wraps, or zero for unlimited line wraps.
    [[nodiscard]] auto maximumLineWraps() const noexcept -> int { return _maximumLineWraps; }
    /// Set the maximum number of line wraps.
    /// @param lines The maximum number of line wraps, or zero for unlimited line wraps.
    void setMaximumLineWraps(const int lines) noexcept { _maximumLineWraps = std::max(lines, 0); }
    /// Get the paragraph ellipsis mark.
    /// This string is used to indicate that a paragraph would have to be wrapped over even more lines to be
    /// displayed completely.
    /// A single character can be used, but a short text like `(more…)` works as well.
    /// Please note that the width of this mark further reduces the available space for the paragraph text.
    /// @return The paragraph ellipsis mark or an empty string if no ellipsis mark shall be used.
    [[nodiscard]] auto paragraphEllipsisMark() const noexcept -> const String & { return _paragraphEllipsisMark; }
    /// Set the paragraph ellipsis mark.
    /// We recommend using a single character or a very short text. Longer texts quickly make paragraph
    /// rendering impossible.
    /// @param mark The paragraph ellipsis mark. If empty, no ellipsis mark will be used.
    void setParagraphEllipsisMark(String mark) noexcept { _paragraphEllipsisMark = std::move(mark); }
    /// Get the tab stops for the paragraph.
    /// Only valid if the alignment is set to `Alignment::Left`.
    /// If a line (text up to a newline character) contains TAB characters, each tab character will pick the
    /// next tab-stop columns value from this array. If the column is larger than the current column,
    /// spacing is inserted until the cursor reaches the tab-stop column.
    /// If the tab column is smaller, or there is no further tab stop in the sequence, `tabOverflowBehavior()` is
    /// used to decide whether the tab becomes a single space or starts a wrapped continuation line.
    /// In centered or right-aligned paragraphs, tabs use `wordSeparators` instead of these tab stops.
    /// The special value `cTabWrappedLineIndent` can be used to use the same indent as for wrapped lines.
    [[nodiscard]] auto tabStops() const noexcept -> const std::vector<int> & { return _tabStops; }
    /// Set the tab stops.
    void setTabStops(std::vector<int> tabStops) noexcept { _tabStops = std::move(tabStops); }
    /// Get the overflow handling for non-advancing tab stops.
    /// This mode is used if a TAB resolves to a tab-stop column that is not larger than the current column, or if
    /// there is no further configured tab stop.
    [[nodiscard]] auto tabOverflowBehavior() const noexcept -> TabOverflowBehavior { return _tabOverflowBehavior; }
    /// Set the overflow handling for non-advancing tab stops.
    /// @param behavior The behavior to use for tabs that do not advance the current line.
    void setTabOverflowBehavior(const TabOverflowBehavior behavior) noexcept { _tabOverflowBehavior = behavior; }
    /// Get the error resolution if a paragraph cannot be rendered with the given settings.
    /// If the screen layout and the given parameters do not allow the paragraph to be rendered properly,
    /// this error resolution is used.
    /// @return The error resolution to use when a paragraph cannot be rendered.
    [[nodiscard]] auto onError() const noexcept -> ParagraphOnError { return _onError; }
    /// Set the error resolution.
    /// @param onError The error resolution to use when a paragraph cannot be rendered.
    void setOnError(const ParagraphOnError onError) noexcept { _onError = onError; }

public:
    /// Globally shared default options.
    /// @return A reusable default instance with the library defaults for all paragraph settings.
    [[nodiscard]] static auto defaultOptions() noexcept -> const ParagraphOptions &;

private:
    Alignment _alignment{Alignment::TopLeft}; ///< The text alignment.
    int _lineIndent{0};                       ///< Indent for all lines, except the first one and wrapped ones.
    int _firstLineIndent{cUseLineIndent};     ///< Indent for the first line (only for left alignment).
    int _wrappedLineIndent{cUseLineIndent};   ///< Indent for wrapped lines (only for left alignment).
    ParagraphBackgroundMode _backgroundMode{ParagraphBackgroundMode::Default}; ///< The background mode.
    String _lineBreakEndMark{};   ///< String to mark wrapped lines on the right side. Max 2 characters.
    String _lineBreakStartMark{}; ///< String to mark wrapped lines on the left side. Max 2 characters.
    ParagraphSpacing _paragraphSpacing{ParagraphSpacing::SingleLine}; ///< Spacing between paragraphs.
    std::u32string _wordSeparators{U" \t"};                           ///< Word separators.
    Char _wordBreakMark{U"-"};                         ///< The mark added if a long word needs to be broken.
    int _maximumLineWraps{0};                          ///< The maximum number of lines in a paragraph.
    String _paragraphEllipsisMark{U"…"};               ///< The ellipsis mark for the paragraph.
    std::vector<int> _tabStops{cTabWrappedLineIndent}; ///< Tab stops for TAB character.
    TabOverflowBehavior _tabOverflowBehavior{TabOverflowBehavior::AddSpace}; ///< Fallback for non-advancing tabs.
    ParagraphOnError _onError{ParagraphOnError::PlainOutput};                ///< Error handling mode for paragraphs.
};


}
