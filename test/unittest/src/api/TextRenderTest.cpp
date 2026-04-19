// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "BufferTestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(Buffer)
class TextRenderTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testBufferRendersAlignedText() {
        auto buffer = Buffer{Size{8, 3}};
        buffer.drawText("Hi", Rectangle{0, 0, 8, 3}, Alignment::Center, Color{fg::Yellow, bg::Black});
        REQUIRE_EQUAL(buffer.get(Position{3, 1}), U'H');
        REQUIRE_EQUAL(buffer.get(Position{4, 1}), U'i');
        REQUIRE_EQUAL(buffer.get(Position{3, 1}).color(), Color(fg::Yellow, bg::Black));
    }

    void testBufferRendersNewParagraphsWithSingleParagraphSpacing() {
        auto text = Text{String{"A\nB"}, Rectangle{0, 0, 1, 2}, Alignment::TopLeft};
        auto buffer = Buffer{Size{1, 2}};
        buffer.drawText(text);
        REQUIRE_EQUAL(buffer.get(Position{0, 0}), U'A');
        REQUIRE_EQUAL(buffer.get(Position{0, 1}), U'B');
    }

    void testBufferDoesNotInsertSpacingBetweenWrappedLines() {
        auto text = Text{String{"AA BB"}, Rectangle{0, 0, 2, 2}, Alignment::TopLeft};
        text.setParagraphSpacing(ParagraphSpacing::DoubleLine);
        auto buffer = Buffer{Size{2, 2}};
        buffer.drawText(text);
        REQUIRE_EQUAL(buffer.get(Position{0, 0}), U'A');
        REQUIRE_EQUAL(buffer.get(Position{1, 0}), U'A');
        REQUIRE_EQUAL(buffer.get(Position{0, 1}), U'B');
        REQUIRE_EQUAL(buffer.get(Position{1, 1}), U'B');
    }

    void testBufferRendersDoubleParagraphSpacingWithVerticalAlignment() {
        auto text = Text{String{"A\nB"}, Rectangle{0, 0, 3, 5}, Alignment::Center};
        text.setParagraphSpacing(ParagraphSpacing::DoubleLine);
        auto buffer = Buffer{Size{3, 5}};
        buffer.drawText(text);
        REQUIRE_EQUAL(buffer.get(Position{1, 1}), U'A');
        REQUIRE_EQUAL(buffer.get(Position{1, 3}), U'B');
        REQUIRE_EQUAL(buffer.get(Position{1, 2}), U' ');
    }

    void testBufferKeepsCharacterColorsIfNoColorSequenceIsDefined() {
        auto text = String{};
        text.append(Char{U'A', fg::Red, bg::Black});
        text.append(Char{U'B', fg::Green, bg::Black});
        auto buffer = Buffer{Size{4, 1}};
        buffer.drawText(Text{text, Rectangle{0, 0, 4, 1}, Alignment::TopLeft});
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Red, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Green, bg::Black));
    }

    void testBufferUsesExplicitAnimationCycleForFlashAnimation() {
        auto buffer = Buffer{Size{3, 1}};
        auto text = Text{String{"ABC"}, Rectangle{0, 0, 3, 1}, Alignment::TopLeft};
        text.setColorSequence(
            ColorSequence{Color{fg::Red, bg::Black}, Color{fg::Green, bg::Black}, Color{fg::Blue, bg::Black}});
        text.setAnimation(TextAnimation::ColorDiagonal);
        buffer.drawText(text, 1);
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Green, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Blue, bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{2, 0}).color(), Color(fg::Red, bg::Black));
    }

    void testBufferKeepsCharacterColorPartsOverTextColor() {
        auto text = String{};
        text.append(Char{U'A', fg::Red, bg::Inherited});
        text.append(Char{U'B', fg::Inherited, bg::Blue});
        auto renderedText = Text{text, Rectangle{0, 0, 2, 1}, Alignment::TopLeft};
        renderedText.setColor(Color{fg::Green, bg::Yellow});
        auto buffer = Buffer{Size{2, 1}};
        buffer.drawText(renderedText);
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Red, bg::Yellow));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Green, bg::Blue));
    }

    void testBufferUsesDefaultAsExplicitTextReset() {
        auto text = String{};
        text.append(Char{U'A', fg::Default, bg::Inherited});
        auto renderedText = Text{text, Rectangle{0, 0, 1, 1}, Alignment::TopLeft};
        renderedText.setColor(Color{fg::Green, bg::Yellow});
        auto buffer = Buffer{Size{1, 1}};
        buffer.drawText(renderedText);

        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Default, bg::Yellow));
    }

    void testBufferRespectsParagraphMarginsWhenRenderingText() {
        auto text = Text{String{"A"}, Rectangle{0, 0, 5, 3}, Alignment::TopLeft};
        text.setMargins(Margins{1});
        auto buffer = Buffer{Size{5, 3}};

        buffer.drawText(text);

        REQUIRE_EQUAL(buffer.get(Position{1, 1}), U'A');
        REQUIRE_EQUAL(buffer.get(Position{0, 0}), U' ');
        REQUIRE_EQUAL(buffer.get(Position{4, 2}), U' ');
    }

    void testBufferRendersBitmapFontText() {
        auto font = std::make_shared<Font>(2);
        font->addGlyph("A", FontGlyph{std::vector<uint64_t>{0b11U, 0b11U}});
        auto text = Text{String{"A"}, Rectangle{0, 0, 2, 1}, Alignment::TopLeft};
        text.setColor(Color{fg::BrightWhite, bg::Black});
        text.setFont(font);
        auto buffer = Buffer{Size{2, 1}};
        buffer.drawText(text);
        REQUIRE_EQUAL(buffer.get(Position{0, 0}), U'█');
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::BrightWhite, bg::Black));
    }

    void testBufferRendersParagraphIndentAndWrapMarks() {
        auto text = Text{String{"AA BB CC"}, Rectangle{0, 0, 6, 2}, Alignment::TopLeft};
        text.setWrappedLineIndent(3);
        text.setLineBreakStartMark(String{">"});
        text.setLineBreakEndMark(String{"<"});
        auto buffer = Buffer{Size{6, 2}};

        buffer.drawText(text);

        requireRowsEqual(buffer, {"AA BB<", "   >CC"});
    }

    void testBufferUsesTabStopsForParagraphRendering() {
        auto text = Text{String{"A\tB"}, Rectangle{0, 0, 5, 1}, Alignment::TopLeft};
        text.setTabStops({4});
        auto buffer = Buffer{Size{5, 1}};

        buffer.drawText(text);

        requireRowsEqual(buffer, {"A   B"});
    }

    void testBufferBreaksAtNonAdvancingTabStopsWhenRequested() {
        auto text = Text{String{"Heading\ttext"}, Rectangle{0, 0, 12, 2}, Alignment::TopLeft};
        text.setWrappedLineIndent(6);
        text.setLineBreakEndMark(String{"<"});
        text.setTabStops({6});
        text.setTabOverflowBehavior(TabOverflowBehavior::LineBreak);
        auto buffer = Buffer{Size{12, 2}};

        buffer.drawText(text);

        requireRowsEqual(buffer, {"Heading    <", "      text  "});
    }

    void testBufferReplacesNonAdvancingTabsWithSpacesByDefault() {
        auto text = Text{String{"Heading\ttext"}, Rectangle{0, 0, 12, 1}, Alignment::TopLeft};
        text.setTabStops({6});
        auto buffer = Buffer{Size{12, 1}};

        buffer.drawText(text);

        requireRowsEqual(buffer, {"Heading text"});
    }

    void testBufferBreaksWhenTabStopsAreExhaustedAndLineBreakIsRequested() {
        auto text = Text{String{"A\tB\tC"}, Rectangle{0, 0, 5, 2}, Alignment::TopLeft};
        text.setWrappedLineIndent(2);
        text.setLineBreakEndMark(String{"<"});
        text.setTabStops({2});
        text.setTabOverflowBehavior(TabOverflowBehavior::LineBreak);
        auto buffer = Buffer{Size{5, 2}};

        buffer.drawText(text);

        requireRowsEqual(buffer, {"A B <", "  C  "});
    }

    void testBufferUsesWordSeparatorsAsCollapsedSpacing() {
        auto text = Text{String{"A..B"}, Rectangle{0, 0, 3, 1}, Alignment::TopLeft};
        text.setWordSeparators(U".");
        auto buffer = Buffer{Size{3, 1}};

        buffer.drawText(text);

        requireRowsEqual(buffer, {"A B"});
    }

    void testBufferTreatsTabsAsCollapsedWordSeparatorsForCenteredParagraphs() {
        auto text = Text{String{"A\tB"}, Rectangle{0, 0, 5, 1}, Alignment::Center};
        auto buffer = Buffer{Size{5, 1}};

        buffer.drawText(text);

        requireRowsEqual(buffer, {" A B "});
    }

    void testBufferSplitsLongWordsUsingTheConfiguredWordBreakMark() {
        auto text = Text{String{"ABCDEFG"}, Rectangle{0, 0, 5, 2}, Alignment::TopLeft};
        auto buffer = Buffer{Size{5, 2}};

        buffer.drawText(text);

        requireRowsEqual(buffer, {"ABCD-", "EFG  "});
    }

    void testBufferUsesParagraphEllipsisAfterMaximumWraps() {
        auto text = Text{String{"AA BB CC DD EE"}, Rectangle{0, 0, 5, 2}, Alignment::TopLeft};
        text.setMaximumLineWraps(1);
        auto buffer = Buffer{Size{5, 2}};

        buffer.drawText(text);

        requireRowsEqual(buffer, {"AA BB", "CC…  "});
    }

    void testBufferUsesParagraphBackgroundModesForWrappedParagraphs() {
        auto text = Text{String{"AAAA BBBB"}, Rectangle{0, 0, 6, 2}, Alignment::TopLeft};
        text.setColor(Color{fg::White, bg::Red});
        text.setWrappedLineIndent(2);
        text.setBackgroundMode(ParagraphBackgroundMode::WrappedBoth);
        auto buffer = Buffer{Size{6, 2}, Char{U' ', fg::White, bg::Blue}};

        buffer.drawText(text);

        REQUIRE_EQUAL(buffer.get(Position{4, 0}).color(), Color(fg::White, bg::Red));
        REQUIRE_EQUAL(buffer.get(Position{5, 0}).color(), Color(fg::White, bg::Red));
        REQUIRE_EQUAL(buffer.get(Position{0, 1}).color(), Color(fg::White, bg::Red));
        REQUIRE_EQUAL(buffer.get(Position{1, 1}).color(), Color(fg::White, bg::Red));
    }

    void testBufferFallsBackToTheLegacySimpleDrawForInvalidParagraphSettings() {
        auto text = Text{String{"AA BB"}, Rectangle{0, 0, 2, 2}, Alignment::TopLeft};
        text.setLineBreakEndMark(String{">>"});
        text.setOnError(ParagraphOnError::PlainOutput);
        auto buffer = Buffer{Size{2, 2}};

        buffer.drawText(text);

        requireRowsEqual(buffer, {"AA", "BB"});
    }

    void testBufferSkipsInvalidParagraphsWhenOnErrorIsEmpty() {
        auto text = Text{String{"AA BB"}, Rectangle{0, 0, 2, 2}, Alignment::TopLeft};
        text.setLineBreakEndMark(String{">>"});
        text.setOnError(ParagraphOnError::Empty);
        auto buffer = Buffer{Size{2, 2}, Char{U'X', fg::White, bg::Blue}};

        buffer.drawText(text);

        requireRowsEqual(buffer, {"XX", "XX"});
    }
};
