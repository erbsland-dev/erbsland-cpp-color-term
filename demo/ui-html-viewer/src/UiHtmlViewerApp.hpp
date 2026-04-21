// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "UiDemoApplication.hpp"

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

class UiHtmlViewerApp : public demo::UiDemoApplication {
public:
    UiHtmlViewerApp(const int argc, char *argv[]) : UiDemoApplication(argc, argv) {}
    ~UiHtmlViewerApp() override = default;

protected: // implement ui::Application
    void setupUi() override;
    auto processCommandLineArguments(const CommandLineArgs &args) -> ExitCode override;

private:
    using DocumentStylePreset = text::Style::Predefined;

private:
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
    void parseStyleValue(std::string_view styleText);
    void printUsage() const;

private:
    std::filesystem::path _htmlFilePath;
    std::string _html;
    DocumentStylePreset _documentStylePreset{DocumentStylePreset::Styled};
    std::shared_ptr<HtmlDocumentPanel> _documentPanel;
    ui::HeaderLinePtr _headerStatus;
    ui::FooterLinePtr _footerStatus;
};

}
