// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ActionHelp.hpp"

#include "../Page.hpp"

#include "../../theme/LayoutHelper.hpp"

#include <algorithm>
#include <chrono>
#include <ranges>

namespace erbsland::cterm::ui::surface {

using namespace std::chrono_literals;

ActionHelp::ActionHelp([[maybe_unused]] ProtectedTag protectedTag) noexcept {
    editLayoutMetrics().setFixedHeight(1);
}

auto ActionHelp::create() -> ActionHelpPtr {
    auto result = std::make_shared<ActionHelp>(ProtectedTag{});
    result->initializeUi();
    return result;
}

void ActionHelp::initializeUi() {
    Surface::initializeUi();
    themeAttributes().setElement(theme::Element::ActionHelp);
    const auto weakThis = std::weak_ptr<ActionHelp>{std::static_pointer_cast<ActionHelp>(shared_from_this())};
    scheduler().addRepeated(
        [weakThis]() -> void {
            if (const auto surface = weakThis.lock(); surface != nullptr) {
                surface->flags().setPaintOutdated();
            }
        },
        100ms);
}

auto ActionHelp::collectActions() -> std::vector<ActionPtr> {
    const auto page = nearestPage();
    if (page == nullptr) {
        return {};
    }
    auto candidates = std::vector<ActionCandidate>{};
    auto seen = std::vector<ActionPtr>{};
    auto chain = focusedSurfaceChain(page);
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

auto ActionHelp::renderHelpText(const PaintContext &context, const Coordinate width) -> String {
    if (width <= 0) {
        return {};
    }
    auto createEllipsisText = [&]() -> theme::StringWithMargins {
        const auto ellipsisTheme = context.theme().forPart(theme::Part::Ellipsis);
        const auto ellipsisBlock = ellipsisTheme.block();
        return theme::layout::stylePaddingAndMargins(String{1U, ellipsisBlock}, ellipsisTheme);
    };
    const auto actions = collectActions();
    auto result = theme::StringWithMargins{};
    for (const auto &action : actions) {
        auto actionText = renderActionText(action, context);
        if (result.joinedDisplayWidth(actionText) > width) {
            auto ellipsis = createEllipsisText();
            if (result.joinedDisplayWidth(ellipsis) > width) {
                break;
            }
            result.join(ellipsis);
            break;
        }
        result.join(actionText);
    }
    return result.text();
}

void ActionHelp::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    const auto helpText = renderHelpText(context, context.surfaceRect().width());
    buffer.drawText(helpText, context.surfaceRect(), TextOptions{Alignment::TopRight});
}

auto ActionHelp::focusedSurfaceChain(const PagePtr &page) const -> std::vector<SurfacePtr> {
    auto chain = std::vector<SurfacePtr>{};
    auto focusSurface = page->focusSurface().lock();
    auto focusBelongsToPage = false;
    for (auto parent = focusSurface; parent != nullptr; parent = parent->parent().lock()) {
        if (parent == page) {
            focusBelongsToPage = true;
            break;
        }
    }
    if (focusSurface != nullptr && focusSurface->flags().isVisibleInTree() && focusBelongsToPage) {
        for (auto surface = focusSurface; surface != nullptr && surface != page; surface = surface->parent().lock()) {
            chain.emplace_back(surface);
        }
    }
    chain.emplace_back(page);
    return chain;
}

void ActionHelp::addCandidates(
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
        if (!action->isEnabled() || !action->help().isVisibleInFooter() || action->help().name().empty() ||
            action->keys().mainKeyCount() == 0) {
            continue;
        }
        seen.emplace_back(action);
        candidates.push_back(
            ActionCandidate{
                .action = action,
                .chainIndex = chainIndex,
                .actionIndex = actionIndex,
            });
        actionIndex += 1;
    }
}

auto ActionHelp::renderActionText(const ActionPtr &action, const PaintContext &context) -> theme::StringWithMargins {
    const auto keyLabels = action->keys().mainKeyLabels();
    auto keysWithBrackets =
        theme::layout::encloseInBrackets(keyLabels, context.theme(), theme::Part::KeyBracket, theme::Part::Key);
    if (action->help().name().empty()) {
        return keysWithBrackets;
    }
    auto actionName =
        theme::layout::stylePaddingAndMargins(action->help().name(), context.theme(), theme::Part::ActionName);
    return keysWithBrackets.joined(actionName);
}

}
