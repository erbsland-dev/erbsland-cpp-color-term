// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/BufferTestHelper.hpp"

#include <erbsland/cterm/theme/ThemeBuilder.hpp>
#include <erbsland/cterm/ui/surface/DynamicText.hpp>
#include <erbsland/cterm/ui/surface/StaticText.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <format>

TESTED_TARGETS(UiStaticText UiDynamicText)
class UiStaticTextTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testStaticTextMeasuresAndCropsToAvailableWidth() {
        auto staticText = ui::StaticText::create("ABCDEFG");
        auto scope = ui::MeasureScope{};
        const auto metrics = staticText->onMeasure(scope, ui::LayoutProposal::atMost(Size{4, 1}));

        REQUIRE_EQUAL(metrics.preferred(), (Size{4, 1}));

        auto builder = theme::ThemeBuilder::zero();
        builder.edit(theme::Selector{theme::Element::Surface, theme::Part::Ellipsis})
            .setBlock(theme::BlockRole::Single, U'*');
        const auto activeTheme = builder.build();
        auto buffer = Buffer{Size{4, 1}, Char{U'.'}};
        staticText->setRectangle(Rectangle{0, 0, 4, 1});
        staticText->onPaint(
            buffer,
            ui::PaintContext{
                buffer.rect(),
                buffer.rect(),
                buffer.rect(),
                ui::ThemeContext{activeTheme, theme::Element::StaticText}});

        requireRowsEqual(buffer, {"ABC*"});
    }

    void testStaticTextInvalidatesLayoutOnlyWhenDisplayWidthChanges() {
        auto staticText = ui::StaticText::create("abcd");
        staticText->layout(Size{4, 1}, ui::LayoutContext{});

        staticText->setText("WXYZ");

        REQUIRE_FALSE(staticText->flags().isLayoutOutdated());
        REQUIRE(staticText->flags().isPaintOutdated());

        staticText->layout(Size{4, 1}, ui::LayoutContext{});
        staticText->setText("abcde");

        REQUIRE(staticText->flags().isLayoutOutdated());
        REQUIRE(staticText->flags().isPaintOutdated());
    }

    void testStaticTextUsesHorizontalTextMarginsAndPaddingOnly() {
        auto builder = theme::ThemeBuilder::zero();
        builder.edit(theme::Selector{theme::Element::StaticText, theme::Part::Text})
            .setBlock(theme::BlockRole::LeftPadding, U'<')
            .setBlock(theme::BlockRole::RightPadding, U'>')
            .setMargins(Margins{9, 1, 9, 1})
            .setPadding(Margins{9, 2, 9, 2});
        const auto activeTheme = builder.build();
        auto staticText = ui::StaticText::create("X");
        const auto themeContext = staticText->themeContextFrom(ui::ThemeContext{activeTheme});
        auto scope = ui::MeasureScope{{}, themeContext};

        const auto metrics = staticText->onMeasure(scope, ui::LayoutProposal::unconstrained());
        REQUIRE_EQUAL(metrics.preferred(), (Size{5, 1}));
        REQUIRE_EQUAL(metrics.margins(), (Margins{0, 1, 0, 1}));

        auto buffer = Buffer{Size{5, 1}, Char{U'.'}};
        staticText->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), themeContext});

        requireRowsEqual(buffer, {"<<X>>"});
    }

    void testDynamicTextUpdatesManuallyWithCurrentWidth() {
        auto dynamicText = ui::DynamicText::create();
        auto updateCount = 0;

        dynamicText->setUpdateFn([&updateCount](String &text, const Coordinate width) -> void {
            updateCount += 1;
            text = String{std::format("D{}-{}", updateCount, width)};
        });
        dynamicText->setRectangle(Rectangle{0, 0, 8, 1});
        dynamicText->updateText();

        REQUIRE_EQUAL(updateCount, 1);
        REQUIRE_EQUAL(render(dynamicText->text()), "D1-8");
    }
};
