// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "CursorWriterTestProbe.hpp"

#include "support/StringTestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <optional>

TESTED_TARGETS(CursorWriter)
class CursorWriterTest final : public UNITTEST_SUBCLASS(StringTestHelper) {
public:
    void testStyleAndDefaultColorWrappersUpdateTheTrackedState() {
        auto writer = CursorWriterProbe{};
        auto attributes = CharAttributes{};
        attributes.setUnderline(true);

        writer.setStyle(CharStyle{Color{fg::Red, bg::Blue}, attributes});

        REQUIRE_EQUAL(writer.style().color(), Color(fg::Red, bg::Blue));
        REQUIRE(writer.style().attributes().isUnderline());

        writer.setDefaultColor();

        REQUIRE_EQUAL(writer.color(), Color::reset());
    }

    void testAttributeConvenienceWrappersPreserveExistingState() {
        auto writer = CursorWriterProbe{};

        writer.setBold(true);
        writer.setUnderline(true);
        writer.setBold(false);
        writer.setStrikethrough(true);

        REQUIRE(writer.charAttributes().isBoldSpecified());
        REQUIRE_FALSE(writer.charAttributes().isBold());
        REQUIRE(writer.charAttributes().isUnderline());
        REQUIRE(writer.charAttributes().isStrikethrough());
    }

    void testCursorMovementWrappersTranslateToMoveCursorCalls() {
        auto writer = CursorWriterProbe{};

        writer.moveLeft(2);
        REQUIRE_EQUAL(writer._lastMove, (Position{-2, 0}));
        REQUIRE_EQUAL(writer._lastMoveMode, MoveMode::Relative);

        writer.moveRight(3);
        REQUIRE_EQUAL(writer._lastMove, (Position{3, 0}));
        REQUIRE_EQUAL(writer._lastMoveMode, MoveMode::Relative);

        writer.moveUp(4);
        REQUIRE_EQUAL(writer._lastMove, (Position{0, -4}));
        REQUIRE_EQUAL(writer._lastMoveMode, MoveMode::Relative);

        writer.moveDown(5);
        REQUIRE_EQUAL(writer._lastMove, (Position{0, 5}));
        REQUIRE_EQUAL(writer._lastMoveMode, MoveMode::Relative);

        writer.moveTo(Position{6, 7});
        REQUIRE_EQUAL(writer._lastMove, (Position{6, 7}));
        REQUIRE_EQUAL(writer._lastMoveMode, MoveMode::Absolute);

        writer.moveHome();
        REQUIRE_EQUAL(writer._lastMove, (Position{0, 0}));
        REQUIRE_EQUAL(writer._lastMoveMode, MoveMode::Absolute);
    }

    void testPrintAndParagraphWrappersDispatchSupportedArgumentTypes() {
        auto writer = CursorWriterProbe{};
        auto attributes = CharAttributes{};
        attributes.setItalic(true);

        writer.print(
            Color{fg::Green, bg::Black},
            Foreground{fg::Yellow},
            Background{bg::Blue},
            CharStyle{Color{fg::Inherited, bg::Magenta}, attributes},
            CharAttributes{}.withFlag(CharAttributes::Underline, true),
            Char{U'X'},
            String{"Y"},
            StringView{String{"QR"}}.substr(0, 1),
            std::string{"Z"},
            std::u32string_view{U"Ω"},
            std::string_view{"!"},
            "?");
        const auto paragraphSource = String{"xAA!"};
        const auto lineCount =
            writer.printParagraph(StringView{paragraphSource}.substr(1, 2), ParagraphOptions{Alignment::Right});
        writer.printLine("tail");

        REQUIRE_EQUAL(writer.color(), Color(fg::Yellow, bg::Magenta));
        REQUIRE(writer.charAttributes().isItalic());
        REQUIRE(writer.charAttributes().isUnderline());
        REQUIRE_EQUAL(writer._writtenChars.size(), std::size_t{1});
        REQUIRE_EQUAL(writer._writtenChars[0], U'X');
        REQUIRE_EQUAL(writer._writtenStrings.size(), std::size_t{7});
        requireStringEqual(writer._writtenStrings[0], U"Y");
        requireStringEqual(writer._writtenStrings[1], U"Q");
        requireStringEqual(writer._writtenStrings[2], U"Z");
        requireStringEqual(writer._writtenStrings[3], U"Ω");
        requireStringEqual(writer._writtenStrings[4], U"!");
        requireStringEqual(writer._writtenStrings[5], U"?");
        requireStringEqual(writer._writtenStrings[6], U"tail");
        REQUIRE_EQUAL(writer._lineBreakCount, 1);
        REQUIRE_EQUAL(lineCount, 7);
        requireStringEqual(writer._lastParagraph, U"AA");
        REQUIRE_EQUAL(writer._lastParagraphAlignment, Alignment::Right);
    }

    void testRepeatedWriteWrappersEmitTheRequestedCharacterCount() {
        auto writer = CursorWriterProbe{};

        writer.writeRepeated(Char{U'A'}, 3);
        writer.writeRepeatedResolved(Char{U'B', Color{fg::Cyan, bg::Black}}, 2);
        writer.writeRepeated(Char{U'C'}, 0);
        writer.writeRepeatedResolved(Char{U'D'}, -1);

        REQUIRE_EQUAL(writer._writtenChars.size(), std::size_t{5});
        REQUIRE_EQUAL(writer._writtenChars[0], U'A');
        REQUIRE_EQUAL(writer._writtenChars[1], U'A');
        REQUIRE_EQUAL(writer._writtenChars[2], U'A');
        REQUIRE_EQUAL(writer._writtenChars[3], U'B');
        REQUIRE_EQUAL(writer._writtenChars[4], U'B');
    }
};
