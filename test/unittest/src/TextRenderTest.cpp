// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/unittest/UnitTest.hpp>

#include <memory>

#include "TestHelper.hpp"


class TextRenderTest : public el::UnitTest {
public:
    void testBufferRendersAlignedText() {
        auto buffer = term::Buffer{Size{8, 3}};
        buffer.drawText("Hi", Alignment::Center, Rectangle{0, 0, 8, 3}, term::Color{term::fg::Yellow, term::bg::Black});
        REQUIRE_EQUAL(buffer.get(Position{3, 1}).charStr(), std::string{"H"});
        REQUIRE_EQUAL(buffer.get(Position{4, 1}).charStr(), std::string{"i"});
        REQUIRE_EQUAL(buffer.get(Position{3, 1}).color(), term::Color(term::fg::Yellow, term::bg::Black));
    }

    void testBufferKeepsCharacterColorsIfNoColorSequenceIsDefined() {
        auto text = term::String{};
        text.append(term::Char{"A", term::Color{term::fg::Red, term::bg::Black}});
        text.append(term::Char{"B", term::Color{term::fg::Green, term::bg::Black}});
        auto buffer = term::Buffer{Size{4, 1}};
        buffer.drawText(term::Text{text, Rectangle{0, 0, 4, 1}, Alignment::TopLeft});
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), term::Color(term::fg::Red, term::bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), term::Color(term::fg::Green, term::bg::Black));
    }

    void testBufferUsesExplicitAnimationCycleForFlashAnimation() {
        auto buffer = term::Buffer{Size{3, 1}};
        auto text = term::Text{term::String{"ABC"}, Rectangle{0, 0, 3, 1}, Alignment::TopLeft};
        text.setColorSequence(term::ColorSequence{
            term::Color{term::fg::Red, term::bg::Black},
            term::Color{term::fg::Green, term::bg::Black},
            term::Color{term::fg::Blue, term::bg::Black}});
        text.setAnimation(term::TextAnimation::ColorDiagonal);
        buffer.drawText(text, 1);
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), term::Color(term::fg::Green, term::bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), term::Color(term::fg::Blue, term::bg::Black));
        REQUIRE_EQUAL(buffer.get(Position{2, 0}).color(), term::Color(term::fg::Red, term::bg::Black));
    }

    void testBufferKeepsCharacterColorPartsOverTextColor() {
        auto text = term::String{};
        text.append(term::Char{"A", term::Color{term::fg::Red, term::bg::Inherited}});
        text.append(term::Char{"B", term::Color{term::fg::Inherited, term::bg::Blue}});
        auto renderedText = term::Text{text, Rectangle{0, 0, 2, 1}, Alignment::TopLeft};
        renderedText.setColor(term::Color{term::fg::Green, term::bg::Yellow});
        auto buffer = term::Buffer{Size{2, 1}};
        buffer.drawText(renderedText);
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), term::Color(term::fg::Red, term::bg::Yellow));
        REQUIRE_EQUAL(buffer.get(Position{1, 0}).color(), term::Color(term::fg::Green, term::bg::Blue));
    }

    void testBufferUsesDefaultAsExplicitTextReset() {
        auto text = term::String{};
        text.append(term::Char{"A", term::Color{term::fg::Default, term::bg::Inherited}});
        auto renderedText = term::Text{text, Rectangle{0, 0, 1, 1}, Alignment::TopLeft};
        renderedText.setColor(term::Color{term::fg::Green, term::bg::Yellow});
        auto buffer = term::Buffer{Size{1, 1}};
        buffer.drawText(renderedText);

        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), term::Color(term::fg::Default, term::bg::Yellow));
    }

    void testBufferRendersBitmapFontText() {
        auto font = std::make_shared<term::Font>(2);
        font->addGlyph("A", term::FontGlyph{std::vector<uint64_t>{0b11U, 0b11U}});
        auto text = term::Text{term::String{"A"}, Rectangle{0, 0, 2, 1}, Alignment::TopLeft};
        text.setColor(term::Color{term::fg::BrightWhite, term::bg::Black});
        text.setFont(font);
        auto buffer = term::Buffer{Size{2, 1}};
        buffer.drawText(text);
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).charStr(), std::string{"█"});
        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color(), term::Color(term::fg::BrightWhite, term::bg::Black));
    }

    void testTextSetColorCreatesSingleEntryColorSequence() {
        auto text = term::Text{term::String{"A"}, Rectangle{0, 0, 1, 1}, Alignment::TopLeft};
        REQUIRE_EQUAL(text.color(), term::Color{});
        text.setColor(term::Color{term::fg::Cyan, term::bg::Black});
        REQUIRE_EQUAL(text.color(), term::Color(term::fg::Cyan, term::bg::Black));
        REQUIRE_EQUAL(text.colorSequence().sequenceLength(), std::size_t{1});
    }
};
