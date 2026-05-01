// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TerminalTestHelper.hpp"
#include "ui/event/ManualClock.hpp"

#include <erbsland/cterm/theme/Theme.hpp>
#include <erbsland/cterm/theme/ThemeBuilder.hpp>
#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/cterm/ui/event/impl/EventClockAccess.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <chrono>
#include <memory>

class LayoutOrderingChildSurface final : public ui::Surface {
public:
    LayoutOrderingChildSurface() :
        Surface{ui::LayoutMetrics{Size{}, Size::maximum(), Size{}, ui::SizePolicy{ui::SizePolicy::Grow}}} {}

    void onLayout(ui::LayoutScope &scope) noexcept override {
        _layoutCount += 1;
        _layoutSize = scope.size();
    }

    [[nodiscard]] auto layoutCount() const noexcept -> int { return _layoutCount; }
    [[nodiscard]] auto layoutSizeSeen() const noexcept -> Size { return _layoutSize; }

private:
    int _layoutCount{0};
    Size _layoutSize;
};

class LayoutOrderingParentSurface final : public ui::Surface {
public:
    explicit LayoutOrderingParentSurface(std::shared_ptr<LayoutOrderingChildSurface> child) :
        Surface{ui::LayoutMetrics{Size{}, Size::maximum(), Size{}, ui::SizePolicy{ui::SizePolicy::Grow}}},
        _child{std::move(child)} {}

    void onLayout(ui::LayoutScope &scope) noexcept override {
        _childLayoutCountDuringParentLayout = _child->layoutCount();
        scope.place(_child, Rectangle{1, 1, 7, 2});
    }

    [[nodiscard]] auto childLayoutCountDuringParentLayout() const noexcept -> int {
        return _childLayoutCountDuringParentLayout;
    }

private:
    std::shared_ptr<LayoutOrderingChildSurface> _child;
    int _childLayoutCountDuringParentLayout{-1};
};

TESTED_TARGETS(UiLayout)
class UiLayoutRenderTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testDisplayLaysOutParentBeforeChildSubtree() {
        auto page = ui::Page::create();
        auto child = std::make_shared<LayoutOrderingChildSurface>();
        auto parent = std::make_shared<LayoutOrderingParentSurface>(child);
        page->addSurface(parent);
        parent->addSurface(child);

        layoutWithDisplay(page, Size{20, 10});

