// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/StringTestHelper.hpp"

#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <string>

TESTED_TARGETS(UiHtmlHelpSection UiActionHelpSection)
class UiHelpSectionTest final : public UNITTEST_SUBCLASS(StringTestHelper) {
public:
    void testHtmlHelpSectionStoresTitleAndRendersHtml() {
        auto section = ui::HtmlHelpSection::create();
        section->setTitle("Overview");
        section->setHtml("<h1>Title</h1><p>Hello</p>");
        section->setRectangle(Rectangle{0, 0, 20, 4});
        section->layout(Size{20, 4}, ui::LayoutContext{});

        REQUIRE_EQUAL(section->title(), std::string_view{"Overview"});
        REQUIRE_EQUAL(section->html(), std::string{"<h1>Title</h1><p>Hello</p>"});
        REQUIRE(section->contentSize().width() >= Coordinate{20});
        REQUIRE(section->contentSize().height() >= Coordinate{2});
    }

    void testActionHelpSectionFiltersSortsDeduplicatesAndEscapesActions() {
        auto page = ui::Page::create();
        auto child = ui::Panel::create();
        child->flags().setFocusable(true);
        page->addSurface(child);
        page->focusTo(child);

        auto shared = ui::Action::create("Shared");
        shared->setKeys(Keys{U's', U'S'}.setMainKeyCount(1));
        shared->help().setDescription("Deduped action");
        child->actions().add(shared);
        page->actions().add(shared);

        auto high = ui::Action::create("High <Action>");
        high->setKeys(Keys{Key::Enter, U'h', U'H'}.setMainKeyCount(1));
        high->help().setDescription("Use & enjoy");
        high->help().setPriority(100);
        child->actions().add(high);

        auto htmlAction = ui::Action::create("HTML");
        htmlAction->setKeys(U'<');
        htmlAction->help().setDescription("Use <strong>rich</strong> help.", ui::HelpFormat::Html);
        child->actions().add(htmlAction);

        auto hidden = ui::Action::create("Hidden");
        hidden->setKeys(U'x');
        hidden->help().setVisibility(ui::HelpVisibility::Footer);
        child->actions().add(hidden);

        auto disabled = ui::Action::create("Disabled");
        disabled->setKeys(U'd');
        disabled->setEnabled(false);
        page->actions().add(disabled);

        auto pageOnly = ui::Action::create("Page Only");
        pageOnly->setKeys(U'p');
        pageOnly->help().setVisibility(ui::HelpVisibility::HelpPage);
        page->actions().add(pageOnly);

        auto section = ui::ActionHelpSection::create();
        section->setActionSource(child);
        const auto &html = section->html();

        REQUIRE(html.find("High &lt;Action&gt;") < html.find("Shared"));
        REQUIRE(html.find("Shared") < html.find("Page Only"));
        REQUIRE(html.find("Hidden") == std::string::npos);
        REQUIRE(html.find("Disabled") == std::string::npos);
        REQUIRE(html.find("<span class=\"key\">↵</span>") != std::string::npos);
        REQUIRE(html.find("<span class=\"key\">h</span>/<span class=\"key\">H</span>") != std::string::npos);
        REQUIRE(html.find("<span class=\"key\">&lt;</span>") != std::string::npos);
        REQUIRE(html.find("Use &amp; enjoy") != std::string::npos);
        REQUIRE(html.find("Use <strong>rich</strong> help.") != std::string::npos);
        REQUIRE_EQUAL(high->help().descriptionFormat(), ui::HelpFormat::Text);
        REQUIRE_EQUAL(htmlAction->help().descriptionFormat(), ui::HelpFormat::Html);
        REQUIRE_EQUAL(html.find("Shared"), html.rfind("Shared"));
    }
};
