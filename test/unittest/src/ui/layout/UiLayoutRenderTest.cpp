// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TerminalTestSupport.hpp"

#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <memory>

namespace ui = erbsland::cterm::ui;


TESTED_TARGETS(UiLayout)
class UiLayoutRenderTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testPageLaysOutFixedAndGrowingChildren() {
        auto page = ui::Page::create();
        auto mainLayout = ui::Stack::create(Orientation::Vertical);
        auto header = ui::Panel::create();
        auto center = ui::Panel::create();
        auto footer = ui::Panel::create();

        header->geometry().setFixedHeight(1);
        footer->geometry().setFixedHeight(1);

        page->addChild(mainLayout);
        mainLayout->addChild(header);
        mainLayout->addChild(center);
        mainLayout->addChild(footer);

        page->setRectangle(Rectangle{0, 0, 20, 10});
        page->onLayout(Size{20, 10});

        REQUIRE_EQUAL(mainLayout->rectangle(), Rectangle(0, 0, 20, 10));
        REQUIRE_EQUAL(header->rectangle(), Rectangle(0, 0, 20, 1));
        REQUIRE_EQUAL(center->rectangle(), Rectangle(0, 1, 20, 8));
        REQUIRE_EQUAL(footer->rectangle(), Rectangle(0, 9, 20, 1));
    }

    void testHorizontalStackDistributesWidthAcrossGrowingTextBoxes() {
        auto stack = ui::Stack::create(Orientation::Horizontal);
        auto left = ui::TextBox::create("A");
        auto middle = ui::TextBox::create("B", Alignment::Center);
        auto right = ui::TextBox::create("C", Alignment::Right);

        stack->addChild(left);
        stack->addChild(middle);
        stack->addChild(right);

        stack->setRectangle(Rectangle{0, 0, 30, 1});
        stack->onLayout(Size{30, 1});

        REQUIRE_EQUAL(left->rectangle(), Rectangle(0, 0, 10, 1));
        REQUIRE_EQUAL(middle->rectangle(), Rectangle(10, 0, 10, 1));
        REQUIRE_EQUAL(right->rectangle(), Rectangle(20, 0, 10, 1));
    }

    void testPanelUsesGenericSurfaceLayoutForChildren() {
        auto panel = ui::Panel::create();
        auto child = ui::TextBox::create("Hello");
        child->geometry().setFixedHeight(2);

        panel->addChild(child);
        panel->setRectangle(Rectangle{0, 0, 12, 4});
        panel->onLayout(Size{12, 4});

        REQUIRE_EQUAL(child->rectangle(), Rectangle(0, 0, 12, 2));
    }

    void testDisplayRendersTheInitialPageContent() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{12, 3});
        terminal->setOutputMode(Terminal::OutputMode::Text);

        auto page = ui::Page::create();
        page->setBackground(Char{U'.'});
        page->addChild(ui::TextBox::create("HI"));

        auto display = ui::Display(std::move(terminal), page);
        display.pollRender();

        REQUIRE_EQUAL(backend->output(), std::string{"HI..........\n............\n............\n"});
    }
};
