// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "UiHtmlViewerApp.hpp"

#include "HtmlDocumentPanel.hpp"

#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

namespace demo::ui_html_viewer {

void UiHtmlViewerApp::setupUi() {
    auto page = ui::Page::create();
    addDefaultActions(page);

    auto scrollKeys = Keys{Key::Up, Key::Down, Key::PageUp, Key::PageDown, Key::Space, Key::Enter};
    scrollKeys.setMainKeyCount(4);
    auto scrollAction = ui::Action::create("Scroll");
    scrollAction->help().setDescription("Scroll the document by line or by page.");
    scrollAction->setKeys(scrollKeys);
    scrollAction->setTriggerFn([this](const ui::ActionTriggerContext &context) -> void {
        switch (context.key().type()) {
        case Key::Up:
            _documentPanel->view()->scrollUp();
            break;
        case Key::Down:
        case Key::Enter:
            _documentPanel->view()->scrollDown();
            break;
        case Key::PageUp:
            _documentPanel->view()->pageUp();
            break;
        case Key::PageDown:
        case Key::Space:
            _documentPanel->view()->pageDown();
            break;
        default:
            return;
        }
        updateDynamicUi();
    });
    page->actions().add(scrollAction);

    auto topBottomAction = ui::Action::create("Top / Bottom");
    topBottomAction->help().setDescription("Scroll to the top or bottom of the document.");
    topBottomAction->setKeys({Key::Home, Key::End});
    topBottomAction->setTriggerFn([this](const ui::ActionTriggerContext &context) -> void {
        if (context.key() == Key::Home) {
            _documentPanel->view()->scrollToTop();
        } else if (context.key() == Key::End) {
            _documentPanel->view()->scrollToBottom();
        }
        updateDynamicUi();
    });
    page->actions().add(topBottomAction);

    auto styleAction = ui::Action::create("Cycle Style");
    styleAction->help().setDescription("Cycle through available document styles.");
    styleAction->setKeys(Keys{U's'});
    styleAction->setTriggerFn([this]() -> void {
        advanceDocumentStylePreset();
        updateDynamicUi();
    });
    styleAction->help().setPriority(50);
    page->actions().add(styleAction);

    auto root = ui::Stack::create(Orientation::Vertical);
    page->addSurface(root);

    using Section = ui::DynamicTextLine::Section;
    using SpacePriority = ui::DynamicTextLine::SpacePriority;

    auto header = ui::HeaderLine::create();
    header->setText(Section::Left, String{std::format("UI HTML Viewer  |  {}", displayName()), fg::BrightWhite});
    header->setMargins(Section::Left, Margins{1, 0});
    header->setSpacePriority(Section::Middle, SpacePriority::Hide);
    header->dynamicText(Section::Middle)->setUpdateFn([this](String &text, const Coordinate) -> void {
        text = String{std::format("Style {}", documentStylePresetName()), fg::BrightYellow};
    });
    header->dynamicText(Section::Middle)->updateText();
    header->setMargins(Section::Middle, Margins{1, 0});
    header->dynamicText(Section::Right)->setUpdateFn([this](String &text, const Coordinate) -> void {
        if (_documentPanel == nullptr) {
            text.clear();
            return;
        }
        text = String{
            std::format("{} x {}", _documentPanel->contentSize().width(), _documentPanel->contentSize().height()),
            fg::BrightCyan};
    });
    header->dynamicText(Section::Right)->updateText();
    header->setMargins(Section::Right, Margins{1, 0});
    root->addSurface(header);
    _headerStatus = header;

    _documentPanel = HtmlDocumentPanel::create();
    root->addSurface(_documentPanel);

    auto footer = ui::FooterLine::create();
    footer->leftText()->setUpdateFn([this](String &text, const Coordinate) -> void {
        if (_documentPanel == nullptr) {
            text.clear();
            return;
        }
        text = String{locationText(), fg::BrightGreen};
    });
    root->addSurface(footer);
    _footerStatus = footer;
    setMainPage(page);
}

auto UiHtmlViewerApp::processCommandLineArguments(const std::vector<std::string> &args) -> ExitCode {
    try {
        auto htmlFilePath = std::filesystem::path{};
        for (auto index = std::size_t{1}; index < args.size(); ++index) {
            const auto &argument = args[index];
            if (argument == "--style" || argument == "-s") {
                if (index + 1 >= args.size()) {
                    throw std::runtime_error(std::format("Missing value for option: {}", argument));
                }
                parseStyleValue(args[++index]);
                continue;
            }
            if (argument.starts_with("--style=")) {
                parseStyleValue(argument.substr(8));
                continue;
            }
            if (argument.starts_with("-s=")) {
                parseStyleValue(argument.substr(3));
                continue;
            }
            if (argument.starts_with('-')) {
                throw std::runtime_error(std::format("Unknown option: {}", argument));
            }
            if (!htmlFilePath.empty()) {
                throw std::runtime_error("Only one HTML file argument is supported.");
            }
            htmlFilePath = std::filesystem::path{std::string{argument}};
        }
        _htmlFilePath = htmlFilePath.empty() ? defaultHtmlFilePath() : htmlFilePath;
        _html = loadHtmlFile(_htmlFilePath);
        _documentPanel->setHtml(_html);
        _documentPanel->setStyle(documentStyle());
        updateDynamicUi();
    } catch (const std::exception &e) {
        terminal().printLine(fg::BrightRed, e.what());
        printUsage();
        return 1;
    }
    return cExitCodeContinue;
}

void UiHtmlViewerApp::updateDynamicUi() noexcept {
    if (_headerStatus != nullptr) {
        _headerStatus->dynamicText(ui::DynamicTextLine::Section::Middle)->updateText();
        _headerStatus->dynamicText(ui::DynamicTextLine::Section::Right)->updateText();
        _headerStatus->flags().setLayoutOutdated();
        _headerStatus->flags().setPaintOutdated();
    }
    if (_footerStatus != nullptr) {
        _footerStatus->leftText()->updateText();
        _footerStatus->flags().setPaintOutdated();
    }
}

void UiHtmlViewerApp::advanceDocumentStylePreset() noexcept {
    switch (_documentStylePreset) {
    case DocumentStylePreset::Plain:
        _documentStylePreset = DocumentStylePreset::Simple;
        break;
    case DocumentStylePreset::Simple:
        _documentStylePreset = DocumentStylePreset::Styled;
        break;
    case DocumentStylePreset::Styled:
        _documentStylePreset = DocumentStylePreset::Plain;
        break;
    }
    if (_documentPanel != nullptr) {
        _documentPanel->setStyle(documentStyle());
    }
}

auto UiHtmlViewerApp::locationText() const -> std::string {
    const auto totalLines = std::max(_documentPanel->contentSize().height(), 1);
    const auto viewRect = _documentPanel->view()->visibleContentRect();
    const auto topLine = std::min(viewRect.y1() + 1, totalLines);
    const auto bottomLine = std::min(viewRect.y1() + std::max(viewRect.height(), 1), totalLines);
    const auto percent = static_cast<int>((bottomLine * 100) / totalLines);
    return std::format("lines {}-{} / {}  ({}%)", topLine, bottomLine, totalLines, percent);
}

auto UiHtmlViewerApp::displayName() const -> std::string {
    const auto filename = _htmlFilePath.filename().string();
    return filename.empty() ? _htmlFilePath.string() : filename;
}

auto UiHtmlViewerApp::documentStylePresetName() const noexcept -> std::string_view {
    switch (_documentStylePreset) {
    case DocumentStylePreset::Plain:
        return "Plain";
    case DocumentStylePreset::Simple:
        return "Simple";
    case DocumentStylePreset::Styled:
        return "Styled";
    }
    return "Styled";
}

auto UiHtmlViewerApp::documentStyle() const -> const text::StyleConstPtr & {
    return documentStyleForPreset(_documentStylePreset);
}

auto UiHtmlViewerApp::documentStyleForPreset(const DocumentStylePreset preset) -> const text::StyleConstPtr & {
    return text::Style::defaultStyle(preset);
}

auto UiHtmlViewerApp::parseDocumentStylePreset(const std::string_view value, DocumentStylePreset &preset) noexcept
    -> bool {
    if (value == "plain" || value == "default") {
        preset = DocumentStylePreset::Plain;
        return true;
    }
    if (value == "simple" || value == "compact") {
        preset = DocumentStylePreset::Simple;
        return true;
    }
    if (value == "styled" || value == "extended") {
        preset = DocumentStylePreset::Styled;
        return true;
    }
    return false;
}

auto UiHtmlViewerApp::defaultHtmlFilePath() -> std::filesystem::path {
    return std::filesystem::path{ERBSLAND_COLOR_TERM_UI_HTML_VIEWER_DEFAULT_FILE};
}

auto UiHtmlViewerApp::loadHtmlFile(const std::filesystem::path &htmlFilePath) -> std::string {
    auto file = std::ifstream{htmlFilePath, std::ios::binary};
    if (!file) {
        throw std::runtime_error{std::format("Failed to open HTML file: {}", htmlFilePath.string())};
    }
    return std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

void UiHtmlViewerApp::parseStyleValue(const std::string_view styleText) {
    if (parseDocumentStylePreset(styleText, _documentStylePreset)) {
        return;
    }
    throw std::runtime_error{std::format("Unknown style: {}", styleText)};
}

void UiHtmlViewerApp::printUsage() const {
    terminal().printLine(fg::BrightGreen, "Usage:");
    terminal().printLine(fg::BrightWhite, "ui-html-viewer [--style=<plain|simple|styled>|-s=<style>] [html-file]");
}

}
