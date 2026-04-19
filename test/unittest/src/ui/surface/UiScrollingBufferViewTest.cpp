// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "BufferTestHelper.hpp"

#include <erbsland/cterm/ui/surface/ScrollingBufferView.hpp>
#include <erbsland/unittest/UnitTest.hpp>

namespace ui = erbsland::cterm::ui;

TESTED_TARGETS(UiScrollingBufferView)
class UiScrollingBufferViewTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testScrollingBufferViewRendersTheVisibleViewportAndSupportsTwoAxisScrolling() {
        auto source = createSharedBuffer({
            "12345",
            "ABCDE",
            "uvwxy",
            "klmno",
        });
        auto view = ui::ScrollingBufferView::create(source);
        auto buffer = Buffer{Size{3, 2}, Char{U'.'}};

        view->setRectangle(Rectangle{0, 0, 3, 2});
        view->onLayout(Size{3, 2});
        view->onPaint(buffer, ui::PaintContext{buffer.rect()});

        requireRowsEqual(
            buffer,
            {
                "123",
                "ABC",
            });
        REQUIRE_EQUAL(view->viewRect(), Rectangle(0, 0, 3, 2));

        buffer.fill(Char{U'.'});
        view->scrollDown();
        view->scrollRight(2);
        view->onPaint(buffer, ui::PaintContext{buffer.rect()});

        requireRowsEqual(
            buffer,
            {
                "CDE",
                "wxy",
            });
        REQUIRE_EQUAL(view->viewOffset(), Position(2, 1));
        REQUIRE_EQUAL(view->viewRect(), Rectangle(2, 1, 3, 2));
    }

    void testScrollingBufferViewClampsPageScrollingAndEdgeJumps() {
        auto source = createSharedBuffer({
            "123456",
            "ABCDEF",
            "uvwxyz",
            "ghijkl",
            "MNOPQR",
        });
        auto view = ui::ScrollingBufferView::create(source);

        view->setRectangle(Rectangle{0, 0, 4, 2});
        view->onLayout(Size{4, 2});

        view->pageRight();
        REQUIRE_EQUAL(view->viewOffset(), Position(2, 0));

        view->pageDown();
        REQUIRE_EQUAL(view->viewOffset(), Position(2, 2));

        view->pageDown();
        REQUIRE_EQUAL(view->viewOffset(), Position(2, 3));

        view->scrollToTop();
        REQUIRE_EQUAL(view->viewOffset(), Position(2, 0));

        view->scrollToLeftEdge();
        REQUIRE_EQUAL(view->viewOffset(), Position(0, 0));

        view->scrollToBottom();
        REQUIRE_EQUAL(view->viewOffset(), Position(0, 3));

        view->scrollToRightEdge();
        REQUIRE_EQUAL(view->viewOffset(), Position(2, 3));
    }
};
