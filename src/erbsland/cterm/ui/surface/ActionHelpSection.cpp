// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ActionHelpSection.hpp"

#include "../../text/HtmlRenderer.hpp"

#include <algorithm>
#include <format>
#include <ranges>

namespace erbsland::cterm::ui::surface {

ActionHelpSection::ActionHelpSection(ProtectedTag protectedTag) noexcept : HtmlHelpSection{protectedTag} {
}

auto ActionHelpSection::create() -> ActionHelpSectionPtr {
    auto result = std::make_shared<ActionHelpSection>(ProtectedTag{});
    result->initializeUi();
    return result;
}

void ActionHelpSection::initializeUi() {
    HtmlHelpSection::initializeUi();
    setTitle("Keyboard Shortcuts");
    rebuildHtml();
}

void ActionHelpSection::setIntro(const std::string_view introHtml) {
    _introHtml = introHtml;
    rebuildHtml();
}

void ActionHelpSection::setOutro(const std::string_view outroHtml) {
    _outroHtml = outroHtml;
    rebuildHtml();
}

void ActionHelpSection::setActionSource(const SurfacePtr &surface) {
    _actionSource = surface;
    rebuildHtml();
}

auto ActionHelpSection::actionSource() const noexcept -> SurfacePtr {
    return _actionSource.lock();
}

void ActionHelpSection::rebuildHtml() {
    auto html = std::string{};
    html += _introHtml;
    html += renderActionTable(collectActions());
    html += _outroHtml;
    setHtml(html);
}

auto ActionHelpSection::collectActions() const -> std::vector<ActionPtr> {
    auto candidates = std::vector<ActionCandidate>{};
    auto seen = std::vector<ActionPtr>{};
    const auto chain = sourceSurfaceChain();
    for (auto index = std::size_t{0}; index < chain.size(); ++index) {
        addCandidates(chain[index], static_cast<int>(index), candidates, seen);
    }
    std::ranges::stable_sort(candidates, [](const ActionCandidate &left, const ActionCandidate &right) -> bool {
        if (left.action->help().priority() != right.action->help().priority()) {
            return left.action->help().priority() > right.action->help().priority();
        }
        if (left.chainIndex != right.chainIndex) {
            return left.chainIndex < right.chainIndex;
        }
        return left.actionIndex < right.actionIndex;
    });
    auto result = std::vector<ActionPtr>{};
    result.reserve(candidates.size());
    for (const auto &candidate : candidates) {
        result.emplace_back(candidate.action);
    }
    return result;
}

auto ActionHelpSection::sourceSurfaceChain() const -> std::vector<SurfacePtr> {
    auto result = std::vector<SurfacePtr>{};
    for (auto surface = _actionSource.lock(); surface != nullptr; surface = surface->parent().lock()) {
        result.emplace_back(surface);
        if (surface->isPage()) {
            break;
        }
    }
    return result;
}

void ActionHelpSection::addCandidates(
    const SurfacePtr &surface,
    const int chainIndex,
    std::vector<ActionCandidate> &candidates,
    std::vector<ActionPtr> &seen) {
    if (surface == nullptr || surface->actionsPtr() == nullptr) {
        return;
    }
    auto actionIndex = candidates.size();
    for (const auto &action : *surface->actionsPtr()) {
        if (action == nullptr || std::ranges::find(seen, action) != seen.end()) {
            continue;
        }
        action->refreshEnabled(Action::EnabledRefreshReason::BeforeRepaint);
        action->refreshEnabled(Action::EnabledRefreshReason::Polled100ms);
        if (!action->isEnabled() || !action->help().isVisibleOnHelpPage() || action->help().name().empty() ||
            action->keys().empty()) {
            continue;
        }
        seen.emplace_back(action);
        candidates.push_back(
            ActionCandidate{
                .action = action,
                .chainIndex = chainIndex,
                .actionIndex = actionIndex++,
            });
    }
}

auto ActionHelpSection::renderActionTable(const std::vector<ActionPtr> &actions) -> std::string {
    if (actions.empty()) {
        return "<p>No keyboard shortcuts are available for the current context.</p>";
    }
    auto html = std::string{"<h2>Keyboard Shortcuts</h2><dl>"};
    for (const auto &action : actions) {
        html += std::format(
            "<dt>{} <strong>{}</strong></dt><dd>{}</dd>",
            renderKeys(action->keys().keys()),
            text::HtmlRenderer::escapeHtml(action->help().name()),
            renderDescription(action->help()));
    }
    html += "</dl>";
    return html;
}

auto ActionHelpSection::renderDescription(const HelpData &helpData) -> std::string {
    switch (helpData.descriptionFormat()) {
    case HelpFormat::Html:
        return helpData.description();
    case HelpFormat::Text:
        return text::HtmlRenderer::escapeHtml(helpData.description());
    }
    return text::HtmlRenderer::escapeHtml(helpData.description());
}

auto ActionHelpSection::renderKeys(const std::vector<Key> &keys) -> std::string {
    auto result = std::string{};
    for (auto index = std::size_t{0}; index < keys.size(); ++index) {
        if (index > 0) {
            result += "/";
        }
        result += "<span class=\"key\">";
        result += text::HtmlRenderer::escapeHtml(keys[index].toDisplayText(false));
        result += "</span>";
    }
    return result;
}

}
