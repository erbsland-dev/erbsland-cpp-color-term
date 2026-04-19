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

auto UiHtmlViewerApp::run(const int argc, char **argv) -> int {
    auto args = std::vector<std::string_view>{};
    args.reserve(static_cast<std::size_t>(argc));
    for (auto index = 0; index < argc; ++index) {
        args.emplace_back(argv[index]);
    }
    if (const auto exitCode = onCommandLine(args); exitCode != 0) {
        return exitCode;
    }
    _html = loadHtmlFile(_htmlFilePath);
    setupUi();
    return _app.run();
}

auto UiHtmlViewerApp::onCommandLine(const std::vector<std::string_view> &args) -> int {
    const auto printUsage = []() -> void {
        std::cerr << "Usage: ui-html-viewer [--style=<plain|simple|styled>|-s=<style>] [html-file]\n";
    };
    const auto parseStyleValue = [printUsage, this](const std::string_view styleText) -> bool {
        if (parseDocumentStylePreset(styleText, _documentStylePreset)) {
            return true;
        }
        std::cerr << std::format("Unknown style: {}\n", styleText);
        printUsage();
        return false;
    };
    auto htmlFilePath = std::filesystem::path{};
    for (auto index = std::size_t{1}; index < args.size(); ++index) {
        const auto argument = args[index];
        if (argument == "--style" || argument == "-s") {
            if (index + 1 >= args.size()) {
                std::cerr << std::format("Missing value for option: {}\n", argument);
                printUsage();
                return 1;
            }
            if (!parseStyleValue(args[++index])) {
                return 1;
            }
            continue;
        }
        if (argument.starts_with("--style=")) {
            if (!parseStyleValue(argument.substr(8))) {
                return 1;
            }
            continue;
        }
        if (argument.starts_with("-s=")) {
            if (!parseStyleValue(argument.substr(3))) {
                return 1;
            }
            continue;
        }
        if (argument.starts_with('-')) {
            std::cerr << std::format("Unknown option: {}\n", argument);
            printUsage();
            return 1;
        }
        if (!htmlFilePath.empty()) {
            std::cerr << "Only one HTML file argument is supported.\n";
            printUsage();
            return 1;
        }
        htmlFilePath = std::filesystem::path{std::string{argument}};
    }
    _htmlFilePath = htmlFilePath.empty() ? defaultHtmlFilePath() : htmlFilePath;
    return 0;
}

void UiHtmlViewerApp::setupUi() {
    auto page = ui::Page::create();
    page->setBackground(Char{U' ', bg::Black});
    page->keyBindings().bind({U'q', U'Q'}, []() -> void { ui::getApplication().quit(); });
    page->keyBindings().bind(Key::Escape, []() -> void { ui::getApplication().quit(); });
    page->keyBindings().bind({Key{Key::Up}, Key{Key::Character, U'k'}}, [this]() -> void {
        _documentPanel->view()->scrollUp();
        updateDynamicUi();
    });
    page->keyBindings().bind({Key{Key::Down}, Key{Key::Character, U'j'}, Key{Key::Enter}}, [this]() -> void {
        _documentPanel->view()->scrollDown();
        updateDynamicUi();
    });
    page->keyBindings().bind(Key::PageUp, [this]() -> void {
        _documentPanel->view()->pageUp();
        updateDynamicUi();
    });
    page->keyBindings().bind({Key::PageDown, Key::Space}, [this]() -> void {
        _documentPanel->view()->pageDown();
        updateDynamicUi();
    });
    page->keyBindings().bind(Key::Home, [this]() -> void {
        _documentPanel->view()->scrollToTop();
        updateDynamicUi();
    });
    page->keyBindings().bind(Key::End, [this]() -> void {
        _documentPanel->view()->scrollToBottom();
        updateDynamicUi();
    });
    page->keyBindings().bind({U's', U'S'}, [this]() -> void {
        advanceDocumentStylePreset();
        updateDynamicUi();
    });

    auto root = ui::Stack::create(Orientation::Vertical);
    page->addChild(root);

    auto header = ui::StatusLine::create();
    header->setBackground(Char{U' ', bg::Blue});
    header->setText(
        ui::StatusLine::Section::Left,
        String{std::format("UI HTML Viewer  |  {}", displayName()), Color{fg::BrightWhite, bg::Blue}});
    header->setMargins(ui::StatusLine::Section::Left, Margins{0, 1, 0, 1});
    header->setCollapseBehavior(ui::StatusLine::Section::Middle, ui::StatusLine::CollapseBehavior::Hide);
    header->setUpdateMode(ui::StatusLine::Section::Middle, ui::StatusLine::UpdateMode::OnRefresh);
    header->setUpdateFn(ui::StatusLine::Section::Middle, [this](String &text, const Coordinate) -> void {
        text = String{std::format("style {}", documentStylePresetName()), Color{fg::BrightYellow, bg::Blue}};
    });
    header->setMargins(ui::StatusLine::Section::Middle, Margins{0, 1, 0, 1});
    header->setUpdateMode(ui::StatusLine::Section::Right, ui::StatusLine::UpdateMode::OnRefresh);
    header->setUpdateFn(ui::StatusLine::Section::Right, [this](String &text, const Coordinate) -> void {
        if (_documentPanel == nullptr) {
            text.clear();
            return;
        }
        text = String{
            std::format("{} x {}", _documentPanel->contentSize().width(), _documentPanel->contentSize().height()),
            Color{fg::BrightCyan, bg::Blue}};
    });
    header->setMargins(ui::StatusLine::Section::Right, Margins{0, 1, 0, 1});
    root->addChild(header);
    _headerStatus = header;

    _documentPanel = HtmlDocumentPanel::create();
    _documentPanel->setHtml(_html);
    _documentPanel->setStyle(documentStyle());
    root->addChild(_documentPanel);

    auto footer = ui::StatusLine::create();
    footer->setBackground(Char{U' ', bg::BrightBlack});
    footer->setUpdateMode(ui::StatusLine::Section::Left, ui::StatusLine::UpdateMode::OnRefresh);
    footer->setUpdateFn(ui::StatusLine::Section::Left, [this](String &text, const Coordinate) -> void {
        if (_documentPanel == nullptr) {
            text.clear();
            return;
        }
        text = String{locationText(), Color{fg::BrightGreen, bg::BrightBlack}};
    });
    footer->setMargins(ui::StatusLine::Section::Left, Margins{0, 1, 0, 1});
    footer->setCollapseBehavior(ui::StatusLine::Section::Middle, ui::StatusLine::CollapseBehavior::Hide);
    footer->setText(
        ui::StatusLine::Section::Right,
        String{
            "Arrows/Enter scroll  PgUp/PgDn page  Home/End jump  [S] style  [Q] quit",
            Color{fg::BrightYellow, bg::BrightBlack}});
    footer->setMargins(ui::StatusLine::Section::Right, Margins{0, 1, 0, 1});
    root->addChild(footer);
    _footerStatus = footer;

    _app.terminal().setSafeMarginEnabled(false);
    _app.setMainPage(page);
}

void UiHtmlViewerApp::updateDynamicUi() noexcept {
    if (_headerStatus != nullptr) {
        _headerStatus->setPaintOutdated();
    }
    if (_footerStatus != nullptr) {
        _footerStatus->setPaintOutdated();
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
    const auto viewRect = _documentPanel->view()->rectangle();
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
        return "plain";
    case DocumentStylePreset::Simple:
        return "simple";
    case DocumentStylePreset::Styled:
        return "styled";
    }
    return "styled";
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

}
