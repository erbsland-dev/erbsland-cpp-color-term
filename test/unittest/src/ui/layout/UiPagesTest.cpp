// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/BufferTestHelper.hpp"

#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>

TESTED_TARGETS(UiPages UiLayout)
class UiPagesTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testPagesIndexesVisibleChildrenOnly() {
        auto pages = ui::Pages::create();
        auto first = ui::Panel::create();
        auto hidden = ui::Panel::create();
        auto third = ui::Panel::create();
        hidden->flags().setVisible(false);
        pages->addSurface(first);
        pages->addSurface(hidden);
        pages->addSurface(third);

        REQUIRE_EQUAL(pages->pageCount(), 2);
        REQUIRE_EQUAL(pages->currentPage(), 0);
        REQUIRE_EQUAL(pages->indexForPage(first), 0);
        REQUIRE_EQUAL(pages->indexForPage(hidden), -1);
        REQUIRE_EQUAL(pages->indexForPage(third), 1);
    }

    void testPagesClampNavigationAndRejectInvalidPages() {
        auto pages = ui::Pages::create();
        auto first = ui::Panel::create();
        auto second = ui::Panel::create();
        pages->addSurface(first);
        pages->addSurface(second);

        pages->previousPage();
        REQUIRE_EQUAL(pages->currentPage(), 0);
        pages->nextPage();
        pages->nextPage();
        REQUIRE_EQUAL(pages->currentPage(), 1);
        pages->previousPage();
        REQUIRE_EQUAL(pages->currentPage(), 0);

        REQUIRE_THROWS_AS(std::out_of_range, pages->showPage(-1));
        REQUIRE_THROWS_AS(std::out_of_range, pages->showPage(2));
        REQUIRE_THROWS_AS(std::out_of_range, pages->showPage(ui::Panel::create()));
    }

    void testPagesCentersConstrainedCurrentPageAndIgnoresOtherPages() {
        auto pages = ui::Pages::create();
        auto first = ui::Panel::create();
        auto second = ui::Panel::create();
        first->editLayoutMetrics().setFixedSize(Size{4, 2});
        second->editLayoutMetrics().setFixedSize(Size{3, 1});
        pages->addSurface(first);
        pages->addSurface(second);
        pages->showPage(0);

        pages->setRectangle(Rectangle{0, 0, 10, 6});
        pages->layout(Size{10, 6}, ui::LayoutContext{});

        REQUIRE_EQUAL(first->rectangle(), Rectangle(3, 2, 4, 2));
        REQUIRE_EQUAL(second->rectangle(), Rectangle());
    }

    void testPagesCentersOuterMarginBox() {
        auto pages = ui::Pages::create();
        auto child = ui::Panel::create();
        child->editLayoutMetrics().setFixedSize(Size{2, 1}).setMargins(Margins{1});
        pages->addSurface(child);

        pages->setRectangle(Rectangle{0, 0, 10, 5});
        pages->layout(Size{10, 5}, ui::LayoutContext{});

        REQUIRE_EQUAL(child->rectangle(), Rectangle(4, 2, 2, 1));
    }

    void testPagesPaintsOpaqueBackground() {
        auto pages = ui::Pages::create();
        pages->setRectangle(Rectangle{0, 0, 4, 2});
        const auto context = pages->themeContextFrom(ui::ThemeContext{});
        auto buffer = Buffer{Size{4, 2}, Char{U'.'}};

        pages->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), context});

        REQUIRE(pages->isOpaque());
        requireRowsEqual(buffer, {"    ", "    "});
    }
};
