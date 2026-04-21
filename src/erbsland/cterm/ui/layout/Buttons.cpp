// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Buttons.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace erbsland::cterm::ui::layout {

Buttons::Buttons(ProtectedTag) noexcept {
    themeAttributes().setElement(theme::Element::Buttons);
}

auto Buttons::create() -> ButtonsPtr {
    return std::make_shared<Buttons>(ProtectedTag{});
}

auto Buttons::addAction(ButtonActionPtr action) -> surface::ButtonPtr {
    auto button = surface::Button::create(std::move(action));
    auto result = button;
    addButton(std::move(button));
    return result;
}

void Buttons::addButton(surface::ButtonPtr button) {
    insertButton(childStorage().size(), std::move(button));
}

void Buttons::insertButton(const std::size_t index, surface::ButtonPtr button) {
    childStorage().insert(index, std::move(button));
}

auto Buttons::buttonCount() const noexcept -> std::size_t {
    return childStorage().size();
}

auto Buttons::button(const std::size_t index) const -> surface::ButtonPtr {
    return std::dynamic_pointer_cast<surface::Button>(childStorage()[index]);
}

auto Buttons::onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept -> LayoutMetrics {
    const auto spacing = scope.theme().forPart(theme::Part::Spacing).margins().spacing();
    const auto width = proposal.width().valueOr(layoutMetrics().preferred().width());
    const auto normalizedWidth = std::max(width, Coordinate{1});
    const auto height = requiredHeight(normalizedWidth, spacing);
    auto result = layoutMetrics();
    result.setMinimumHeight(height);
    result.setPreferred(Size{normalizedWidth, height});
    return result;
}

void Buttons::onLayout(LayoutScope &scope) noexcept {
    auto y = Coordinate{0};
    const auto newSize = scope.size();
    const auto spacing = scope.theme().forPart(theme::Part::Spacing).margins().spacing();
    for (const auto &row : buildRows(newSize, spacing)) {
        auto x = std::max((newSize.width() - row.width) / 2, Coordinate{0});
        for (const auto &layoutButton : row.buttons) {
            const auto buttonSize = Size{layoutButton.width, 1};
            scope.place(layoutButton.button, Rectangle{{x, y}, buttonSize});
            x += layoutButton.width + spacing.width();
        }
        y += 1 + spacing.height();
    }
}

void Buttons::onKeyPress(KeyPressEvent &keyPressEvent) noexcept {
    switch (keyPressEvent.key().type()) {
    case Key::Left:
    case Key::Up:
    case Key::BackTab:
        if (moveFocus(false)) {
            keyPressEvent.setHandled();
        }
        return;
    case Key::Right:
    case Key::Down:
    case Key::Tab:
        if (moveFocus(true)) {
            keyPressEvent.setHandled();
        }
        return;
    default:
        break;
    }
    Surface::onKeyPress(keyPressEvent);
}

void Buttons::willAdd(
    const SurfacePtr &surface, [[maybe_unused]] const std::size_t index, [[maybe_unused]] const LayoutDataPtr &data) {
    if (std::dynamic_pointer_cast<surface::Button>(surface) == nullptr) {
        throw std::invalid_argument{"Buttons accepts only button surfaces."};
    }
}

void Buttons::didAdd(
    const SurfacePtr &surface,
    [[maybe_unused]] const std::size_t index,
    [[maybe_unused]] const LayoutDataPtr &data) noexcept {
    const auto button = std::dynamic_pointer_cast<surface::Button>(surface);
    if (button != nullptr) {
        attachAction(button->action());
    }
}

void Buttons::didRemove(
    const SurfacePtr &surface,
    [[maybe_unused]] const std::size_t index,
    [[maybe_unused]] const LayoutDataPtr &data) noexcept {
    const auto button = std::dynamic_pointer_cast<surface::Button>(surface);
    if (button != nullptr) {
        detachActionIfUnused(button->action());
    }
}

void Buttons::willReplace(
    [[maybe_unused]] const SurfacePtr &oldSurface,
    const SurfacePtr &newSurface,
    [[maybe_unused]] const std::size_t index,
    [[maybe_unused]] const LayoutDataPtr &oldData,
    [[maybe_unused]] const LayoutDataPtr &newData) {
    if (std::dynamic_pointer_cast<surface::Button>(newSurface) == nullptr) {
        throw std::invalid_argument{"Buttons accepts only button surfaces."};
    }
}

