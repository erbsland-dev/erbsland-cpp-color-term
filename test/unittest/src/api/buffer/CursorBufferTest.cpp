// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/BufferTestHelper.hpp"

#include <erbsland/cterm/CursorBuffer.hpp>
#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(CursorBuffer)
class CursorBufferTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testDefaultConstructionCreatesAn80x25Buffer() {
        const auto buffer = CursorBuffer{};

        REQUIRE_EQUAL(buffer.size(), (Size{80, 25}));
        REQUIRE_EQUAL(buffer.color(), Color::reset());
        REQUIRE_EQUAL(buffer.fillChar(), Char::space());
    }

    void testConstructorUsesFillCharForTheInitialBufferContents() {
        const auto fillChar = Char{U'.', fg::BrightBlack, bg::Black};
        const auto buffer =
            CursorBuffer{Size{3, 2}, CursorBuffer::OverflowMode::Shift, CursorBuffer::cMaximumSize, fillChar};

        REQUIRE_EQUAL(buffer.fillChar(), fillChar);
        REQUIRE_EQUAL(buffer.get(Position{0, 0}), fillChar);
        REQUIRE_EQUAL(buffer.get(Position{2, 1}), fillChar);
    }

    void testColorSettersResolveInheritedColorsAndApplyToWrittenCharacters() {
        auto buffer = CursorBuffer{Size{3, 1}};
        const auto expectedAttributes = CharAttributes::reset();

        buffer.setColor(Color{fg::Inherited, bg::Inherited});
        REQUIRE_EQUAL(buffer.color(), Color::reset());

        buffer.setForeground(fg::Red);
        buffer.setBackground(bg::Blue);
        buffer.write(Char{U'X', fg::Inherited, bg::Inherited});

        REQUIRE_EQUAL(buffer.color(), Color(fg::Red, bg::Blue));
        REQUIRE_EQUAL(buffer.get(Position{0, 0}), (Char{U'X', Color{fg::Red, bg::Blue}, expectedAttributes}));
    }

    void testCharAttributesResolveAgainstTheWriterStateAndCanBeOverwrittenExplicitly() {
        auto buffer = CursorBuffer{Size{3, 1}};
        auto bold = CharAttributes{};
        bold.setBold(true);
        buffer.setCharAttributes(bold);

        buffer.write(Char{U'A'});

        auto explicitNoBold = CharAttributes{};
        explicitNoBold.setBold(false);
        explicitNoBold.setUnderline(true);
        buffer.write(Char{U'B', Color{}, explicitNoBold});

        REQUIRE(buffer.charAttributes().isBold());
        REQUIRE(buffer.supportedCharAttributes() == CharAttributes::all());
        REQUIRE(buffer.get(Position{0, 0}).attributes().isBold());
        REQUIRE_FALSE(buffer.get(Position{1, 0}).attributes().isBold());
        REQUIRE(buffer.get(Position{1, 0}).attributes().isUnderline());
    }

    void testWritingAStringResolvesInheritedStyleAgainstTheTrackedWriterStateForEachCharacter() {
        auto buffer = CursorBuffer{Size{2, 1}};
        auto underline = CharAttributes{};
        underline.setUnderline(true);
        buffer.setStyle(CharStyle{Color{fg::Red, bg::Blue}, underline});

        auto text = String{};
        text += Char{U'A', Color{fg::Green, bg::Inherited}, CharAttributes{}.withFlag(CharAttributes::Bold, true)};
        text += Char{U'B'};
        buffer.write(text);

        REQUIRE_EQUAL(buffer.color(), Color(fg::Red, bg::Blue));
        REQUIRE(buffer.charAttributes().isUnderline());
        REQUIRE_FALSE(buffer.charAttributes().isBold());
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Green, bg::Blue));
        REQUIRE(buffer.get(Position{0, 0}).attributes().isBold());
        REQUIRE(buffer.get(Position{0, 0}).attributes().isUnderline());
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Red, bg::Blue));
        REQUIRE_FALSE(buffer.get(Position{1, 0}).attributes().isBold());
        REQUIRE(buffer.get(Position{1, 0}).attributes().isUnderline());
    }

    void testWritingAStringViewUsesTheSameInheritedStyleResolution() {
        auto buffer = CursorBuffer{Size{2, 1}};
        buffer.setColor(Color{fg::Green, bg::Blue});
        const auto source = String{"AB"};

        buffer.write(StringView{source}.substr(0, 2));

        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Green, bg::Blue));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Green, bg::Blue));
    }

    void testPrintAcceptsCharacterAttributesAndAppliesThemToFollowingCharacters() {
        auto buffer = CursorBuffer{Size{3, 1}};
        auto bold = CharAttributes{};
        bold.setBold(true);
        auto noBold = CharAttributes{};
        noBold.setBold(false);

        buffer.print(bold, "A", noBold, "B");

        REQUIRE(buffer.get(Position{0, 0}).attributes().isBold());
        REQUIRE_FALSE(buffer.get(Position{1, 0}).attributes().isBold());
    }

    void testSetStyleAndPrintCharStyleUpdateTheTrackedWriterState() {
        auto buffer = CursorBuffer{Size{3, 1}};
        auto emphasis = CharAttributes{};
        emphasis.setBold(true);

        buffer.setStyle(CharStyle{Color{fg::Red, bg::Blue}, emphasis});
        buffer.write(Char{U'A'});
        buffer.print(
            CharStyle{Color{fg::Inherited, bg::Green}, CharAttributes{}.withFlag(CharAttributes::Bold, false)}, "B");

        REQUIRE_EQUAL(buffer.color(), Color(fg::Red, bg::Green));
        REQUIRE_FALSE(buffer.charAttributes().isBold());
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), Color(fg::Red, bg::Blue));
        REQUIRE(buffer.get(Position{0, 0}).attributes().isBold());
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), Color(fg::Red, bg::Green));
        REQUIRE_FALSE(buffer.get(Position{1, 0}).attributes().isBold());
    }

    void testWideCharactersPreserveAttributesInTheContinuationCell() {
        auto buffer = CursorBuffer{Size{3, 1}};
        auto attributes = CharAttributes{};
        attributes.setItalic(true);

        buffer.write(Char{U'界', Color{fg::Red, bg::Black}, attributes});

        REQUIRE(buffer.get(Position{0, 0}).attributes().isItalic());
        REQUIRE(buffer.get(Position{1, 0}).isEmpty());
        REQUIRE(buffer.get(Position{1, 0}).attributes().isItalic());
    }

    void testFillCharSetterControlsClearScreenAndResetsTheCursor() {
        auto buffer = CursorBuffer{Size{3, 2}};
        const auto fillChar = Char{U'.', fg::BrightBlack, bg::Black};

        buffer.setFillChar(fillChar);
        buffer.write(String{"ABC"});
        buffer.writeLineBreak();
        buffer.write(Char{U'D'});

        buffer.clearScreen();
        buffer.write(Char{U'X'});

        REQUIRE_EQUAL(buffer.fillChar(), fillChar);
        REQUIRE_EQUAL(buffer.get(Position{1, 0}), fillChar);
        REQUIRE_EQUAL(buffer.get(Position{0, 1}), fillChar);
        requireRowsEqual(buffer, {"X..", "..."});
    }

    void testFillCharSetterRejectsCharactersThatDoNotOccupyExactlyOneCell() {
        auto buffer = CursorBuffer{Size{3, 2}};

        REQUIRE_THROWS_AS(std::invalid_argument, buffer.setFillChar(Char{U'\n'}));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.setFillChar(Char{U'界'}));
    }

    void testMoveCursorClampsToTheBufferArea() {
        auto buffer = CursorBuffer{Size{3, 2}};

        buffer.moveTo(Position{99, 99});
        buffer.write(Char{U'X'});

        REQUIRE(buffer.get(Position{2, 1}) == U'X');
    }

    void testMoveCursorIgnoresExtremeAbsoluteMoves() {
        auto buffer = CursorBuffer{Size{3, 2}};

        buffer.moveCursor(Position{10'001, 0}, MoveMode::Absolute);
        buffer.write(Char{U'X'});

        REQUIRE(buffer.get(Position{0, 0}) == U'X');
    }

    void testDeferredWrapMovesTheNextCharacterToTheFollowingLine() {
        auto buffer = CursorBuffer{Size{2, 2}};

        buffer.write(String{"AB"});
        buffer.write(Char{U'C'});

        requireRowsEqual(buffer, {"AB", "C "});
    }

    void testExplicitLineBreakClearsDeferredWrap() {
        auto buffer = CursorBuffer{Size{3, 3}};

        buffer.write(String{"ABC"});
        buffer.writeLineBreak();
        buffer.write(Char{U'D'});

        requireRowsEqual(buffer, {"ABC", "D  ", "   "});
    }

    void testMovingTheCursorClearsDeferredWrap() {
        auto buffer = CursorBuffer{Size{3, 2}};

        buffer.write(String{"ABC"});
        buffer.moveTo(Position{1, 1});
        buffer.write(Char{U'D'});

        requireRowsEqual(buffer, {"ABC", " D "});
    }

    void testAutoWrapCanBeDisabledAtTheRightMargin() {
        auto buffer = CursorBuffer{Size{2, 2}};

        buffer.moveTo(Position{1, 0});
        buffer.setAutoWrap(false);
        buffer.write(Char{U'A'});
        buffer.write(Char{U'B'});

        requireRowsEqual(buffer, {" B", "  "});
    }

    void testWideCharactersWrapBeforeWritingAtTheLastColumn() {
        auto buffer = CursorBuffer{Size{3, 2}};

        buffer.moveTo(Position{2, 0});
        buffer.write(Char{U'界'});

        REQUIRE(buffer.get(Position{0, 1}) == U'界');
        REQUIRE(buffer.get(Position{1, 1}).isEmpty());
        requireRowsEqual(buffer, {"   ", "界  "});
    }

    void testWideCharactersEndingAtTheRightMarginDeferTheNextWrap() {
        auto buffer = CursorBuffer{Size{3, 2}};

        buffer.moveTo(Position{1, 0});
        buffer.write(Char{U'界'});
        buffer.write(Char{U'X'});

        REQUIRE(buffer.get(Position{1, 0}) == U'界');
        REQUIRE(buffer.get(Position{2, 0}).isEmpty());
        REQUIRE(buffer.get(Position{0, 1}) == U'X');
    }

    void testWritingAnotherBufferMovesToTheNextLineForEachSourceRow() {
        auto buffer = CursorBuffer{Size{3, 3}};
        auto source = Buffer{Size{2, 2}};
        source.set(Position{0, 0}, Char{U'A'});
        source.set(Position{1, 0}, Char{U'B'});
        source.set(Position{0, 1}, Char{U'C'});
        source.set(Position{1, 1}, Char{U'D'});

        buffer.write(source);

        requireRowsEqual(buffer, {"AB ", "CD ", "   "});
    }

    void testShiftOverflowScrollsTheBufferUpUsingTheConfiguredFillChar() {
        auto buffer =
            CursorBuffer{Size{3, 2}, CursorBuffer::OverflowMode::Shift, CursorBuffer::cMaximumSize, Char{U'.'}};

        buffer.write(Char{U'A'});
        buffer.writeLineBreak();
        buffer.write(Char{U'B'});
        buffer.writeLineBreak();
        buffer.write(Char{U'C'});

        requireRowsEqual(buffer, {"B..", "C.."});
    }

    void testWrapOverflowMovesBackToTheFirstLine() {
        auto buffer = CursorBuffer{Size{3, 2}, CursorBuffer::OverflowMode::Wrap};

        buffer.write(Char{U'A'});
        buffer.writeLineBreak();
        buffer.write(Char{U'B'});
        buffer.writeLineBreak();
        buffer.write(Char{U'C'});

        requireRowsEqual(buffer, {"C  ", "B  "});
    }

    void testExpandThenShiftGrowsVerticallyUsingTheConfiguredFillChar() {
        auto buffer = CursorBuffer{Size{3, 2}, CursorBuffer::OverflowMode::ExpandThenShift, Size{3, 3}, Char{U'.'}};

        buffer.write(Char{U'A'});
        buffer.writeLineBreak();
        buffer.write(Char{U'B'});
        buffer.writeLineBreak();
        buffer.write(Char{U'C'});

        REQUIRE_EQUAL(buffer.size(), (Size{3, 3}));
        requireRowsEqual(buffer, {"A..", "B..", "C.."});

        buffer.writeLineBreak();
        buffer.write(Char{U'D'});

        requireRowsEqual(buffer, {"B..", "C..", "D.."});
    }

    void testExpandThenWrapGrowsVerticallyUsingTheConfiguredFillChar() {
        auto buffer = CursorBuffer{Size{3, 2}, CursorBuffer::OverflowMode::ExpandThenWrap, Size{3, 3}, Char{U'.'}};

        buffer.write(Char{U'A'});
        buffer.writeLineBreak();
        buffer.write(Char{U'B'});
        buffer.writeLineBreak();
        buffer.write(Char{U'C'});

        REQUIRE_EQUAL(buffer.size(), (Size{3, 3}));
        requireRowsEqual(buffer, {"A..", "B..", "C.."});

        buffer.writeLineBreak();
        buffer.write(Char{U'D'});

        requireRowsEqual(buffer, {"D..", "B..", "C.."});
    }

    void testPrintParagraphWritesWrappedLinesAndReturnsTheRenderedLineCount() {
        auto buffer = CursorBuffer{Size{4, 4}};

        const auto lineCount = buffer.printParagraph(String{"AB CD"});

        REQUIRE_EQUAL(lineCount, 2);
        requireRowsEqual(buffer, {"AB  ", "CD  ", "    ", "    "});
    }

    void testPrintParagraphLeavesTheRemainingCellsUntouchedWithoutRightFill() {
        auto buffer = CursorBuffer{Size{4, 4}};
        fillBufferFromRows(buffer, {"1234", "5678", "ABCD", "EFGH"});

        const auto lineCount = buffer.printParagraph(String{"AB CD"});

        REQUIRE_EQUAL(lineCount, 2);
        requireRowsEqual(buffer, {"AB34", "CD78", "ABCD", "EFGH"});
    }

    void testPrintParagraphStillFillsTheRightSideWhenBackgroundModeRequiresIt() {
        auto buffer = CursorBuffer{Size{4, 4}};
        fillBufferFromRows(buffer, {"1234", "5678", "ABCD", "EFGH"});
        buffer.setBackground(bg::Blue);
        auto options = ParagraphOptions{};
        options.setBackgroundMode(ParagraphBackgroundMode::FullRight);

        const auto lineCount = buffer.printParagraph(String{"AB CD"}, options);

        REQUIRE_EQUAL(lineCount, 2);
        requireRowsEqual(buffer, {"AB  ", "CD  ", "ABCD", "EFGH"});
        REQUIRE_EQUAL(buffer.get(Position{2, 0}).color().bg(), bg::Blue);
        REQUIRE_EQUAL(buffer.get(Position{2, 1}).color().bg(), bg::Blue);
    }

    void testPrintParagraphSupportsLogStyleWrapMarkersAndEllipsis() {
        auto buffer = CursorBuffer{Size{18, 5}};
        buffer.setColor(Color{fg::BrightBlue, bg::Black});
        auto options = ParagraphOptions{};
        options.setWrappedLineIndent(4);
        options.setLineBreakEndMark(String{">"});
        options.setLineBreakStartMark(String{"< "});
        options.setMaximumLineWraps(2);
        options.setParagraphEllipsisMark(String{"(...)"});

        const auto lineCount =
            buffer.printParagraph(String{"alpha beta gamma delta epsilon zeta eta theta iota"}, options);

        REQUIRE_EQUAL(lineCount, 3);
        REQUIRE(buffer.get(Position{17, 0}) == U'>');
        REQUIRE(buffer.get(Position{4, 1}) == U'<');
        REQUIRE(buffer.get(Position{4, 2}) == U'<');
        auto hasEllipsis = false;
        for (Coordinate x = 0; x < buffer.size().width(); ++x) {
            if (buffer.get(Position{x, 2}) == U'(') {
                hasEllipsis = true;
            }
        }
        REQUIRE(hasEllipsis);
    }

    void testPrintParagraphCanIndentWrappedContinuationLinesForLogDetails() {
        auto buffer = CursorBuffer{Size{18, 4}};
        buffer.setColor(Color{fg::White, bg::Black});
        auto options = ParagraphOptions{};
        options.setFirstLineIndent(4);
        options.setWrappedLineIndent(4);

        const auto lineCount = buffer.printParagraph(String{"detail line wraps here"}, options);

        REQUIRE_EQUAL(lineCount, 2);
        REQUIRE(buffer.get(Position{4, 0}) == U'd');
        REQUIRE(buffer.get(Position{4, 1}) == U'w');
        for (Coordinate x = 0; x < 4; ++x) {
            REQUIRE(buffer.get(Position{x, 0}) == U' ');
            REQUIRE(buffer.get(Position{x, 1}) == U' ');
        }
    }

    void testPrintParagraphHonorsHorizontalMargins() {
        auto buffer = CursorBuffer{Size{6, 4}};
        auto options = ParagraphOptions{};
        options.setMargins(Margins{1, 0});

        const auto lineCount = buffer.printParagraph(String{"AB CD"}, options);

        REQUIRE_EQUAL(lineCount, 2);
        requireRowsEqual(buffer, {" AB   ", " CD   ", "      ", "      "});
    }

    void testPrintParagraphPreservesExistingRowsWhileExpandingTheBufferHeight() {
        auto buffer = CursorBuffer{Size{4, 1}, CursorBuffer::OverflowMode::ExpandThenShift, Size{4, 4}, Char{U'.'}};

        const auto lineCount = buffer.printParagraph(String{"AB CD EF"});

        REQUIRE_EQUAL(lineCount, 3);
        REQUIRE_EQUAL(buffer.size(), (Size{4, 4}));
        requireRowsEqual(buffer, {"AB..", "CD..", "EF..", "...."});
    }

    void testExpandThenShiftPreservesVisibleRowsEvenWhenTheRowMapWasChangedBeforehand() {
        auto buffer = CursorBuffer{Size{3, 2}, CursorBuffer::OverflowMode::ExpandThenShift, Size{3, 4}, Char{U'.'}};

        buffer.write(Char{U'A'});
        buffer.writeLineBreak();
        buffer.write(Char{U'B'});
        buffer.eraseRows(0, Char{U'.'}, 1);
        buffer.moveTo(Position{0, 1});

        buffer.writeLineBreak();
        buffer.write(Char{U'C'});

        REQUIRE_EQUAL(buffer.size(), (Size{3, 3}));
        requireRowsEqual(buffer, {"B..", "...", "C.."});
    }
};
