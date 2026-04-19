// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "HtmlViewerApp.hpp"

#include "DemoError.hpp"

#include <algorithm>
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <stdexcept>

namespace demo::htmlviewer {

void HtmlViewerApp::beforeInitialize() {
    _documentBuffer = std::make_shared<CursorBuffer>(
        Size{80, 1}, CursorBuffer::OverflowMode::ExpandThenShift, Size{80, 20'000}, Char{U' '});
}

auto HtmlViewerApp::onCommandLine(const std::vector<std::string_view> &args) -> int {
    const auto printUsage = []() -> void {
        std::cerr << "Usage: html-viewer [--print|-p] [--style=<plain|simple|styled>|-s=<style>] [html-file]\n";
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
        if (argument == "--print" || argument == "-p") {
            _printMode = true;
            continue;
        }
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

auto HtmlViewerApp::beforeRun() -> int {
    _html = loadHtmlFile(_htmlFilePath);
    if (_printMode) {
        return printRenderedDocument();
    }
    if (!_terminal.isInteractive()) {
        _terminal.printLine(text::HtmlRenderer{_html, documentStyle()}.renderString());
        return -1;
    }
    return 0;
}

auto HtmlViewerApp::printRenderedDocument() -> int {
    renderDocumentIfRequired(std::max(_terminal.size().width(), 1));
    _terminal.write(*_documentBuffer);
    _terminal.flush();
    return -1;
}

void HtmlViewerApp::onKey(const Key &key) {
    const auto contentRect = contentRectForBuffer(_buffer.size());
    const auto pageStep = std::max(contentRect.height() - 1, 1);
    const auto contentHeight = _documentBuffer->size().height();
    if (key == U'q' || key == Key::Escape) {
        _quitRequested = true;
    } else if (key == Key::Up) {
        _viewOffsetY = clampViewOffset(_viewOffsetY - 1, contentRect.height(), contentHeight);
    } else if (key == Key::Down) {
        _viewOffsetY = clampViewOffset(_viewOffsetY + 1, contentRect.height(), contentHeight);
    } else if (key == Key::PageUp) {
        _viewOffsetY = clampViewOffset(_viewOffsetY - pageStep, contentRect.height(), contentHeight);
    } else if (key == Key::PageDown || key == Key::Space) {
        _viewOffsetY = clampViewOffset(_viewOffsetY + pageStep, contentRect.height(), contentHeight);
    } else if (key == Key::Home) {
        _viewOffsetY = 0;
    } else if (key == Key::End) {
        _viewOffsetY = clampViewOffset(contentHeight - contentRect.height(), contentRect.height(), contentHeight);
    } else if (key == U's' || key == U'S') {
        advanceDocumentStylePreset();
    }
}

void HtmlViewerApp::onRenderToBuffer() {
    _buffer.fill(Char{U' ', bg::Black});
    const auto headerRect = Rectangle{0, 0, _buffer.size().width(), 1};
    const auto contentRect = contentRectForBuffer(_buffer.size());
    const auto footerRect = Rectangle{0, _buffer.size().height() - 1, _buffer.size().width(), 1};
    renderDocumentIfRequired(contentRect.width());
    drawHeader(headerRect);
    drawDocument(contentRect);
    drawFooter(footerRect);
}

void HtmlViewerApp::renderDocumentIfRequired(const int contentWidth) {
    const auto normalizedWidth = std::max(contentWidth, 1);
    if (!_documentDirty && _renderedContentWidth == normalizedWidth) {
        return;
    }
    const auto baseColor = documentBaseColor();
    _documentBuffer->resize(Size{normalizedWidth, 1});
    _documentBuffer->setFillChar(Char(U' ', baseColor));
    _documentBuffer->fill(_documentBuffer->rect(), Char(U' ', baseColor));
    _documentBuffer->setColor(baseColor);
    _documentBuffer->clearScreen();
    _documentBuffer->moveHome();
    text::HtmlRenderer{_html, documentStyle()}.renderTo(_documentBuffer);
    _documentView.setContent(_documentBuffer);
    _renderedContentWidth = normalizedWidth;
    _documentDirty = false;
    _viewOffsetY =
        clampViewOffset(_viewOffsetY, contentRectForBuffer(_buffer.size()).height(), _documentBuffer->size().height());
}

void HtmlViewerApp::drawHeader(const Rectangle rect) {
    _buffer.fill(rect, Char{U' ', bg::Blue});
    _buffer.drawText(
        std::format("HTML Viewer  |  {}", displayName()),
        rect.insetBy(Margins{1, 0}),
        Alignment::CenterLeft,
        Color{fg::BrightWhite, bg::Blue});
    _buffer.drawText(
        std::format(
            "{}  |  {} x {}",
            documentStylePresetName(),
            _documentBuffer->size().width(),
            _documentBuffer->size().height()),
        rect.insetBy(Margins{1, 0}),
        Alignment::CenterRight,
        Color{fg::BrightCyan, bg::Blue});
}

void HtmlViewerApp::drawFooter(const Rectangle rect) {
    _buffer.fill(rect, Char{U' ', bg::BrightBlack});
    _buffer.drawText(
        locationText(), rect.insetBy(Margins{1, 0}), Alignment::CenterLeft, Color{fg::BrightGreen, bg::BrightBlack});
    auto help = String{};
    help.append(
        fg::BrightYellow,
        Key{Key::Up}.toDisplayText(),
        " ",
        Key{Key::Down}.toDisplayText(),
        fg::BrightWhite,
        " scroll  ",
        fg::BrightYellow,
        Key{Key::PageUp}.toDisplayText(),
        " ",
        Key{Key::PageDown}.toDisplayText(),
        fg::BrightWhite,
        " page  ",
        fg::BrightYellow,
        "[S]",
        fg::BrightWhite,
        " style  ",
        fg::BrightYellow,
        "[Q]",
        fg::BrightWhite,
        " quit");
    _buffer.drawText(Text{help, rect.insetBy(Margins{1, 0}), Alignment::CenterRight});
}

void HtmlViewerApp::drawDocument(const Rectangle rect) {
    _buffer.fill(rect, Char{U' ', documentBaseColor()});
    updateView(rect);
    _buffer.drawBuffer(_documentView, rect);
}

void HtmlViewerApp::updateView(const Rectangle contentRect) noexcept {
    _viewOffsetY = clampViewOffset(
        _viewOffsetY, contentRect.height(), _documentBuffer == nullptr ? 0 : _documentBuffer->size().height());
    _documentView.setViewRect(Rectangle{0, _viewOffsetY, contentRect.width(), contentRect.height()});
}

void HtmlViewerApp::advanceDocumentStylePreset() noexcept {
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
    _documentDirty = true;
}

auto HtmlViewerApp::locationText() const -> std::string {
    const auto totalLines = std::max(_documentBuffer->size().height(), 1);
    const auto topLine = std::min(_viewOffsetY + 1, totalLines);
    const auto bottomLine = std::min(_viewOffsetY + _documentView.viewRect().height(), totalLines);
    const auto percent = static_cast<int>((bottomLine * 100) / totalLines);
    return std::format("lines {}-{} / {}  ({}%)", topLine, bottomLine, totalLines, percent);
}

auto HtmlViewerApp::displayName() const -> std::string {
    const auto filename = _htmlFilePath.filename().string();
    return filename.empty() ? _htmlFilePath.string() : filename;
}

auto HtmlViewerApp::documentStylePresetName() const noexcept -> std::string_view {
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

auto HtmlViewerApp::documentBaseColor() const -> Color {
    return documentStyle()->baseTextStyle().color();
}

auto HtmlViewerApp::defaultHtmlFilePath() -> std::filesystem::path {
    return std::filesystem::path{ERBSLAND_COLOR_TERM_HTML_VIEWER_DEFAULT_FILE};
}

auto HtmlViewerApp::loadHtmlFile(const std::filesystem::path &htmlFilePath) -> std::string {
    auto file = std::ifstream{htmlFilePath, std::ios::binary};
    if (!file) {
        throw DemoError{std::format("Failed to open HTML file: {}", htmlFilePath.string())};
    }
    return std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

auto HtmlViewerApp::documentStyle() const -> const text::StyleConstPtr & {
    return documentStyleForPreset(_documentStylePreset);
}

auto HtmlViewerApp::documentStyleForPreset(const DocumentStylePreset preset) -> const text::StyleConstPtr & {
    return text::Style::defaultStyle(preset);
}

auto HtmlViewerApp::parseDocumentStylePreset(const std::string_view value, DocumentStylePreset &preset) noexcept
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

auto HtmlViewerApp::contentRectForBuffer(const Size bufferSize) noexcept -> Rectangle {
    return Rectangle{0, 1, bufferSize.width(), std::max(bufferSize.height() - 2, 1)};
}

auto HtmlViewerApp::clampViewOffset(const int viewOffset, const int viewHeight, const int contentHeight) noexcept
    -> int {
    return std::clamp(viewOffset, 0, std::max(contentHeight - std::max(viewHeight, 1), 0));
}

}
