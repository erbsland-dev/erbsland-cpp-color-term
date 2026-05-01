// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TerminalTestHelper.hpp"

#include <erbsland/cterm/theme/Theme.hpp>
#include <erbsland/cterm/theme/ThemeBuilder.hpp>
#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(UiTextBox)
class UiTextBoxTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testCreateUsesTheTextDimensionsAsPreferredSize() {
        const auto textBox = ui::TextBox::create(String{U"AB\n\nC\n"});

        requireStringEqual(textBox->text(), U"AB\n\nC\n");
        REQUIRE_EQUAL(textBox->layoutMetrics().preferred(), Size(2, 3));
    }

    void testSetTextRecomputesPreferredSizeWithoutChangingContentSemantics() {
        const auto textBox = ui::TextBox::create("X");

        textBox->setText(String{U"中A\nB"});

        requireStringEqual(textBox->text(), U"中A\nB");
        REQUIRE_EQUAL(textBox->layoutMetrics().preferred(), Size(3, 2));
    }

    void testPreferredLineWidthWrapsPreferredSize() {
        const auto textBox = ui::TextBox::create("alpha beta gamma");

        textBox->setPreferredLineWidth(6);

        REQUIRE_EQUAL(textBox->preferredLineWidth(), std::optional<Coordinate>{6});
        REQUIRE_EQUAL(textBox->layoutMetrics().preferred(), Size(6, 3));
    }

    void testAtMostMeasurementUsesPreferredLineWidthWithoutClaimingAllAvailableSpace() {
        auto textBox = ui::TextBox::create("alpha beta gamma");
        textBox->setPreferredLineWidth(6);
        auto scope = ui::MeasureScope{};

        const auto metrics = textBox->onMeasure(scope, ui::LayoutProposal::atMost(Size{20, 10}));

        REQUIRE_EQUAL(metrics.preferred(), (Size{6, 3}));
    }

    void testAtMostMeasurementWithoutPreferredLineWidthUsesTheAvailableWidth() {
        auto textBox = ui::TextBox::create("alpha beta gamma");
        auto scope = ui::MeasureScope{};

        const auto metrics = textBox->onMeasure(scope, ui::LayoutProposal::atMost(Size{20, 10}));

        REQUIRE_EQUAL(metrics.preferred(), (Size{20, 1}));
    }

    void testExactMeasurementUsesTheProposedWidth() {
        auto textBox = ui::TextBox::create("alpha beta gamma");
        textBox->setPreferredLineWidth(6);
        auto scope = ui::MeasureScope{};

        const auto metrics = textBox->onMeasure(scope, ui::LayoutProposal::exact(Size{20, 10}));

        REQUIRE_EQUAL(metrics.preferred(), (Size{20, 1}));
    }

    void testTextPartMarginsRemainParentOwnedAndPaddingIsPainted() {
        auto builder = theme::ThemeBuilder::zero();
        builder.edit(theme::Selector{theme::Element::TextBox, theme::Part::Text})
            .setMargins(Margins{1})
            .setPadding(Margins{2, 2});
        builder.edit(theme::Selector{theme::Element::TextBox, theme::Part::Background}).setBlocks(U"123456789");
        const auto activeTheme = builder.build();
        auto textBox = ui::TextBox::create("X");
        const auto themeContext = textBox->themeContextFrom(ui::ThemeContext{activeTheme});
        auto scope = ui::MeasureScope{{}, themeContext};

        const auto metrics = textBox->onMeasure(scope, ui::LayoutProposal::unconstrained());
        REQUIRE_EQUAL(metrics.preferred(), (Size{5, 5}));
        REQUIRE_EQUAL(metrics.margins(), (Margins{1}));

        auto buffer = Buffer{Size{5, 5}, Char{U'.'}};
        textBox->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), themeContext});

        requireRowsEqual(buffer, {"12223", "45556", "45X56", "45556", "78889"});
    }

private:
    void requireStringEqual(const String &actual, const std::u32string_view expected) {
        REQUIRE_EQUAL(actual.size(), expected.size());
        for (std::size_t i = 0; i < expected.size(); ++i) {
            REQUIRE_EQUAL(actual[i], expected[i]);
        }
    }
};
