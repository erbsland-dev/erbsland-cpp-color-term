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

void UiHelloWorldApp::run() {
    _app.terminal().setSafeMarginEnabled(false);
    setupUi();
    _app.run();
}

void UiHelloWorldApp::setupUi() {
    auto mainPage = ui::Page::create();
    mainPage->setBackground(Char{U'╱', fg::BrightBlack, bg::Black});
    mainPage->keyBindings().bind({U'q', U'Q'}, []() -> void { ui::getApplication().quit(); });
    mainPage->keyBindings().bind(Key::Escape, []() -> void { ui::getApplication().quit(); });
    _app.setMainPage(mainPage);

    auto mainLayout = ui::Stack::create(Orientation::Vertical);
    mainPage->addChild(mainLayout);

    using Section = ui::StatusLine::Section;
    using CollapseBehavior = ui::StatusLine::CollapseBehavior;
    using UpdateMode = ui::StatusLine::UpdateMode;
    auto header = ui::StatusLine::create();
    header->setBackground(Char{U' ', bg::Blue});
    header->setText(Section::Left, String{"Hello World!", Color{fg::BrightWhite, bg::Blue}});
    header->setMargins(Section::Left, Margins{0, 1, 0, 1});
    header->setUpdateMode(Section::Middle, UpdateMode::OnRefresh);
    header->setUpdateFn(Section::Middle, [](String &text, const Coordinate) -> void {
        text.clear();
        text.append(Color{fg::BrightCyan, bg::Blue}, currentDateTimeText());
    });
    header->setCollapseBehavior(Section::Middle, CollapseBehavior::Hide);
    header->setMargins(Section::Middle, Margins{0, 1, 0, 1});
    header->setText(Section::Right, String{"Minimal UI Demo", Color{fg::BrightYellow, bg::Blue}});
    header->setMargins(Section::Right, Margins{0, 1, 0, 1});
    header->scheduler().addRepeated([header]() -> void { header->setPaintOutdated(); }, std::chrono::seconds{1});
    mainLayout->addChild(header);

    auto center = ui::TextBox::create(
        String{
            "Hello World!\n"
            "\n"
            "A complete full-screen terminal UI with centered content,\n"
            "live status lines, and key handling in just a few surfaces.",
            Color{fg::BrightWhite, bg::Inherited}},
        Alignment::Center);
    center->geometry().setSizePolicy(ui::SizePolicy{ui::SizePolicy::Grow});
    mainLayout->addChild(center);

    auto footer = ui::StatusLine::create();
    footer->setBackground(Char{U' ', bg::BrightBlack});
    footer->setText(
        Section::Left, String{"Built from Page + Stack + TextBox + StatusLine", Color{fg::White, bg::BrightBlack}});
    footer->setCollapseBehavior(Section::Left, CollapseBehavior::Hide);
    footer->setMargins(Section::Left, Margins{0, 1, 0, 1});
    footer->setText(Section::Right, String{"[Q] or [Esc] quit", Color{fg::BrightYellow, bg::BrightBlack}});
    footer->setMargins(Section::Right, Margins{0, 1, 0, 1});
    mainLayout->addChild(footer);
}

}
