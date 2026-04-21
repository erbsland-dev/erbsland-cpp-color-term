// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "UiHelloWorldApp.hpp"

#include <chrono>
#include <format>

namespace demo::ui_hello_world {

auto currentDateTimeText() -> String {
    const auto now = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
    return String{std::format("{:%F %T}", now)};
}

void UiHelloWorldApp::setupUi() {
    auto mainPage = ui::Page::create();
    auto quitAction = ui::Action::create("Quit");
    quitAction->setKeys(Keys{U'q', Key::Escape}.setMainKeyCount(1));
    quitAction->setTriggerFn([]() -> void { ui::getApplication().quit(); });
    quitAction->help().setPriority(-100);
    mainPage->actions().add(quitAction);

    auto mainLayout = ui::Stack::create(Orientation::Vertical);
    mainPage->addSurface(mainLayout);

    using Section = ui::TextLine::Section;
    using CollapseBehavior = ui::TextLine::CollapseBehavior;
    using UpdateMode = ui::TextLine::UpdateMode;
    auto header = ui::HeaderLine::create();
    header->setText(Section::Left, String{"Hello World!", fg::BrightWhite});
    header->setMargins(Section::Left, Margins{1, 0});
    header->setUpdateMode(Section::Middle, UpdateMode::OnRefresh);
    header->setUpdateFn(Section::Middle, [](String &text, const Coordinate) -> void {
        text.clear();
        text.append(fg::BrightCyan, currentDateTimeText());
    });
    header->setCollapseBehavior(Section::Middle, CollapseBehavior::Hide);
    header->setMargins(Section::Middle, Margins{1, 0});
    header->setText(Section::Right, String{"Minimal UI Demo", fg::BrightYellow});
    header->setMargins(Section::Right, Margins{1, 0});
    header->scheduler().addRepeated(
        [header]() -> void { header->flags().setPaintOutdated(); }, std::chrono::seconds{1});
    mainLayout->addSurface(header);

    auto center = ui::TextBox::create(
        String{
            "Hello World!\n"
            "\n"
            "A complete full-screen terminal UI with centered content,\n"
            "live status lines, and key handling in just a few surfaces.",
            fg::BrightWhite},
        Alignment::Center);
    center->editLayoutMetrics().setSizePolicy(ui::SizePolicy::Grow);
    mainLayout->addSurface(center);

    auto footer = ui::FooterLine::create();
    footer->setText(String{"Built via Stack[Header, TextBox, Footer]", fg::White});
    mainLayout->addSurface(footer);
    setMainPage(mainPage);
}

}
