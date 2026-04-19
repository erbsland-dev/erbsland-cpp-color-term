// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <erbsland/cterm/all.hpp>
#include <erbsland/cterm/text/Style.hpp>
#include <erbsland/cterm/ui/all.hpp>

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace demo::ui_html_viewer {

using namespace erbsland::cterm;

class HtmlDocumentPanel;

class UiHtmlViewerApp {
public:
    UiHtmlViewerApp() = default;
    ~UiHtmlViewerApp() = default;

public:
    auto run(int argc, char **argv) -> int;

private:
    using DocumentStylePreset = text::Style::Predefined;

private:
    auto onCommandLine(const std::vector<std::string_view> &args) -> int;
    void setupUi();
    void updateDynamicUi() noexcept;
    void advanceDocumentStylePreset() noexcept;
    [[nodiscard]] auto locationText() const -> std::string;
    [[nodiscard]] auto displayName() const -> std::string;
    [[nodiscard]] auto documentStylePresetName() const noexcept -> std::string_view;
    [[nodiscard]] auto documentStyle() const -> const text::StyleConstPtr &;
    [[nodiscard]] static auto documentStyleForPreset(DocumentStylePreset preset) -> const text::StyleConstPtr &;
    [[nodiscard]] static auto parseDocumentStylePreset(std::string_view value, DocumentStylePreset &preset) noexcept
        -> bool;
    [[nodiscard]] static auto defaultHtmlFilePath() -> std::filesystem::path;
    [[nodiscard]] static auto loadHtmlFile(const std::filesystem::path &htmlFilePath) -> std::string;

private:
    ui::Application _app;
    std::filesystem::path _htmlFilePath;
    std::string _html;
    DocumentStylePreset _documentStylePreset{DocumentStylePreset::Styled};
    std::shared_ptr<HtmlDocumentPanel> _documentPanel;
    ui::StatusLinePtr _headerStatus;
    ui::StatusLinePtr _footerStatus;
};

}
