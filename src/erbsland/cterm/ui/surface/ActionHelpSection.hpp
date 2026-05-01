// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "HtmlHelpSection.hpp"

#include "../Action.hpp"
#include "../Page.hpp"

#include <vector>

namespace erbsland::cterm::ui::surface {

class ActionHelpSection;
using ActionHelpSectionPtr = std::shared_ptr<ActionHelpSection>;

/// A generated help section that lists available actions and shortcuts.
class ActionHelpSection final : public HtmlHelpSection {
public:
    /// Create an action help section.
    explicit ActionHelpSection(ProtectedTag) noexcept;
    ~ActionHelpSection() override = default;

public:
    /// Create an action help section.
    /// @return The new action help section.
    [[nodiscard]] static auto create() -> ActionHelpSectionPtr;

public:
    /// Set HTML text rendered before the shortcuts table.
    /// @param introHtml The intro HTML.
    void setIntro(std::string_view introHtml);
    /// Set HTML text rendered after the shortcuts table.
    /// @param outroHtml The outro HTML.
    void setOutro(std::string_view outroHtml);
    /// Set the surface whose action chain is scanned for shortcuts.
    /// @param surface The source surface.
    void setActionSource(const SurfacePtr &surface);
    /// Access the current action source.
    [[nodiscard]] auto actionSource() const noexcept -> SurfacePtr;

private:
    /// Initialize inherited HTML view state and generated content after construction.
    void initializeUi() override;

    struct ActionCandidate final {
        ActionPtr action;          ///< The action.
        int chainIndex{};          ///< The source-chain index.
        std::size_t actionIndex{}; ///< The action insertion index within the scan.
    };

private:
    /// Rebuild the generated HTML page.
    void rebuildHtml();
    /// Collect actions for the source surface chain.
    [[nodiscard]] auto collectActions() const -> std::vector<ActionPtr>;
    /// Build the source surface chain.
    [[nodiscard]] auto sourceSurfaceChain() const -> std::vector<SurfacePtr>;
    /// Add candidates for one surface.
    static void addCandidates(
        const SurfacePtr &surface,
        int chainIndex,
        std::vector<ActionCandidate> &candidates,
        std::vector<ActionPtr> &seen);
    /// Render all candidates into a preformatted shortcuts table.
    [[nodiscard]] static auto renderActionTable(const std::vector<ActionPtr> &actions) -> std::string;
    /// Render one action description according to its configured help format.
    [[nodiscard]] static auto renderDescription(const HelpData &helpData) -> std::string;
    /// Render one key collection.
    [[nodiscard]] static auto renderKeys(const std::vector<Key> &keys) -> std::string;

private:
    std::string _introHtml;       ///< HTML before the shortcut table.
    std::string _outroHtml;       ///< HTML after the shortcut table.
    SurfaceWeakPtr _actionSource; ///< The surface whose actions are displayed.
};

}
