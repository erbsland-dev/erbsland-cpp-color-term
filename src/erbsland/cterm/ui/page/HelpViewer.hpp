// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../layout/Pages.hpp"
#include "../layout/Stack.hpp"
#include "../Page.hpp"
#include "../surface/AbstractHelpSection.hpp"
#include "../surface/FooterLine.hpp"
#include "../surface/HeaderLine.hpp"

#include <vector>

namespace erbsland::cterm::ui::page {

class HelpViewer;
using HelpViewerPtr = std::shared_ptr<HelpViewer>;

/// A full-screen help viewer page with titled, scrollable help sections.
class HelpViewer final : public Page {
public:
    /// Create a help viewer page.
    explicit HelpViewer(ProtectedTag);
    ~HelpViewer() override = default;

public:
    /// Create a help viewer page.
    /// @return The new help viewer.
    [[nodiscard]] static auto create() -> HelpViewerPtr;

public:
    /// Add one help section.
    /// @param section The section to add.
    void addSection(surface::AbstractHelpSectionPtr section);
    /// Remove all help sections.
    void clearSections();
    /// Show this help viewer on the application display.
    void show();
    /// Close this help viewer.
    void close();
    /// Access the page-switching layout.
    [[nodiscard]] auto pages() const noexcept -> const layout::PagesPtr &;
    /// Access the current help section.
    [[nodiscard]] auto currentSection() const noexcept -> surface::AbstractHelpSectionPtr;
    /// Access the header line.
    [[nodiscard]] auto header() const noexcept -> const surface::HeaderLinePtr &;
    /// Access the footer line.
    [[nodiscard]] auto footer() const noexcept -> const surface::FooterLinePtr &;

public: // implement Surface
    void onLayout(LayoutScope &scope) noexcept override;

private:
    /// Create the static child tree and page actions.
    void initializeUi() override;
    /// Create actions for the help viewer page.
    void initializeActions();
    /// Move to the previous section.
    void previousSection();
    /// Move to the next section.
    void nextSection();
    /// Scroll the current section for a key.
    void scrollCurrentSection(const Key &key) noexcept;
    /// Refresh header and footer status text.
    void updateStatus() noexcept;
    /// Get the visible sections in configured order.
    [[nodiscard]] auto visibleSections() const -> std::vector<surface::AbstractHelpSectionPtr>;
    /// Get the display title for the current section.
    [[nodiscard]] auto currentTitle() const -> String;
    /// Get the display page counter for the footer.
    [[nodiscard]] auto pageCounterText() const -> String;

private:
    layout::StackPtr _root;                                 ///< Root vertical layout.
    surface::HeaderLinePtr _header;                         ///< Section title header.
    layout::PagesPtr _pages;                                ///< Help-section page layout.
    surface::FooterLinePtr _footer;                         ///< Page counter and key help footer.
    std::vector<surface::AbstractHelpSectionPtr> _sections; ///< Configured help sections.
};

}
