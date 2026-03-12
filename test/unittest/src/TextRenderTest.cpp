// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <memory>


class TextRenderTest : public el::UnitTest {
public:
    void testTextUsesSingleParagraphSpacingByDefault() {
        auto text = Text{};
        REQUIRE_EQUAL(text.paragraphSpacing(), ParagraphSpacing::SingleLine);
    }

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

    void testTextSetColorCreatesSingleEntryColorSequence() {
        auto text = Text{String{"A"}, Rectangle{0, 0, 1, 1}, Alignment::TopLeft};
        REQUIRE_EQUAL(text.color(), Color{});
        text.setColor(Color{fg::Cyan, bg::Black});
        REQUIRE_EQUAL(text.color(), Color(fg::Cyan, bg::Black));
        REQUIRE_EQUAL(text.colorSequence().sequenceLength(), std::size_t{1});
    }
};
