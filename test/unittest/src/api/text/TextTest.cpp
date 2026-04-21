// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(Text)
class TextTest final : public el::UnitTest {
public:
    void testDefaultConstructionUsesEmptyTextAndDefaultOptions() {
        const auto text = Text{};
        const auto expectedRect = Rectangle{0, 0, 0, 0};

        REQUIRE(text.text().empty());
        REQUIRE_EQUAL(text.rectangle(), expectedRect);
        REQUIRE_EQUAL(text.color(), Color{});
        REQUIRE(text.font() == nullptr);
        REQUIRE_EQUAL(text.animation(), TextAnimation::None);
        REQUIRE_EQUAL(text.alignment(), Alignment::TopLeft);
        REQUIRE_EQUAL(text.paragraphSpacing(), ParagraphSpacing::SingleLine);
    }

    void testTextUsesSingleParagraphSpacingByDefault() {
        const auto text = Text{};

        REQUIRE_EQUAL(text.paragraphSpacing(), ParagraphSpacing::SingleLine);
    }

    void testConstructorStoresTextRectangleAndAlignment() {
        const auto text = Text{String{"Hello"}, Rectangle{1, 2, 7, 3}, Alignment::CenterRight};

        requireStringEqual(text.text(), U"Hello");
        REQUIRE_EQUAL(text.rectangle(), (Rectangle{1, 2, 7, 3}));
        REQUIRE_EQUAL(text.alignment(), Alignment::CenterRight);
    }

    void testTextAndTextOptionSettersReplaceStoredConfiguration() {
        auto text = Text{};
        auto options = TextOptions{Alignment::BottomLeft};
        const auto font = std::make_shared<Font>(2);

        options.setColor(Color{fg::Yellow, bg::Blue});
        options.setFont(font);
        options.setAnimation(TextAnimation::ColorDiagonal);
        options.setLineIndent(3);
        options.setWrappedLineIndent(5);
        options.setMargins(Margins{2, 1});
        options.setParagraphSpacing(ParagraphSpacing::DoubleLine);
        options.setTabStops({6});

        text.setText(String{"ABC"});
        text.setRectangle(Rectangle{4, 5, 6, 7});
        text.setTextOptions(options);

        requireStringEqual(text.text(), U"ABC");
        REQUIRE_EQUAL(text.rectangle(), (Rectangle{4, 5, 6, 7}));
        REQUIRE_EQUAL(text.color(), Color(fg::Yellow, bg::Blue));
        REQUIRE(text.font() == font);
        REQUIRE_EQUAL(text.animation(), TextAnimation::ColorDiagonal);
        REQUIRE_EQUAL(text.alignment(), Alignment::BottomLeft);
        REQUIRE_EQUAL(text.lineIndent(), 3);
        REQUIRE_EQUAL(text.wrappedLineIndent(), 5);
        REQUIRE_EQUAL(text.margins(), Margins(2, 1));
        REQUIRE_EQUAL(text.paragraphSpacing(), ParagraphSpacing::DoubleLine);
        REQUIRE_EQUAL(text.tabStops().size(), std::size_t{1});
        REQUIRE_EQUAL(text.tabStops()[0], 6);
    }

    void testWrapperSettersUpdateTheEmbeddedTextOptions() {
        auto text = Text{};

        text.setColorSequence(ColorSequence{Color{fg::Red, bg::Black}, Color{fg::Green, bg::Black}});
        text.setColor(Color{fg::Magenta, bg::Cyan});
        text.setAnimation(TextAnimation::ColorDiagonal);
        text.setAlignment(Alignment::BottomCenter);
        text.setMargins(Margins{1});
        text.setWordSeparators(U",;");
        text.setWordBreakMark(Char{U'='});
        text.setMaximumLineWraps(-1);
        text.setOnError(ParagraphOnError::Empty);

        REQUIRE_EQUAL(text.colorSequence().sequenceLength(), std::size_t{1});
        REQUIRE_EQUAL(text.color(), Color(fg::Magenta, bg::Cyan));
        REQUIRE_EQUAL(text.animation(), TextAnimation::ColorDiagonal);
        REQUIRE_EQUAL(text.alignment(), Alignment::BottomCenter);
        REQUIRE_EQUAL(text.margins(), Margins(1));
        REQUIRE_EQUAL(text.wordSeparators(), std::u32string{U",;"});
        REQUIRE_EQUAL(text.wordBreakMark(), U'=');
        REQUIRE_EQUAL(text.maximumLineWraps(), 0);
        REQUIRE_EQUAL(text.onError(), ParagraphOnError::Empty);
    }

    void testSetColorCreatesSingleEntryColorSequence() {
        auto text = Text{String{"A"}, Rectangle{0, 0, 1, 1}, Alignment::TopLeft};

        REQUIRE_EQUAL(text.color(), Color{});

        text.setColor(Color{fg::Cyan, bg::Black});

        REQUIRE_EQUAL(text.color(), Color(fg::Cyan, bg::Black));
        REQUIRE_EQUAL(text.colorSequence().sequenceLength(), std::size_t{1});
    }

private:
    void requireStringEqual(const String &actual, const std::u32string_view expected) {
        REQUIRE_EQUAL(actual.size(), expected.size());
        for (std::size_t i = 0; i < expected.size(); ++i) {
            REQUIRE_EQUAL(actual[i], expected[i]);
        }
    }
};
