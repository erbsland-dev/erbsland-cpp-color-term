// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "UiDemoApplication.hpp"

#include <utility>

namespace demo {

UiDemoApplication::UiDemoApplication() {
    initializeHelp();
}

UiDemoApplication::UiDemoApplication(const int argc, char *argv[]) : Application{argc, argv} {
    initializeHelp();
}

UiDemoApplication::UiDemoApplication(TerminalPtr terminal) : Application{std::move(terminal)} {
    initializeHelp();
}

UiDemoApplication::UiDemoApplication(const int argc, char *argv[], TerminalPtr terminal) :
    Application{argc, argv, std::move(terminal)} {
    initializeHelp();
}

void UiDemoApplication::addDefaultActions(const ui::PagePtr &page) noexcept {
    page->actions().add(_helpAction);
    page->actions().add(_quitAction);
    page->actions().add(_cycleThemeAction);
}

auto UiDemoApplication::helpViewer() const noexcept -> const ui::HelpViewerPtr & {
    return _helpViewer;
}

auto UiDemoApplication::actionHelpSection() const noexcept -> const ui::ActionHelpSectionPtr & {
    return _actionHelpSection;
}

auto UiDemoApplication::helpAction() const noexcept -> const ui::ActionPtr & {
    return _helpAction;
}

auto UiDemoApplication::quitAction() const noexcept -> const ui::ActionPtr & {
    return _quitAction;
}

auto UiDemoApplication::cycleThemeAction() const noexcept -> const ui::ActionPtr & {
    return _cycleThemeAction;
}

void UiDemoApplication::initializeHelp() {
    _helpViewer = ui::HelpViewer::create();
    _actionHelpSection = ui::ActionHelpSection::create();
    _actionHelpSection->setIntro(
        "<p>This page lists the keyboard shortcuts available in the current context. "
        "The displayed keyboard shortcuts are specific to the currently focus user interface element. "
        "To get help for a specific user interface element, press F1 or '?' while the element is focused.</p>");
    _helpViewer->addSection(_actionHelpSection);

    _helpAction = ui::Action::create("Help");
    _helpAction->setKeys(Keys{Key::F1, U'?'}.setMainKeyCount(1));
    _helpAction->setTriggerFn([this](const ui::ActionTriggerContext &context) -> void { openHelp(context); });
    _helpAction->help().setDescription("Open the help viewer.");
    _helpAction->help().setPriority(1'000);

    _quitAction = ui::Action::create("Quit");
    _quitAction->setKeys(Keys{U'q', Key::Escape}.setMainKeyCount(1));
    _quitAction->setTriggerFn([]() -> void { ui::getApplication().quit(); });
    _quitAction->help().setPriority(-100);
    _quitAction->help().setDescription("Quit the application.");

    _cycleThemeAction = ui::Action::create("Cycle Theme");
    _cycleThemeAction->help().setVisibility(ui::HelpVisibility::HelpPage);
    _cycleThemeAction->setKeys(Keys{Key::F2, U't'}.setMainKeyCount(1));
    _cycleThemeAction->setTriggerFn([this]() -> void { cycleTheme(); });
    _cycleThemeAction->help().setPriority(-90);
    _cycleThemeAction->help().setDescription("Cycle through available themes.");
}

void UiDemoApplication::openHelp(const ui::ActionTriggerContext &context) {
    _actionHelpSection->setActionSource(actionSourceFor(context.sourceSurface().lock()));
    _helpViewer->show();
}

auto UiDemoApplication::actionSourceFor(const ui::SurfacePtr &triggerSource) noexcept -> ui::SurfacePtr {
    if (const auto page = std::dynamic_pointer_cast<ui::Page>(triggerSource); page != nullptr) {
        if (const auto focusSurface = page->focusSurface().lock(); focusSurface != nullptr) {
            return focusSurface;
        }
    }
    return triggerSource;
}

void UiDemoApplication::cycleTheme() {
    _currentTheme =
        static_cast<DefaultTheme>((static_cast<int>(_currentTheme) + 1) % static_cast<int>(DefaultTheme::_count));
    switch (_currentTheme) {
    case DefaultTheme::Dark:
        setTheme(theme::Theme::dark());
        break;
    case DefaultTheme::Light:
        setTheme(theme::Theme::light());
        break;
    case DefaultTheme::Monochrome:
        setTheme(theme::Theme::monochrome());
        break;
    default:
        break;
    }
}

}