        REQUIRE_EQUAL(parent->childLayoutCountDuringParentLayout(), 0);
        REQUIRE_EQUAL(child->layoutCount(), 1);
        REQUIRE_EQUAL(child->rectangle(), Rectangle(1, 1, 7, 2));
        REQUIRE_EQUAL(child->layoutSizeSeen(), Size(7, 2));
    }

    void testPageLaysOutFixedAndGrowingChildren() {
        auto page = ui::Page::create();
        auto mainLayout = ui::Stack::create(Orientation::Vertical);
        auto header = ui::Panel::create();
        auto center = ui::Panel::create();
        auto footer = ui::Panel::create();

        header->editLayoutMetrics().setFixedHeight(1);
        footer->editLayoutMetrics().setFixedHeight(1);

        page->addSurface(mainLayout);
        mainLayout->addSurface(header);
        mainLayout->addSurface(center);
        mainLayout->addSurface(footer);

        layoutWithDisplay(page, Size{20, 10});

        REQUIRE_EQUAL(mainLayout->rectangle(), Rectangle(0, 0, 20, 10));
        REQUIRE_EQUAL(header->rectangle(), Rectangle(0, 0, 20, 1));
        REQUIRE_EQUAL(center->rectangle(), Rectangle(0, 1, 20, 8));
        REQUIRE_EQUAL(footer->rectangle(), Rectangle(0, 9, 20, 1));
    }

    void testWidthDependentTextMeasurementMovesLaterStackChildren() {
        auto page = ui::Page::create();
        auto stack = ui::Stack::create(Orientation::Vertical);
        auto text = ui::TextBox::create("abcd efgh ijkl");
        auto footer = ui::Panel::create();

        footer->editLayoutMetrics().setFixedHeight(1);
        page->addSurface(stack);
        stack->addSurface(text);
        stack->addSurface(footer);

        auto setup = createDisplaySetup(page, Size{16, 6});
        renderInitialFrame(setup);

        REQUIRE_EQUAL(text->rectangle(), Rectangle(0, 0, 16, 1));
        REQUIRE_EQUAL(footer->rectangle(), Rectangle(0, 1, 16, 1));

        setup.terminal->setSize(Size{6, 6});
        setup.clock->advance(std::chrono::milliseconds{100});
        setup.display.pollTerminalResize();
        setup.display.pollRender();

        REQUIRE(text->rectangle().height() > Coordinate{1});
        REQUIRE_EQUAL(footer->rectangle().y1(), text->rectangle().height());
    }

    void testHorizontalStackDistributesWidthAcrossGrowingTextBoxes() {
        auto stack = ui::Stack::create(Orientation::Horizontal);
        auto left = ui::TextBox::create("A");
        auto middle = ui::TextBox::create("B", Alignment::Center);
        auto right = ui::TextBox::create("C", Alignment::Right);

        stack->addSurface(left);
        stack->addSurface(middle);
        stack->addSurface(right);

        stack->setRectangle(Rectangle{0, 0, 30, 1});
        stack->layout(Size{30, 1}, ui::LayoutContext{});

        REQUIRE_EQUAL(left->rectangle(), Rectangle(0, 0, 10, 1));
        REQUIRE_EQUAL(middle->rectangle(), Rectangle(10, 0, 10, 1));
        REQUIRE_EQUAL(right->rectangle(), Rectangle(20, 0, 10, 1));
    }

    void testStackIgnoresHiddenChildrenWithoutChangingTheirRectangle() {
        auto stack = ui::Stack::create(Orientation::Vertical);
        auto header = ui::Panel::create();
        auto center = ui::Panel::create();
        auto footer = ui::Panel::create();

        header->editLayoutMetrics().setFixedHeight(1);
        footer->editLayoutMetrics().setFixedHeight(1);
        stack->addSurface(header);
        stack->addSurface(center);
        stack->addSurface(footer);

        stack->setRectangle(Rectangle{0, 0, 12, 6});
        stack->layout(Size{12, 6}, ui::LayoutContext{});
        REQUIRE_EQUAL(center->rectangle(), Rectangle(0, 1, 12, 4));

        center->flags().setVisible(false);
        stack->layout(Size{12, 6}, ui::LayoutContext{});

        REQUIRE_EQUAL(header->rectangle(), Rectangle(0, 0, 12, 1));
        REQUIRE_EQUAL(center->rectangle(), Rectangle(0, 1, 12, 4));
        REQUIRE_EQUAL(footer->rectangle(), Rectangle(0, 1, 12, 1));

        center->flags().setVisible(true);
        stack->layout(Size{12, 6}, ui::LayoutContext{});

        REQUIRE_EQUAL(center->rectangle(), Rectangle(0, 1, 12, 4));
        REQUIRE_EQUAL(footer->rectangle(), Rectangle(0, 5, 12, 1));
    }

    void testNonLayoutParentKeepsManualChildRectButDisplayVisitsNestedLayout() {
        auto page = ui::Page::create();
        auto panel = ui::Panel::create();
        auto stack = ui::Stack::create(Orientation::Vertical);
        auto top = ui::Panel::create();
        auto bottom = ui::Panel::create();

        top->editLayoutMetrics().setFixedHeight(1);

        page->addSurface(panel);
        panel->addSurface(stack);
        stack->addSurface(top);
        stack->addSurface(bottom);
        stack->setRectangle(Rectangle{2, 1, 8, 4});

        layoutWithDisplay(page, Size{12, 6});

        REQUIRE_EQUAL(panel->rectangle(), Rectangle(0, 0, 12, 6));
        REQUIRE_EQUAL(stack->rectangle(), Rectangle(2, 1, 8, 4));
        REQUIRE_EQUAL(top->rectangle(), Rectangle(0, 0, 8, 1));
        REQUIRE_EQUAL(bottom->rectangle(), Rectangle(0, 1, 8, 3));
    }

    void testTextChangeInvalidatesDisplayLayout() {
        auto page = ui::Page::create();
        auto stack = ui::Stack::create(Orientation::Vertical);
        auto text = ui::TextBox::create("Short");
        auto footer = ui::Panel::create();

        footer->editLayoutMetrics().setFixedHeight(1);
        page->addSurface(stack);
        stack->addSurface(text);
        stack->addSurface(footer);

        auto setup = createDisplaySetup(page, Size{6, 6});
        renderInitialFrame(setup);
        REQUIRE_EQUAL(footer->rectangle().y1(), Coordinate{1});

        text->setText(String{"abcd efgh ijkl"});
        renderNextFrame(setup);

        REQUIRE(text->rectangle().height() > Coordinate{1});
        REQUIRE_EQUAL(footer->rectangle().y1(), text->rectangle().height());
    }

    void testLayoutSizeChangeInvalidatesDisplayLayout() {
        auto page = ui::Page::create();
        auto stack = ui::Stack::create(Orientation::Vertical);
        auto header = ui::Panel::create();
        auto footer = ui::Panel::create();

        header->editLayoutMetrics().setFixedHeight(1);
        footer->editLayoutMetrics().setFixedHeight(1);
        page->addSurface(stack);
        stack->addSurface(header);
        stack->addSurface(footer);

        auto setup = createDisplaySetup(page, Size{10, 6});
        renderInitialFrame(setup);
        REQUIRE_EQUAL(footer->rectangle().y1(), Coordinate{1});

        header->editLayoutMetrics().setFixedHeight(3);
        renderNextFrame(setup);

        REQUIRE_EQUAL(header->rectangle(), Rectangle(0, 0, 10, 3));
        REQUIRE_EQUAL(footer->rectangle().y1(), Coordinate{3});
    }

    void testVisibilityChangeInvalidatesDisplayLayout() {
        auto page = ui::Page::create();
        auto stack = ui::Stack::create(Orientation::Vertical);
        auto header = ui::Panel::create();
        auto center = ui::Panel::create();
        auto footer = ui::Panel::create();

        header->editLayoutMetrics().setFixedHeight(1);
        center->editLayoutMetrics().setFixedHeight(3);
        footer->editLayoutMetrics().setFixedHeight(1);
        page->addSurface(stack);
        stack->addSurface(header);
        stack->addSurface(center);
        stack->addSurface(footer);

        auto setup = createDisplaySetup(page, Size{10, 6});
        renderInitialFrame(setup);
        REQUIRE_EQUAL(footer->rectangle().y1(), Coordinate{4});

        center->flags().setVisible(false);
        renderNextFrame(setup);

        REQUIRE_EQUAL(header->rectangle(), Rectangle(0, 0, 10, 1));
        REQUIRE_EQUAL(center->rectangle(), Rectangle(0, 1, 10, 3));
        REQUIRE_EQUAL(footer->rectangle(), Rectangle(0, 1, 10, 1));

        center->flags().setVisible(true);
        renderNextFrame(setup);

        REQUIRE_EQUAL(center->rectangle(), Rectangle(0, 1, 10, 3));
        REQUIRE_EQUAL(footer->rectangle(), Rectangle(0, 4, 10, 1));
    }

    void testStackCollapsesInternalMarginsAndPropagatesOuterMargins() {
        auto stack = ui::Stack::create(Orientation::Vertical);
        auto first = ui::Panel::create();
        auto second = ui::Panel::create();
        first->editLayoutMetrics().setFixedSize(Size{3, 1}).setMargins(Margins{4, 2, 2, 1});
        second->editLayoutMetrics().setFixedSize(Size{4, 1}).setMargins(Margins{5, 3, 6, 2});
        stack->addSurface(first);
        stack->addSurface(second);
        auto scope = measureScope();

        const auto metrics = stack->onMeasure(scope, ui::LayoutProposal::unconstrained());
        stack->layout(Size{10, 10}, ui::LayoutContext{});

        REQUIRE_EQUAL(metrics.preferred(), (Size{4, 7}));
        REQUIRE_EQUAL(metrics.margins(), (Margins{4, 3, 6, 2}));
        REQUIRE_EQUAL(first->rectangle(), Rectangle(0, 0, 3, 1));
        REQUIRE_EQUAL(second->rectangle(), Rectangle(0, 6, 4, 1));
    }

    void testStackShrinksInternalMarginsBeforeChildContent() {
        auto stack = ui::Stack::create(Orientation::Vertical);
        auto first = ui::Panel::create();
        auto second = ui::Panel::create();
        first->editLayoutMetrics().setFixedSize(Size{3, 1}).setMargins(Margins{0, 0, 3, 0});
        second->editLayoutMetrics().setFixedSize(Size{3, 1}).setMargins(Margins{3, 0, 0, 0});
        stack->addSurface(first);
        stack->addSurface(second);

        stack->layout(Size{10, 2}, ui::LayoutContext{});

        REQUIRE_EQUAL(first->rectangle(), Rectangle(0, 0, 3, 1));
        REQUIRE_EQUAL(second->rectangle(), Rectangle(0, 1, 3, 1));
    }

    void testStackIgnoresHiddenChildMargins() {
        auto stack = ui::Stack::create(Orientation::Vertical);
        auto visible = ui::Panel::create();
        auto hidden = ui::Panel::create();
        visible->editLayoutMetrics().setFixedSize(Size{3, 1}).setMargins(Margins{1});
        hidden->editLayoutMetrics().setFixedSize(Size{3, 1}).setMargins(Margins{9});
        hidden->flags().setVisible(false);
        stack->addSurface(visible);
        stack->addSurface(hidden);
        auto scope = measureScope();

        const auto metrics = stack->onMeasure(scope, ui::LayoutProposal::unconstrained());
        stack->layout(Size{10, 10}, ui::LayoutContext{});

        REQUIRE_EQUAL(metrics.preferred(), (Size{3, 1}));
        REQUIRE_EQUAL(metrics.margins(), (Margins{1}));
        REQUIRE_EQUAL(visible->rectangle(), Rectangle(0, 0, 3, 1));
        REQUIRE_EQUAL(hidden->rectangle(), Rectangle{});
    }

    void testNestedStacksCollapsePropagatedCrossAxisMargins() {
        auto row = ui::Stack::create(Orientation::Horizontal);
        auto leftColumn = ui::Stack::create(Orientation::Vertical);
        auto rightColumn = ui::Stack::create(Orientation::Vertical);
        auto left = ui::Panel::create();
        auto right = ui::Panel::create();
        leftColumn->editLayoutMetrics().setSizePolicy(ui::SizePolicy::Preferred);
        rightColumn->editLayoutMetrics().setSizePolicy(ui::SizePolicy::Preferred);
        left->editLayoutMetrics().setFixedSize(Size{2, 1}).setMargins(Margins{0, 4, 0, 0});
        right->editLayoutMetrics().setFixedSize(Size{2, 1}).setMargins(Margins{0, 0, 0, 6});
        leftColumn->addSurface(left);
        rightColumn->addSurface(right);
        row->addSurface(leftColumn);
        row->addSurface(rightColumn);

        row->layout(Size{20, 2}, ui::LayoutContext{});

        REQUIRE_EQUAL(leftColumn->rectangle(), Rectangle(0, 0, 2, 1));
        REQUIRE_EQUAL(rightColumn->rectangle(), Rectangle(8, 0, 2, 1));
    }

    void testDisplayRendersTheInitialPageContent() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{12, 3});
        terminal->setOutputMode(Terminal::OutputMode::Text);

        auto page = ui::Page::create();
        page->setTheme(pageTheme(U'.'));
        page->addSurface(ui::TextBox::create("HI"));

        auto display = ui::Display{std::move(terminal), page, exactTerminalSizeLimits()};
        display.pollRender();

        REQUIRE_EQUAL(backend->output(), std::string{"HI          \n............\n............\n"});
    }

