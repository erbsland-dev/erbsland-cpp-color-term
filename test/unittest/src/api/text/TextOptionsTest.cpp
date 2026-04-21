// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(TextOptions)
class TextOptionsTest final : public el::UnitTest {
public:
    void testDefaultOptionsExposeTheExpectedDefaults() {
        const auto options = TextOptions{};

        REQUIRE(options.colorSequence().empty());
        REQUIRE_EQUAL(options.color(), Color{});
        REQUIRE(options.font() == nullptr);
        REQUIRE_EQUAL(options.animation(), TextAnimation::None);
        REQUIRE_EQUAL(options.alignment(), Alignment::TopLeft);
        REQUIRE_EQUAL(options.lineIndent(), 0);
        REQUIRE_EQUAL(options.firstLineIndent(), 0);
        REQUIRE_EQUAL(options.wrappedLineIndent(), 0);
        REQUIRE_EQUAL(options.margins(), Margins{0});
        REQUIRE_EQUAL(options.backgroundMode(), ParagraphBackgroundMode::Default);
        REQUIRE(options.lineBreakEndMark().empty());
        REQUIRE(options.lineBreakStartMark().empty());
        REQUIRE_EQUAL(options.paragraphSpacing(), ParagraphSpacing::SingleLine);
        REQUIRE_EQUAL(options.wordSeparators(), std::u32string{U"\t "});
        REQUIRE_EQUAL(options.wordBreakMark(), U'-');
        REQUIRE_EQUAL(options.maximumLineWraps(), 0);
        requireStringEqual(options.paragraphEllipsisMark(), U"…");
        REQUIRE_EQUAL(options.tabStops().size(), std::size_t{1});
        REQUIRE_EQUAL(options.tabStops()[0], ParagraphOptions::cTabWrappedLineIndent);
        REQUIRE_EQUAL(options.tabOverflowBehavior(), TabOverflowBehavior::AddSpace);
        REQUIRE_EQUAL(options.onError(), ParagraphOnError::PlainOutput);
    }

    void testColorFontAnimationAndParagraphOptionsCanBeConfigured() {
        auto options = TextOptions{Alignment::Center};
        auto paragraphOptions = ParagraphOptions{Alignment::BottomRight};
        const auto font = std::make_shared<Font>(3);

        paragraphOptions.setLineIndent(2);
        paragraphOptions.setFirstLineIndent(4);
        paragraphOptions.setWrappedLineIndent(6);
        paragraphOptions.setMargins(Margins{1, 2});
        paragraphOptions.setBackgroundMode(ParagraphBackgroundMode::FullBoth);
        paragraphOptions.setLineBreakEndMark(String{"<"});
        paragraphOptions.setLineBreakStartMark(String{">"});
        paragraphOptions.setParagraphSpacing(ParagraphSpacing::DoubleLine);
        paragraphOptions.setWordSeparators(U".,");
        paragraphOptions.setWordBreakMark(Char{U'~'});
        paragraphOptions.setMaximumLineWraps(5);
        paragraphOptions.setParagraphEllipsisMark(String{"..."});
        paragraphOptions.setTabStops({2, 8});
        paragraphOptions.setTabOverflowBehavior(TabOverflowBehavior::LineBreak);
        paragraphOptions.setOnError(ParagraphOnError::Empty);

        options.setColorSequence(ColorSequence{Color{fg::Red, bg::Black}, Color{fg::Blue, bg::Black}});
        options.setFont(font);
        options.setAnimation(TextAnimation::ColorDiagonal);
        options.setParagraphOptions(paragraphOptions);

        REQUIRE_EQUAL(options.colorSequence().sequenceLength(), std::size_t{2});
        REQUIRE_EQUAL(options.color(), Color(fg::Red, bg::Black));
        REQUIRE(options.font() == font);
        REQUIRE_EQUAL(options.animation(), TextAnimation::ColorDiagonal);
        REQUIRE_EQUAL(options.paragraphOptions().alignment(), Alignment::BottomRight);
        REQUIRE_EQUAL(options.lineIndent(), 2);
        REQUIRE_EQUAL(options.firstLineIndent(), 4);
        REQUIRE_EQUAL(options.wrappedLineIndent(), 6);
        REQUIRE_EQUAL(options.margins(), Margins(1, 2));
        REQUIRE_EQUAL(options.backgroundMode(), ParagraphBackgroundMode::FullBoth);
        requireStringEqual(options.lineBreakEndMark(), U"<");
        requireStringEqual(options.lineBreakStartMark(), U">");
        REQUIRE_EQUAL(options.paragraphSpacing(), ParagraphSpacing::DoubleLine);
        REQUIRE_EQUAL(options.wordSeparators(), std::u32string{U",."});
        REQUIRE_EQUAL(options.wordBreakMark(), U'~');
        REQUIRE_EQUAL(options.maximumLineWraps(), 5);
        requireStringEqual(options.paragraphEllipsisMark(), U"...");
        REQUIRE_EQUAL(options.tabStops().size(), std::size_t{2});
        REQUIRE_EQUAL(options.tabStops()[0], 2);
        REQUIRE_EQUAL(options.tabStops()[1], 8);
        REQUIRE_EQUAL(options.tabOverflowBehavior(), TabOverflowBehavior::LineBreak);
        REQUIRE_EQUAL(options.onError(), ParagraphOnError::Empty);
    }

    void testWrapperSettersUpdateAndClampParagraphState() {
        auto options = TextOptions{};

        options.setColor(Color{fg::Cyan, bg::Blue});
        options.setAlignment(Alignment::BottomCenter);
        options.setLineIndent(-4);
        options.setFirstLineIndent(ParagraphOptions::cUseLineIndent);
        options.setWrappedLineIndent(-7);
        options.setMargins(Margins{2});
        options.setMaximumLineWraps(-2);
        options.setLineBreakEndMark(String{"!"});
        options.setLineBreakStartMark(String{"?"});
        options.setParagraphEllipsisMark(String{"(more)"});
        options.setTabStops({4});

        REQUIRE_EQUAL(options.colorSequence().sequenceLength(), std::size_t{1});
        REQUIRE_EQUAL(options.color(), Color(fg::Cyan, bg::Blue));
        REQUIRE_EQUAL(options.alignment(), Alignment::BottomCenter);
        REQUIRE_EQUAL(options.lineIndent(), 0);
        REQUIRE_EQUAL(options.firstLineIndent(), 0);
        REQUIRE_EQUAL(options.wrappedLineIndent(), 0);
        REQUIRE_EQUAL(options.margins(), Margins(2));
        REQUIRE_EQUAL(options.maximumLineWraps(), 0);
        requireStringEqual(options.lineBreakEndMark(), U"!");
        requireStringEqual(options.lineBreakStartMark(), U"?");
        requireStringEqual(options.paragraphEllipsisMark(), U"(more)");
        REQUIRE_EQUAL(options.tabStops().size(), std::size_t{1});
        REQUIRE_EQUAL(options.tabStops()[0], 4);
    }

private:
    void requireStringEqual(const String &actual, const std::u32string_view expected) {
        REQUIRE_EQUAL(actual.size(), expected.size());
        for (std::size_t i = 0; i < expected.size(); ++i) {
            REQUIRE_EQUAL(actual[i], expected[i]);
        }
    }
};
