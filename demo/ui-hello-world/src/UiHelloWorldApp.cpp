// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "UiHelloWorldApp.hpp"

#include <chrono>
#include <format>

namespace demo::ui_hello_world {

auto UiHelloWorldApp::currentDateTimeText() -> String {
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
    mainPage->keyBindings().bind(Key{Key::Character, U'q'}, []() -> void { ui::getApplication().quit(); });
    _app.setMainPage(mainPage);

    auto mainLayout = ui::Stack::create(Orientation::Vertical);
    mainPage->addChild(mainLayout);

    auto header = ui::Stack::create(Orientation::Horizontal);
    header->geometry().setFixedHeight(1);
    header->setBackground(Char(U' ', bg::Blue));
    mainLayout->addChild(header);

    auto demoName = ui::TextBox::create("Hello World!");
    header->addChild(demoName);

    auto clockText = ui::TextBox::create(currentDateTimeText(), Alignment::Center);
    clockText->scheduler().addRepeated(
        [clockText]() -> void { clockText->setText(currentDateTimeText()); }, std::chrono::seconds{1});
    header->addChild(clockText);

    auto topRight = ui::TextBox::create("UI Demo", Alignment::Right);
    header->addChild(topRight);

    auto center = ui::Panel::create();
    mainLayout->addChild(center);

    auto footer = ui::Stack::create(Orientation::Horizontal);
    footer->geometry().setFixedHeight(1);
    footer->setBackground(Char(U' ', bg::BrightBlack));
    mainLayout->addChild(footer);

    auto quitHint = ui::TextBox::create("Press q to quit");
    footer->addChild(quitHint);
}


}