private:
    struct DisplaySetup final {
        std::shared_ptr<ManualClock> clock;
        ui::impl::ScopedEventClockOverride clockOverride;
        std::shared_ptr<TerminalTestBackend> backend;
        std::shared_ptr<Terminal> terminal;
        ui::Display display;
    };

    [[nodiscard]] static auto createDisplaySetup(const ui::PagePtr &page, const Size size) -> DisplaySetup {
        auto clock = std::make_shared<ManualClock>();
        auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, size);
        terminal->setOutputMode(Terminal::OutputMode::Text);
        return DisplaySetup{
            .clock = clock,
            .clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }},
            .backend = backend,
            .terminal = terminal,
            .display = ui::Display{terminal, page, exactTerminalSizeLimits()},
        };
    }

    static void renderInitialFrame(DisplaySetup &setup) {
        setup.display.pollRender();
        setup.backend->clearOutput();
    }

    static void renderNextFrame(DisplaySetup &setup) {
        setup.clock->advance(std::chrono::milliseconds{100});
        setup.display.pollRender();
        setup.backend->clearOutput();
    }

    static void layoutWithDisplay(const ui::PagePtr &page, const Size size) {
        auto setup = createDisplaySetup(page, size);
        renderInitialFrame(setup);
    }

    [[nodiscard]] static auto measureScope() -> ui::MeasureScope {
        return ui::MeasureScope{[](const ui::SurfacePtr &surface, const ui::LayoutProposal &proposal) {
            auto childScope = ui::MeasureScope{};
            return surface->onMeasure(childScope, proposal);
        }};
    }

    [[nodiscard]] static auto pageTheme(const char32_t backgroundBlock) -> theme::ThemeConstPtr {
        auto builder = theme::ThemeBuilder::zero();
        builder.edit(theme::Selector{theme::Element::Page, theme::Part::Background}).setBlocks(backgroundBlock);
        return builder.build();
    }
};
