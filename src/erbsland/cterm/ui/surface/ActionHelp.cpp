// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ActionHelp.hpp"

#include <algorithm>
#include <chrono>
#include <ranges>
#include <string_view>
#include <utility>

namespace erbsland::cterm::ui::surface {

using namespace std::chrono_literals;

constexpr auto cActionItemSeparator = std::string_view{"  "};
constexpr auto cActionNameSeparator = std::string_view{" "};
constexpr auto cOmittedMarker = std::string_view{"…"};

ActionHelp::ActionHelp([[maybe_unused]] ProtectedTag protectedTag) noexcept : Surface{theme::Element::ActionHelp} {
    editLayoutMetrics().setFixedHeight(1);
}

auto ActionHelp::create() -> ActionHelpPtr {
    auto result = std::make_shared<ActionHelp>(ProtectedTag{});
    const auto weakResult = std::weak_ptr<ActionHelp>{result};
    result->scheduler().addRepeated(
        [weakResult]() -> void {
            if (const auto surface = weakResult.lock(); surface != nullptr) {
                surface->flags().setPaintOutdated();
            }
        },
        100ms);
    return result;
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
    const auto normalizedWidth = std::max(width, 0);
    auto result = String{};
    if (normalizedWidth == 0) {
        return result;
    }
    const auto actions = collectActions();
    const auto separator = [&]() -> String {
        auto text = String{};
        text.appendStyled(cActionItemSeparator, context.theme().forPart(theme::Part::Text).style());
        return text;
    }();
    const auto omitted = [&]() -> String {
        auto text = String{};
        text.appendStyled(cOmittedMarker, context.theme().forPart(theme::Part::Text).style());
        return text;
    }();
    auto renderedActionCount = std::size_t{0};
    for (const auto &action : actions) {
        auto item = renderActionItem(action, context);
        auto proposedWidth = result.displayWidth() + item.displayWidth();
        if (!result.empty()) {
            proposedWidth += separator.displayWidth();
        }
        if (proposedWidth > normalizedWidth) {
            if (result.empty()) {
                if (omitted.displayWidth() <= normalizedWidth) {
                    result += omitted;
                }
                break;
            }
            const auto omittedWidth = result.displayWidth() + separator.displayWidth() + omitted.displayWidth();
            if (renderedActionCount > 0 && omittedWidth <= normalizedWidth) {
                result += separator;
                result += omitted;
            }
            break;
        }
        if (!result.empty()) {
            result += separator;
        }
        result += item;
        renderedActionCount += 1;
    }
    return result;
}

void ActionHelp::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    const auto helpText = renderHelpText(context, context.surfaceRect().width());
    buffer.drawText(helpText, context.surfaceRect(), TextOptions{Alignment::TopRight});
}

auto ActionHelp::nearestPage() noexcept -> PagePtr {
    for (auto surface = weak_from_this().lock(); surface != nullptr; surface = surface->parent().lock()) {
        if (surface->isPage()) {
            return std::dynamic_pointer_cast<Page>(surface);
        }
    }
    return {};
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
                .actionIndex = actionIndex++,
            });
    }
}

auto ActionHelp::renderActionItem(const ActionPtr &action, const PaintContext &context) -> String {
    auto result = String{};
    if (action == nullptr) {
        return result;
    }
    const auto themeAccessor = context.theme();
    const auto keyStyle = themeAccessor.forPart(theme::Part::Key).style();
    const auto nameStyle = themeAccessor.forPart(theme::Part::Text).style();
    const auto keys = action->keys().mainKeys();
    for (auto index = std::size_t{0}; index < keys.size(); ++index) {
        if (index > 0) {
            result.append(themeAccessor.forPart(theme::Part::KeyBracket).block(theme::BlockRole::Center));
        } else {
            result.append(themeAccessor.forPart(theme::Part::KeyBracket).block(theme::BlockRole::West));
        }
        result.appendWithBaseStyle(String{keys[index].toDisplayText(false)}, keyStyle);
    }
    result.append(themeAccessor.forPart(theme::Part::KeyBracket).block(theme::BlockRole::East));
    result.appendStyled(cActionNameSeparator, nameStyle);
    result.appendWithBaseStyle(String{action->help().name()}, nameStyle);
    return result;
}

}
