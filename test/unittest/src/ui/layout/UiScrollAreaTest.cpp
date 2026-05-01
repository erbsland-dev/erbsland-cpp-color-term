// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/cterm/ui/layout/ScrollArea.hpp>
#include <erbsland/cterm/ui/layout/Viewport.hpp>
#include <erbsland/cterm/ui/surface/Panel.hpp>
#include <erbsland/unittest/UnitTest.hpp>

using namespace erbsland::cterm;

TESTED_TARGETS(UiViewport UiScrollArea)
class UiScrollAreaTest final : public el::UnitTest {
public:
    void testViewportAcceptsContentThroughDedicatedAndGenericApi() {
        auto viewport = ui::Viewport::create();
        auto content = ui::Panel::create();

        viewport->setContentSurface(content);

        REQUIRE_EQUAL(viewport->contentSurface(), content);
        REQUIRE_EQUAL(content->parent().lock(), viewport);
        REQUIRE_THROWS(viewport->addSurface(ui::Panel::create()));

        viewport->surfaces().remove(content);
        REQUIRE_EQUAL(viewport->contentSurface(), nullptr);
        REQUIRE(content->parent().expired());

        auto genericContent = ui::Panel::create();
        viewport->surfaces().add(genericContent);
        REQUIRE_EQUAL(viewport->contentSurface(), genericContent);
        REQUIRE_EQUAL(genericContent->parent().lock(), viewport);
    }

    void testViewportScrollsOversizedContent() {
        auto viewport = ui::Viewport::create();
        auto content = ui::Panel::create();
        content->editLayoutMetrics().setFixedSize(Size{8, 4});
        viewport->setContentSurface(content);

        viewport->setRectangle(Rectangle{0, 0, 4, 2});
        viewport->layout(Size{4, 2}, ui::LayoutContext{});

        REQUIRE_EQUAL(content->rectangle(), Rectangle(0, 0, 8, 4));

        viewport->setScrollOffset(Position{2, 1});
        viewport->layout(Size{4, 2}, ui::LayoutContext{});

        REQUIRE_EQUAL(viewport->scrollOffset(), Position(2, 1));
        REQUIRE_EQUAL(content->rectangle(), Rectangle(-2, -1, 8, 4));
    }

    void testViewportUsesCurrentMetricsWhenScrollingBeforeFirstLayout() {
        auto viewport = ui::Viewport::create();
        auto content = ui::Panel::create();
        content->editLayoutMetrics().setFixedSize(Size{8, 4});
        viewport->setContentSurface(content);

        viewport->setRectangle(Rectangle{0, 0, 4, 2});
        viewport->setScrollOffset(Position{2, 1});
        viewport->layout(Size{4, 2}, ui::LayoutContext{});

        REQUIRE_EQUAL(viewport->scrollOffset(), Position(2, 1));
        REQUIRE_EQUAL(content->rectangle(), Rectangle(-2, -1, 8, 4));
    }

    void testViewportScrollIntoViewUsesCurrentMetricsBeforeFirstLayout() {
        auto viewport = ui::Viewport::create();
        auto content = ui::Panel::create();
        content->editLayoutMetrics().setFixedSize(Size{8, 4});
        viewport->setContentSurface(content);

        viewport->setRectangle(Rectangle{0, 0, 4, 2});
        viewport->scrollIntoView(Rectangle{6, 3, 1, 1});
        viewport->layout(Size{4, 2}, ui::LayoutContext{});

        REQUIRE_EQUAL(viewport->scrollOffset(), Position(3, 2));
        REQUIRE_EQUAL(content->rectangle(), Rectangle(-3, -2, 8, 4));
    }

    void testViewportAlignsUndersizedContent() {
        auto viewport = ui::Viewport::create();
        auto content = ui::Panel::create();
        content->editLayoutMetrics().setFixedSize(Size{2, 1});
        viewport->setContentSurface(content);
        viewport->setAlignment(Alignment::Center);

        viewport->setRectangle(Rectangle{0, 0, 6, 3});
        viewport->layout(Size{6, 3}, ui::LayoutContext{});

        REQUIRE_EQUAL(content->rectangle(), Rectangle(2, 1, 2, 1));
    }

    void testViewportIncludesContentMarginsInScrollableSize() {
        auto viewport = ui::Viewport::create();
        auto content = ui::Panel::create();
        content->editLayoutMetrics().setFixedSize(Size{4, 2}).setMargins(Margins{1});
        viewport->setContentSurface(content);

        viewport->setRectangle(Rectangle{0, 0, 4, 2});
        viewport->layout(Size{4, 2}, ui::LayoutContext{});

        REQUIRE_EQUAL(viewport->contentSize(), Size(6, 4));
        REQUIRE_EQUAL(content->rectangle(), Rectangle(1, 1, 4, 2));
    }

