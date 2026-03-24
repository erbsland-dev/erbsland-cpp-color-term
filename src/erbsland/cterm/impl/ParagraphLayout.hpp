// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../ParagraphOptions.hpp"
#include "../String.hpp"

#include <cstdint>
#include <optional>
#include <vector>


namespace erbsland::cterm::impl {


/// Shared paragraph layout for wrapped terminal text.
class ParagraphLayout final {
public:
    /// How newline characters in the input string are interpreted.
    enum class NewlineMode : uint8_t {
        ParagraphBreak, ///< Each newline starts a new paragraph.
        HardLineBreak,  ///< Each newline inserts a hard line break inside one paragraph.
    };

    /// One rendered physical line in the final paragraph layout.
    struct Line final {
        String text;                      ///< The rendered line text without the end mark aligned to the right edge.
        String endMark;                   ///< Optional mark aligned to the right edge on wrapped lines.
        int indentWidth = 0;              ///< Width of the indentation area before the first rendered mark/text.
        bool wrappedFromPrevious = false; ///< `true` if this line is a wrapped continuation.
        bool wrapsToNext = false;         ///< `true` if this line wraps to the next physical line.
    };

    /// Full paragraph layout result.
    struct Result final {
        bool valid = true;       ///< `false` if the paragraph settings make rendering impossible.
        std::vector<Line> lines; ///< The rendered physical lines.
    };

public:
    ParagraphLayout(const String &text, int width, const ParagraphOptions &options, NewlineMode newlineMode) noexcept;
    ~ParagraphLayout() = default;
    ParagraphLayout(const ParagraphLayout &) = delete;
    ParagraphLayout(ParagraphLayout &&) = delete;
    auto operator=(const ParagraphLayout &) -> ParagraphLayout & = delete;
    auto operator=(ParagraphLayout &&) -> ParagraphLayout & = delete;

    /// Build the wrapped paragraph layout.
    /// @return The layout result.
    [[nodiscard]] auto build() -> Result;

private:
    struct SpacingElement final {
        enum class Type : uint8_t { Space, Tab };

        Type type = Type::Space;
        Char character{U' '};
    };

    struct WordItem final {
        std::vector<SpacingElement> prefixSpacing;
        String word;
    };

    struct State final {
        std::size_t wordIndex = 0;
        std::size_t wordOffset = 0;
        std::size_t spacingOffset = 0;
        std::size_t tabStopIndex = 0;
    };

    struct PreparedSourceLine final {
        std::vector<WordItem> words;
    };

    struct PrefixSpacing final {
        enum class Action : uint8_t {
            Continue,
            LineBreak,
        };

        String text;
        int width = 0;
        std::size_t nextSpacingOffset = 0;
        std::size_t nextTabStopIndex = 0;
        Action action = Action::Continue;
    };

    struct LineBuild final {
        Line line;
        State nextState;
        bool consumedAll = false;
    };

    struct SplitWordResult final {
        String text;
        std::size_t sourceCharacterCount = 0;
    };

private:
    [[nodiscard]] auto splitInputIntoParagraphs() const -> std::vector<std::vector<String>>;
    [[nodiscard]] static auto splitIntoSourceLines(const String &text) -> std::vector<String>;
    [[nodiscard]] auto layoutParagraph(const std::vector<String> &sourceLines, std::vector<Line> &lines) -> bool;
    [[nodiscard]] auto layoutSourceLine(const String &sourceLine, std::vector<Line> &lines) -> bool;
    [[nodiscard]] auto prepareSourceLine(const String &sourceLine) const -> PreparedSourceLine;
    static void finishCurrentWord(
        String &currentWord,
        std::vector<SpacingElement> &pendingSpacing,
        std::optional<Char> &pendingWordSeparator,
        std::vector<WordItem> &words,
        bool &hasWords);
    static void removeLeadingSpaces(std::vector<SpacingElement> &spacing);
    [[nodiscard]] auto
    tryBuildLastLine(const PreparedSourceLine &preparedLine, const State &initialState, bool isFirstPhysicalLine) const
        -> std::optional<Line>;
    [[nodiscard]] auto
    buildWrappedLine(const PreparedSourceLine &preparedLine, const State &initialState, bool isFirstPhysicalLine) const
        -> std::optional<LineBuild>;
    [[nodiscard]] auto buildTruncatedLine(
        const PreparedSourceLine &preparedLine, const State &initialState, bool isFirstPhysicalLine) const
        -> std::optional<Line>;
    [[nodiscard]] auto buildLine(
        const PreparedSourceLine &preparedLine,
        const State &initialState,
        bool isFirstPhysicalLine,
        int reservedSuffixWidth,
        bool addEndMark,
        bool addEllipsis) const -> std::optional<LineBuild>;
    [[nodiscard]] auto buildSplitWord(const String &word, int availableWidth, bool addWordBreakMark) const
        -> std::optional<SplitWordResult>;
    [[nodiscard]] auto evaluatePrefixSpacing(
        const std::vector<SpacingElement> &spacing,
        std::size_t spacingOffset,
        std::size_t tabStopIndex,
        int currentColumn,
        bool isLineStart) const -> PrefixSpacing;
    [[nodiscard]] auto resolveTabStop(std::size_t tabStopIndex) const noexcept -> int;
    [[nodiscard]] auto isWordSeparator(const Char &character) const noexcept -> bool;
    [[nodiscard]] auto isSpecialTab(const Char &character) const noexcept -> bool;
    [[nodiscard]] auto calculateIndentWidth(bool isFirstPhysicalLine) const noexcept -> int;
    [[nodiscard]] static auto spaceFrom(const Char &character) -> Char;
    [[nodiscard]] static auto createIndentText(int indentWidth) -> String;
    static void appendColoredSpaces(String &text, int width, Color color);

private:
    const String &_text;
    int _width;
    const ParagraphOptions &_options;
    NewlineMode _newlineMode;
    bool _leftAligned;
};


}
