// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "HelpViewer.hpp"

#include "../Application.hpp"

#include <algorithm>
#include <format>
#include <stdexcept>

namespace erbsland::cterm::ui::page {

HelpViewer::HelpViewer(ProtectedTag protectedTag) : Page{protectedTag} {
}

auto HelpViewer::create() -> HelpViewerPtr {
    auto result = std::make_shared<HelpViewer>(ProtectedTag{});
    result->initializeUi();
    return result;
}

void HelpViewer::addSection(surface::AbstractHelpSectionPtr section) {
    if (section == nullptr) {
        throw std::invalid_argument{"Help section must not be null."};
    }
    _sections.emplace_back(section);
    _pages->addSurface(std::move(section));
    updateStatus();
}

void HelpViewer::clearSections() {
    _sections.clear();
    _pages->surfaces().removeAll();
    updateStatus();
}

void HelpViewer::show() {
    if (_pages->pageCount() > 0) {
        _pages->showPage(0);
    }
    updateStatus();
    getApplication().display().pushPage(std::dynamic_pointer_cast<Page>(shared_from_this()));
}

void HelpViewer::close() {
    getApplication().display().popPage();
}

auto HelpViewer::pages() const noexcept -> const layout::PagesPtr & {
    return _pages;
}

auto HelpViewer::currentSection() const noexcept -> surface::AbstractHelpSectionPtr {
    const auto sections = visibleSections();
    const auto index = _pages->currentPage();
    if (index < 0 || index >= static_cast<int>(sections.size())) {
        return {};
    }
    return sections[static_cast<std::size_t>(index)];
}

auto HelpViewer::header() const noexcept -> const surface::HeaderLinePtr & {
    return _header;
}

auto HelpViewer::footer() const noexcept -> const surface::FooterLinePtr & {
    return _footer;
}

void HelpViewer::onLayout(LayoutScope &scope) noexcept {
    updateStatus();
    Page::onLayout(scope);
}

void HelpViewer::initializeUi() {
    Page::initializeUi();
    _root = layout::Stack::create(Orientation::Vertical);
    _header = surface::HeaderLine::create();
    _pages = layout::Pages::create();
    _footer = surface::FooterLine::create();
    addSurface(_root);
    using S = DynamicTextLine::Section;
    _header->setMargins(S::Left, Margins{1, 0});
    _header->setMargins(S::Right, Margins{1, 0});
    _header->themeAttributes().setElement(theme::Element::HelpViewer);
    _root->addSurface(_header);
    _root->addSurface(_pages);
    _footer->setText(String{});
    _root->addSurface(_footer);
    updateStatus();
    initializeActions();
}

void HelpViewer::initializeActions() {
    auto closeAction = Action::create("Close");
    closeAction->setKeys(Keys{Key::F1, Key::Escape, U'?', U'h'}.setMainKeyCount(2));
    closeAction->setTriggerFn([this]() -> void { close(); });
    closeAction->help().setPriority(100);
    actions().add(closeAction);

    auto previousAction = Action::create("Previous");
    previousAction->setKeys(Keys{Key::Left, U'p'}.setMainKeyCount(1));
    previousAction->setTriggerFn([this]() -> void { previousSection(); });
    previousAction->help().setPriority(80);
    actions().add(previousAction);

    auto nextAction = Action::create("Next");
    nextAction->setKeys(Keys{Key::Right, U'n'}.setMainKeyCount(1));
    nextAction->setTriggerFn([this]() -> void { nextSection(); });
    nextAction->help().setPriority(80);
    actions().add(nextAction);

    auto scrollAction = Action::create("Scroll");
    scrollAction->setKeys(
        Keys{Key::Up, Key::Down, Key::PageUp, Key::PageDown, Key::Space, Key::Enter}.setMainKeyCount(4));
    scrollAction->setTriggerFn(
        [this](const ActionTriggerContext &context) -> void { scrollCurrentSection(context.key()); });
    scrollAction->help().setPriority(60);
    actions().add(scrollAction);

    auto topBottomAction = Action::create("Top / Bottom");
    topBottomAction->setKeys({Key::Home, Key::End});
    topBottomAction->setTriggerFn([this](const ActionTriggerContext &context) -> void {
        if (context.key() == Key::Home) {
            if (const auto section = currentSection(); section != nullptr) {
                section->scrollToTop();
            }
        } else if (context.key() == Key::End) {
            if (const auto section = currentSection(); section != nullptr) {
                section->scrollToBottom();
            }
        }
    });
    topBottomAction->help().setPriority(40);
    actions().add(topBottomAction);
}

void HelpViewer::previousSection() {
    _pages->previousPage();
    if (const auto section = currentSection(); section != nullptr) {
        section->scrollToTop();
    }
    updateStatus();
}

void HelpViewer::nextSection() {
    _pages->nextPage();
    if (const auto section = currentSection(); section != nullptr) {
        section->scrollToTop();
    }
    updateStatus();
}

void HelpViewer::scrollCurrentSection(const Key &key) noexcept {
    const auto section = currentSection();
    if (section == nullptr) {
        return;
    }
    switch (key.type()) {
    case Key::Up:
        section->scrollUp();
        break;
    case Key::Down:
    case Key::Enter:
        section->scrollDown();
        break;
    case Key::PageUp:
        section->pageUp();
        break;
    case Key::PageDown:
    case Key::Space:
        section->pageDown();
        break;
    default:
        break;
    }
}

void HelpViewer::updateStatus() noexcept {
    using S = DynamicTextLine::Section;
    _header->setText(S::Left, currentTitle());
    _header->setText(S::Right, String{"Help", fg::BrightYellow});
    _footer->setText(pageCounterText());
}

auto HelpViewer::visibleSections() const -> std::vector<surface::AbstractHelpSectionPtr> {
    auto result = std::vector<surface::AbstractHelpSectionPtr>{};
    for (const auto &section : _sections) {
        if (section != nullptr && section->flags().isVisible()) {
            result.emplace_back(section);
        }
    }
    return result;
}

auto HelpViewer::currentTitle() const -> String {
    const auto section = currentSection();
    if (section == nullptr) {
        return String{"Help", fg::BrightWhite};
    }
    return String{std::string{section->title()}, fg::BrightWhite};
}

auto HelpViewer::pageCounterText() const -> String {
    const auto count = _pages->pageCount();
    const auto index = _pages->currentPage();
    if (count <= 0 || index < 0) {
        return String{"0 / 0", fg::BrightWhite};
    }
    return String{std::format("{} / {}", index + 1, count), fg::BrightWhite};
}

}