void Buttons::didReplace(
    const SurfacePtr &oldSurface,
    const SurfacePtr &newSurface,
    [[maybe_unused]] const std::size_t index,
    [[maybe_unused]] const LayoutDataPtr &oldData,
    [[maybe_unused]] const LayoutDataPtr &newData) noexcept {
    const auto oldButton = std::dynamic_pointer_cast<surface::Button>(oldSurface);
    const auto newButton = std::dynamic_pointer_cast<surface::Button>(newSurface);
    if (newButton != nullptr) {
        attachAction(newButton->action());
    }
    if (oldButton != nullptr) {
        detachActionIfUnused(oldButton->action());
    }
}

auto Buttons::visibleButtons() const -> std::vector<surface::ButtonPtr> {
    auto result = std::vector<surface::ButtonPtr>{};
    result.reserve(childStorage().size());
    for (const auto &surface : childStorage()) {
        auto button = std::dynamic_pointer_cast<surface::Button>(surface);
        if (button != nullptr && button->flags().isVisible()) {
            result.emplace_back(std::move(button));
        }
    }
    return result;
}

auto Buttons::focusableButtons() const -> std::vector<surface::ButtonPtr> {
    auto result = std::vector<surface::ButtonPtr>{};
    for (auto &button : visibleButtons()) {
        if (button->flags().isFocusable() && button->isEnabled() && isButtonFullyVisible(button)) {
            result.emplace_back(std::move(button));
        }
    }
    return result;
}

auto Buttons::buildRows(const Size size, const Size spacing) const -> std::vector<LayoutRow> {
    const auto availableWidth = std::max(size.width(), Coordinate{1});
    auto rows = std::vector<LayoutRow>{};
    auto currentRow = LayoutRow{};
    for (const auto &button : visibleButtons()) {
        button->refreshLayoutSize();
        const auto preferredWidth = button->layoutMetrics().preferred().width();
        const auto width = std::max(std::min(preferredWidth, availableWidth), Coordinate{1});
        const auto addedWidth = currentRow.buttons.empty() ? width : width + spacing.width();
        if (!currentRow.buttons.empty() && currentRow.width + addedWidth > availableWidth) {
            rows.emplace_back(std::move(currentRow));
            currentRow = LayoutRow{};
        }
        if (!currentRow.buttons.empty()) {
            currentRow.width += spacing.width();
        }
        currentRow.buttons.push_back(LayoutButton{button, width});
        currentRow.width += width;
    }
    if (!currentRow.buttons.empty()) {
        rows.emplace_back(std::move(currentRow));
    }
    return rows;
}

auto Buttons::requiredHeight(const Coordinate width, const Size spacing) const -> Coordinate {
    const auto rows = buildRows(Size{std::max(width, Coordinate{1}), Size::maximum().height()}, spacing);
    if (rows.empty()) {
        return 0;
    }
    return static_cast<Coordinate>(rows.size()) + static_cast<Coordinate>(rows.size() - 1) * spacing.height();
}

auto Buttons::isButtonFullyVisible(const surface::ButtonPtr &button) const noexcept -> bool {
    return button != nullptr && localSurfaceRect().contains(button->rectangle());
}

auto Buttons::moveFocus(const bool forward) noexcept -> bool {
    const auto buttons = focusableButtons();
    if (buttons.empty()) {
        return false;
    }
    auto currentIndex = buttons.size();
    for (auto index = std::size_t{0}; index < buttons.size(); ++index) {
        if (buttons[index]->flags().hasFocus()) {
            currentIndex = index;
            break;
        }
    }
    if (currentIndex == buttons.size()) {
        return (forward ? buttons.front() : buttons.back())->requestFocus();
    }
    const auto nextIndex =
        forward ? (currentIndex + 1) % buttons.size() : (currentIndex + buttons.size() - 1) % buttons.size();
    if (nextIndex == currentIndex) {
        return false;
    }
    return buttons[nextIndex]->requestFocus();
}

void Buttons::attachAction(const ActionPtr &action) {
    if (action == nullptr || actions().contains(action)) {
        return;
    }
    actions().add(action);
}

void Buttons::detachActionIfUnused(const ActionPtr &action) {
    if (action == nullptr || actionsPtr() == nullptr) {
        return;
    }
    for (const auto &surface : childStorage()) {
        const auto button = std::dynamic_pointer_cast<surface::Button>(surface);
        if (button != nullptr && button->action() == action) {
            return;
        }
    }
    actions().remove(action);
}

}