    void testScrollAreaIgnoresHiddenContentForAutomaticScrollBars() {
        auto scrollArea = ui::ScrollArea::create();
        auto content = ui::Panel::create();
        content->editLayoutMetrics().setFixedSize(Size{10, 6});
        content->flags().setVisible(false);
        scrollArea->setContentSurface(content);

        scrollArea->setRectangle(Rectangle{0, 0, 5, 4});
        scrollArea->layout(Size{5, 4}, ui::LayoutContext{});

        REQUIRE_FALSE(scrollArea->horizontalScrollBar()->flags().isVisible());
        REQUIRE_FALSE(scrollArea->verticalScrollBar()->flags().isVisible());
        REQUIRE_FALSE(scrollArea->scrollCorner()->flags().isVisible());
        REQUIRE_EQUAL(scrollArea->viewport()->rectangle(), Rectangle(0, 0, 5, 4));
        REQUIRE_EQUAL(scrollArea->contentSize(), Size{});
    }

    void testScrollAreaUsesAutomaticScrollBarsAndProtectsInternalChildren() {
        auto scrollArea = ui::ScrollArea::create();
        auto content = ui::Panel::create();
        content->editLayoutMetrics().setFixedSize(Size{6, 4});
        scrollArea->setContentSurface(content);

        scrollArea->setRectangle(Rectangle{0, 0, 5, 3});
        scrollArea->layout(Size{5, 3}, ui::LayoutContext{});

        REQUIRE(scrollArea->horizontalScrollBar()->flags().isVisible());
        REQUIRE(scrollArea->verticalScrollBar()->flags().isVisible());
        REQUIRE(scrollArea->scrollCorner()->flags().isVisible());
        REQUIRE_EQUAL(scrollArea->viewport()->rectangle(), Rectangle(0, 0, 4, 2));
        REQUIRE_EQUAL(scrollArea->horizontalScrollBar()->rectangle(), Rectangle(0, 2, 4, 1));
        REQUIRE_EQUAL(scrollArea->verticalScrollBar()->rectangle(), Rectangle(4, 0, 1, 2));
        REQUIRE_THROWS(scrollArea->addSurface(ui::Panel::create()));
    }

    void testAutomaticVerticalScrollBarCanForceHorizontalScrollBar() {
        auto scrollArea = ui::ScrollArea::create();
        auto content = ui::Panel::create();
        content->editLayoutMetrics().setFixedSize(Size{5, 5});
        scrollArea->setContentSurface(content);

        scrollArea->setRectangle(Rectangle{0, 0, 5, 4});
        scrollArea->layout(Size{5, 4}, ui::LayoutContext{});

        REQUIRE(scrollArea->verticalScrollBar()->flags().isVisible());
        REQUIRE(scrollArea->horizontalScrollBar()->flags().isVisible());
        REQUIRE_EQUAL(scrollArea->viewport()->rectangle(), Rectangle(0, 0, 4, 3));
    }

    void testScrollAreaScrollIntoViewUpdatesViewportAndScrollBars() {
        auto scrollArea = ui::ScrollArea::create();
        auto content = ui::Panel::create();
        content->editLayoutMetrics().setFixedSize(Size{10, 6});
        scrollArea->setContentSurface(content);

        scrollArea->setRectangle(Rectangle{0, 0, 5, 4});
        scrollArea->layout(Size{5, 4}, ui::LayoutContext{});
        scrollArea->scrollIntoView(Rectangle{7, 4, 2, 1});
        scrollArea->layout(Size{5, 4}, ui::LayoutContext{});
        layoutViewport(scrollArea);

        REQUIRE_EQUAL(scrollArea->scrollOffset(), Position(5, 2));
        REQUIRE_EQUAL(content->rectangle(), Rectangle(-5, -2, 10, 6));
    }

    void testScrollAreaUsesCurrentMetricsWhenScrollingBeforeFirstLayout() {
        auto scrollArea = ui::ScrollArea::create();
        auto content = ui::Panel::create();
        content->editLayoutMetrics().setFixedSize(Size{10, 6});
        scrollArea->setContentSurface(content);

        scrollArea->setScrollOffset(Position{5, 2});
        scrollArea->setRectangle(Rectangle{0, 0, 5, 4});
        scrollArea->layout(Size{5, 4}, ui::LayoutContext{});
        layoutViewport(scrollArea);

        REQUIRE_EQUAL(scrollArea->scrollOffset(), Position(5, 2));
        REQUIRE_EQUAL(content->rectangle(), Rectangle(-5, -2, 10, 6));
    }

    void testVisibleScrollBarsAreHiddenWhenTheSurfaceHasNoArea() {
        auto scrollArea = ui::ScrollArea::create();
        scrollArea->setScrollBarMode(Orientation::Horizontal, ui::ScrollBarMode::Visible);
        scrollArea->setScrollBarMode(Orientation::Vertical, ui::ScrollBarMode::Visible);

        scrollArea->setRectangle(Rectangle{0, 0, 0, 0});
        scrollArea->layout(Size{0, 0}, ui::LayoutContext{});

        REQUIRE_FALSE(scrollArea->horizontalScrollBar()->flags().isVisible());
        REQUIRE_FALSE(scrollArea->verticalScrollBar()->flags().isVisible());
        REQUIRE_FALSE(scrollArea->scrollCorner()->flags().isVisible());
    }

private:
    static void layoutViewport(const ui::ScrollAreaPtr &scrollArea) {
        const auto &viewport = scrollArea->viewport();
        viewport->layout(viewport->rectangle().size(), ui::LayoutContext{});
    }
};
