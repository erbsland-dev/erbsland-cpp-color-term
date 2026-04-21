// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/StringTestHelper.hpp"
#include "support/TerminalTestHelper.hpp"

#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>
#include <string>

class CountingHelpSection final : public ui::AbstractHelpSection {
public:
    CountingHelpSection(std::string title, ProtectedTag protectedTag) :
        AbstractHelpSection{protectedTag}, _title{std::move(title)} {}

    [[nodiscard]] static auto create(std::string title) -> std::shared_ptr<CountingHelpSection> {
        return std::make_shared<CountingHelpSection>(std::move(title), ProtectedTag{});
    }

public:
    [[nodiscard]] auto title() const noexcept -> std::string_view override { return _title; }

    void scrollDown(Coordinate count = 1) noexcept override { _scrollDownCount += count; }
    void pageDown() noexcept override { _pageDownCount += 1; }
    void scrollToTop() noexcept override { _scrollToTopCount += 1; }

    [[nodiscard]] auto scrollDownCount() const noexcept -> Coordinate { return _scrollDownCount; }
    [[nodiscard]] auto pageDownCount() const noexcept -> int { return _pageDownCount; }
    [[nodiscard]] auto scrollToTopCount() const noexcept -> int { return _scrollToTopCount; }

private:
    std::string _title;
    Coordinate _scrollDownCount{0};
    int _pageDownCount{0};
    int _scrollToTopCount{0};
};

TESTED_TARGETS(UiHelpViewer UiDisplay)
class UiHelpViewerTest final : public UNITTEST_SUBCLASS(StringTestHelper) {
public:
    void testHelpViewerNavigationUpdatesHeaderAndFooter() {
        auto viewer = ui::HelpViewer::create();
        auto first = CountingHelpSection::create("First");
        auto second = CountingHelpSection::create("Second");
        viewer->addSection(first);
        viewer->addSection(second);

        REQUIRE_EQUAL(render(viewer->header()->text(ui::TextLine::Section::Left)), "First");
        REQUIRE_EQUAL(render(viewer->footer()->leftText()->text()), "1 / 2");

        auto nextEvent = ui::KeyPressEvent{Key{Key::Right}};
        viewer->onKeyPress(nextEvent);

        REQUIRE(nextEvent.isHandled());
        REQUIRE_EQUAL(viewer->pages()->currentPage(), 1);
        REQUIRE_EQUAL(render(viewer->header()->text(ui::TextLine::Section::Left)), "Second");
        REQUIRE_EQUAL(render(viewer->footer()->leftText()->text()), "2 / 2");
        REQUIRE_EQUAL(second->scrollToTopCount(), 1);
    }

    void testHelpViewerScrollKeysAffectCurrentSectionOnly() {
        auto viewer = ui::HelpViewer::create();
        auto first = CountingHelpSection::create("First");
        auto second = CountingHelpSection::create("Second");
        viewer->addSection(first);
        viewer->addSection(second);

        auto downEvent = ui::KeyPressEvent{Key{Key::Down}};
        viewer->onKeyPress(downEvent);
        auto pageDownEvent = ui::KeyPressEvent{Key{Key::PageDown}};
        viewer->onKeyPress(pageDownEvent);

        REQUIRE(downEvent.isHandled());
        REQUIRE(pageDownEvent.isHandled());
        REQUIRE_EQUAL(first->scrollDownCount(), Coordinate{1});
        REQUIRE_EQUAL(first->pageDownCount(), 1);
        REQUIRE_EQUAL(second->scrollDownCount(), Coordinate{0});
        REQUIRE_EQUAL(second->pageDownCount(), 0);
    }

    void testHelpViewerShowPushesAndClosePopsPage() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{50, 12});
        auto application = ui::Application{terminal};
        auto mainPage = ui::Page::create();
        application.setMainPage(mainPage);
        REQUIRE_EQUAL(application.manualInitialize(), ui::Application::cExitCodeContinue);

        auto viewer = ui::HelpViewer::create();
        viewer->addSection(CountingHelpSection::create("First"));
        viewer->show();
        application.display().popPage();
        REQUIRE_THROWS_AS(std::logic_error, application.display().popPage());

        viewer->show();
        auto escapeEvent = ui::KeyPressEvent{Key{Key::Escape}};
        application.display().onKeyPress(escapeEvent);
        REQUIRE(escapeEvent.isHandled());
        REQUIRE_THROWS_AS(std::logic_error, application.display().popPage());

        application.manualShutdown();
    }
};
