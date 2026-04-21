// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "UiChoiceApp.hpp"

#include <format>

namespace demo::ui_choice {

void UiChoiceApp::setupUi() {
    auto page = ui::Page::create();
    addDefaultActions(page);

    auto openAction = ui::ButtonAction::create("Open Choice");
    openAction->setKeys(Keys{U'c', Key::Enter}.setMainKeyCount(1));
    openAction->setTriggerFn([this]() -> void { openChoice(); });
    openAction->help().setPriority(100);
    openAction->help().setDescription("Open the choice example dialog.");
    page->actions().add(openAction);

    auto root = ui::Stack::create(Orientation::Vertical);
    page->addSurface(root);

    auto header = ui::HeaderLine::create();
    header->setText(ui::TextLine::Section::Left, String{"Choice Dialog", fg::BrightWhite});
    header->setMargins(ui::TextLine::Section::Left, Margins{1, 0});
    header->setText(ui::TextLine::Section::Right, String{"composition demo", fg::BrightYellow});
    header->setMargins(ui::TextLine::Section::Right, Margins{1, 0});
    root->addSurface(header);

    auto frame = ui::Frame::create();
    frame->setTitle(String{"Main Page"});
    frame->setPadding(Margins{1, 2});
    frame->editLayoutMetrics().setSizePolicy({ui::SizePolicy::Grow, ui::SizePolicy::Grow});
    root->addSurface(frame);

    auto body = ui::Stack::create(Orientation::Vertical);
    frame->setContentSurface(body);

    auto intro = ui::TextBox::create(String{"This demo app opens a modal choice overlay.", fg::BrightWhite});
    intro->setTextOptions(TextOptions{Alignment::Center});
    intro->editLayoutMetrics().setSizePolicy(ui::SizePolicy::Grow);
    body->addSurface(intro);

    auto buttons = ui::Buttons::create();
    buttons->addAction(openAction);
    buttons->editLayoutMetrics().setFixedHeight(1);
    body->addSurface(buttons);

    _status = ui::TextBox::create(String{"No selection yet.", fg::BrightCyan}, Alignment::Center);
    _status->editLayoutMetrics().setSizePolicy(ui::SizePolicy::Grow);
    body->addSurface(_status);

    auto footer = ui::FooterLine::create();
    root->addSurface(footer);
    setMainPage(page);
}

void UiChoiceApp::openChoice() {
    auto choice =
        ui::Choice::create("Pick a Direction", "Please pick one of the directions below, or cancel with escape.");
    choice->addChoice("north", "North", U'n', 30);
    choice->addChoice("east", "East", U'e', 20);
    choice->addChoice("south", "South", U's', 10);
    choice->addChoice("cancel", "Cancel", Key::Escape, -10);
    choice->setCallback([status = _status](const ui::Choice::Selection &selection) -> void {
        status->setText(
            String{
                std::format("Selected id: {} (insertion index: {})", selection.id, selection.index), fg::BrightGreen});
    });
    choice->actions().add(helpAction());
    choice->show();
}

}
