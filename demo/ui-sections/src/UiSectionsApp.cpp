// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "UiSectionsApp.hpp"

#include <array>
#include <format>

namespace demo::ui_sections {

void UiSectionsApp::setupUi() {
    auto page = ui::Page::create();
    addDefaultActions(page);

    auto toggleDetailsAction = ui::Action::create("Details");
    toggleDetailsAction->help().setDescription("Toggle visibility of the details section.");
    toggleDetailsAction->setKeys(Keys{U'd'});
    toggleDetailsAction->setTriggerFn([this]() -> void { toggleDetails(); });
    toggleDetailsAction->help().setPriority(90);
    page->actions().add(toggleDetailsAction);

    auto trailingAction = ui::Action::create("Separator");
    trailingAction->help().setDescription("Toggle visibility of the trailing separator.");
    trailingAction->setKeys(Keys{U't'});
    trailingAction->setTriggerFn([this]() -> void { toggleTrailingSeparator(); });
    trailingAction->help().setPriority(80);
    page->actions().add(trailingAction);

    auto rightTextAction = ui::Action::create("Right Text");
    rightTextAction->help().setDescription("Cycle through different right text options.");
    rightTextAction->setKeys(Keys{U'r'});
    rightTextAction->setTriggerFn([this]() -> void { cycleRightText(); });
    rightTextAction->help().setPriority(70);
    page->actions().add(rightTextAction);

    auto root = ui::Stack::create(Orientation::Vertical);
    page->addSurface(root);

    auto header = ui::HeaderLine::create();
    header->setText(ui::TextLine::Section::Left, String{"Sections Layout", fg::BrightWhite});
    header->setMargins(ui::TextLine::Section::Left, Margins{1, 0});
    header->setText(ui::TextLine::Section::Right, String{"interactive demo", fg::BrightYellow});
    header->setMargins(ui::TextLine::Section::Right, Margins{1, 0});
    root->addSurface(header);

    _sections = ui::Sections::create();
    root->addSurface(_sections);

    auto overview = ui::TextBox::create(
        String{
            "The Sections layout reserves title separator rows and stacks the visible bodies between them.\n"
            "Use the keys in the footer to hide a section, remove the trailing separator, and change right text.",
            fg::BrightWhite});
    overview->editLayoutMetrics().setFixedHeight(3);
    _sections->addSection(overview, ui::SectionOptions{String{"Overview"}, String{"always visible"}});

    _detailsSection = ui::TextBox::create(
        String{
            "This body can be hidden at runtime. The following section moves up automatically because visibility "
            "invalidates layout.",
            fg::BrightCyan});
    _detailsSection->editLayoutMetrics().setFixedHeight(3);
    _sections->addSection(_detailsSection, ui::SectionOptions{String{"Details"}, String{"press d"}});

    _stateText = ui::TextBox::create(String{});
    _stateText->editLayoutMetrics().setSizePolicy(ui::SizePolicy::Grow);
    _sections->addSection(_stateText, ui::SectionOptions{String{"State"}});

    _footer = ui::FooterLine::create();
    root->addSurface(_footer);

    updateStateText();
    setMainPage(page);
}

void UiSectionsApp::toggleDetails() noexcept {
    _detailsVisible = !_detailsVisible;
    _detailsSection->flags().setVisible(_detailsVisible);
    updateStateText();
}

void UiSectionsApp::toggleTrailingSeparator() noexcept {
    _trailingSeparator = !_trailingSeparator;
    _sections->setTrailingSeparatorVisible(_trailingSeparator);
    updateStateText();
}

void UiSectionsApp::cycleRightText() noexcept {
    _rightTextIndex = (_rightTextIndex + 1) % 3;
    auto rightText = std::array<String, 3>{
        String{"plain", fg::BrightBlack},
        String{"status: ok", fg::BrightGreen},
        String{"wide right-side label", fg::BrightYellow}};
    _sections->setSectionOptions(0, ui::SectionOptions{String{"Overview"}, rightText[_rightTextIndex]});
    updateStateText();
}

void UiSectionsApp::updateStateText() {
    _stateText->setText(
        String{
            std::format(
                "Details section: {}\nTrailing separator: {}\nRight text variant: {}",
                _detailsVisible ? "visible" : "hidden",
                _trailingSeparator ? "visible" : "hidden",
                _rightTextIndex + 1),
            fg::BrightWhite});
}

}
