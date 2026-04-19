// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "TerminalApplication.hpp"

#include <erbsland/cterm/text/HtmlRenderer.hpp>

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace demo::htmlviewer {

/// A small full-screen HTML viewer demo with scrolling and pagination.
class HtmlViewerApp final : public TerminalApplication {
public:
    // defaults
    HtmlViewerApp() = default;
    ~HtmlViewerApp() override = default;

public: // implement TerminalApplication
    void beforeInitialize() override;
    auto onCommandLine(const std::vector<std::string_view> &args) -> int override;
    auto beforeRun() -> int override;
    void onKey(const Key &key) override;
    void onRenderToBuffer() override;

private:
    /// Get the bundled default HTML file used when no file argument is given.
    [[nodiscard]] static auto defaultHtmlFilePath() -> std::filesystem::path;
    /// Load the contents of an HTML file.
    /// @param htmlFilePath The file to read.
    /// @return The file contents as a UTF-8 byte string.
    [[nodiscard]] static auto loadHtmlFile(const std::filesystem::path &htmlFilePath) -> std::string;

    /// Predefined document styles available in the viewer.
    using DocumentStylePreset = text::Style::Predefined;

private:
    auto printRenderedDocument() -> int;
    void renderDocumentIfRequired(int contentWidth);
    void drawHeader(Rectangle rect);
    void drawFooter(Rectangle rect);
    void drawDocument(Rectangle rect);
    void updateView(Rectangle contentRect) noexcept;
    void advanceDocumentStylePreset() noexcept;
    [[nodiscard]] auto locationText() const -> std::string;
    [[nodiscard]] auto displayName() const -> std::string;
    [[nodiscard]] auto documentStylePresetName() const noexcept -> std::string_view;
    [[nodiscard]] auto documentBaseColor() const -> Color;
    [[nodiscard]] auto documentStyle() const -> const text::StyleConstPtr &;
    [[nodiscard]] static auto documentStyleForPreset(DocumentStylePreset preset) -> const text::StyleConstPtr &;
    [[nodiscard]] static auto parseDocumentStylePreset(std::string_view value, DocumentStylePreset &preset) noexcept
        -> bool;
    [[nodiscard]] static auto contentRectForBuffer(Size bufferSize) noexcept -> Rectangle;
    [[nodiscard]] static auto clampViewOffset(int viewOffset, int viewHeight, int contentHeight) noexcept -> int;

private:
    std::filesystem::path _htmlFilePath;
    std::string _html;
    DocumentStylePreset _documentStylePreset{DocumentStylePreset::Styled};
    bool _printMode{false};
    std::shared_ptr<CursorBuffer> _documentBuffer = std::make_shared<CursorBuffer>(
        Size{1, 1},
        CursorBuffer::OverflowMode::ExpandThenShift,
        Size{1, 20'000},
        Char{U' ', Color{fg::White, bg::Black}});
    BufferView _documentView{_documentBuffer, Rectangle{0, 0, 1, 1}};
    int _viewOffsetY{0};
    int _renderedContentWidth{0};
    bool _documentDirty{true};
};

}
