// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/BufferTestHelper.hpp"

#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(UiCentered UiFrame UiLayout)
class UiCenteredFrameTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testCenteredAcceptsGenericSingleContentMutation() {
        auto centered = ui::Centered::create();
        auto child = ui::Panel::create();

        centered->surfaces().add(child);

        REQUIRE_EQUAL(centered->contentSurface(), child);
        REQUIRE_EQUAL(child->parent().lock(), centered);
        REQUIRE_THROWS(centered->surfaces().add(ui::Panel::create()));

        centered->surfaces().remove(child);

        REQUIRE_EQUAL(centered->contentSurface(), nullptr);
        REQUIRE(child->parent().expired());
    }

    void testSingleContentLayoutsReparentContentThroughGenericMutation() {
        auto centered = ui::Centered::create();
        auto newParent = ui::Stack::create(Orientation::Vertical);
        auto child = ui::Panel::create();
        centered->setContentSurface(child);

        newParent->surfaces().add(child);

        REQUIRE_EQUAL(centered->contentSurface(), nullptr);
        REQUIRE(centered->surfaces().empty());
        REQUIRE_EQUAL(child->parent().lock(), newParent);
        REQUIRE_EQUAL(newParent->surfaces().at(0), child);
    }

    void testFrameAcceptsGenericSingleContentMutation() {
        auto frame = ui::Frame::create();
        auto child = ui::Panel::create();
        auto replacement = ui::Panel::create();

        frame->surfaces().add(child);
        frame->surfaces().replace(child, replacement);

        REQUIRE_EQUAL(frame->contentSurface(), replacement);
        REQUIRE(child->parent().expired());
        REQUIRE_EQUAL(replacement->parent().lock(), frame);
    }

    void testEmptySingleContentLayoutsAreStable() {
        auto centered = ui::Centered::create();
        auto frame = ui::Frame::create();
        auto scope = ui::MeasureScope{};
        auto buffer = Buffer{Size{8, 3}, Char{U'.'}};
        const auto context = frame->themeContextFrom(ui::ThemeContext{});

        REQUIRE_NOTHROW(centered->onMeasure(scope, ui::LayoutProposal::atMost(Size{8, 3})));
        REQUIRE_NOTHROW(centered->layout(Size{8, 3}, ui::LayoutContext{}));
        REQUIRE_NOTHROW(frame->onMeasure(scope, ui::LayoutProposal::atMost(Size{8, 3})));
        REQUIRE_NOTHROW(frame->layout(Size{8, 3}, ui::LayoutContext{}));
        REQUIRE_NOTHROW(frame->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), context}));
    }

    void testCenteredPlacesOneChildInPaddedArea() {
        auto centered = ui::Centered::create();
        auto child = ui::Panel::create();
        child->editLayoutMetrics().setFixedSize(Size{4, 2});
        centered->setPadding(Margins{1});
        centered->setContentSurface(child);

        centered->setRectangle(Rectangle{0, 0, 20, 10});
        centered->layout(Size{20, 10}, ui::LayoutContext{});

        REQUIRE_EQUAL(child->rectangle(), Rectangle(8, 4, 4, 2));
    }

    void testCenteredFallsBackWhenPaddingConsumesTheWholeArea() {
        auto centered = ui::Centered::create();
        auto child = ui::Panel::create();
        child->editLayoutMetrics().setFixedSize(Size{4, 2});
        centered->setPadding(Margins{100});
        centered->setContentSurface(child);

        centered->setRectangle(Rectangle{0, 0, 10, 6});
        centered->layout(Size{10, 6}, ui::LayoutContext{});

        REQUIRE_EQUAL(child->rectangle(), Rectangle(3, 2, 4, 2));
    }

    void testFramePlacesContentInsideBorderAndPadding() {
        auto frame = ui::Frame::create();
        auto child = ui::Panel::create();
        frame->setPadding(Margins{1});
        frame->setContentSurface(child);

        frame->setRectangle(Rectangle{0, 0, 12, 6});
        frame->layout(Size{12, 6}, ui::LayoutContext{});

        REQUIRE_EQUAL(child->rectangle(), Rectangle(2, 2, 8, 2));
    }

    void testFrameCollapsesPaddingWithChildMargins() {
        auto frame = ui::Frame::create();
        auto child = ui::Panel::create();
        child->editLayoutMetrics().setMargins(Margins{3});
        frame->setPadding(Margins{1});
        frame->setContentSurface(child);

        frame->setRectangle(Rectangle{0, 0, 14, 10});
        frame->layout(Size{14, 10}, ui::LayoutContext{});

        REQUIRE_EQUAL(child->rectangle(), Rectangle(4, 4, 6, 2));
    }

    void testFrameMeasuresContentWithBorderAndPadding() {
        auto frame = ui::Frame::create();
        auto child = ui::Panel::create();
        child->editLayoutMetrics().setFixedSize(Size{4, 2});
        frame->setPadding(Margins{2});
        frame->setContentSurface(child);
        auto childScope = ui::MeasureScope{};
        auto scope = ui::MeasureScope{[&](const ui::SurfacePtr &surface, const ui::LayoutProposal &proposal) {
            return surface->onMeasure(childScope, proposal);
        }};

        const auto metrics = frame->onMeasure(scope, ui::LayoutProposal::unconstrained());

        REQUIRE_EQUAL(metrics.preferred(), (Size{10, 8}));
    }

    void testFrameMeasuresCollapsedChildMarginsAsInset() {
        auto frame = ui::Frame::create();
        auto child = ui::Panel::create();
        child->editLayoutMetrics().setFixedSize(Size{4, 2}).setMargins(Margins{3});
        frame->setPadding(Margins{1});
        frame->setContentSurface(child);
        auto childScope = ui::MeasureScope{};
        auto scope = ui::MeasureScope{[&](const ui::SurfacePtr &surface, const ui::LayoutProposal &proposal) {
            return surface->onMeasure(childScope, proposal);
        }};

        const auto metrics = frame->onMeasure(scope, ui::LayoutProposal::unconstrained());

        REQUIRE_EQUAL(metrics.preferred(), (Size{12, 10}));
        REQUIRE_EQUAL(metrics.margins(), Margins{});
    }

    void testFramePaintsThemedBorderAndTitle() {
        auto frame = ui::Frame::create();
        frame->setTitle(String{"Box"});
        frame->setRectangle(Rectangle{0, 0, 16, 4});
        const auto context = frame->themeContextFrom(ui::ThemeContext{});
        auto buffer = Buffer{Size{16, 4}, Char{U'.'}};

        frame->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), context});

        REQUIRE(buffer.get(Position{0, 0}) == U'┌');
        REQUIRE(buffer.get(Position{2, 0}) != Char{U'.'});
        REQUIRE(buffer.get(Position{15, 3}) == U'┘');
    }
};
