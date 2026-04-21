// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Button.hpp"

#include "../../theme/ThemePainter.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace erbsland::cterm::ui::surface {

Button::Button(ButtonActionPtr action, ProtectedTag) : Surface{theme::Element::Button}, _action{std::move(action)} {
    if (_action == nullptr) {
        throw std::invalid_argument{"Button action must not be null."};
    }
    flags().setFocusable(true);
    refreshLayoutSize();
}

auto Button::create(ButtonActionPtr action) -> ButtonPtr {
    auto result = std::make_shared<Button>(std::move(action), ProtectedTag{});
    result->_action->bindButton(result);
    result->actions().add(result->_action);
    return result;
}

auto Button::action() const noexcept -> const ButtonActionPtr & {
    return _action;
}

auto Button::isEnabled() const noexcept -> bool {
    return flags().isEnabled();
}

auto Button::preferredSize() const -> Size {
    if (_action == nullptr) {
        return Size{2, 1};
    }
    auto width = Coordinate{2}; // side indicators
    width += 2 * cHorizontalPadding;
    width += String{_action->help().name()}.displayWidth();
    if (!_action->keys().empty()) {
        width += cGapBeforeKey + keyText().displayWidth() + 2;
    }
    return Size{std::max(width, Coordinate{2}), 1};
}

void Button::refreshLayoutSize() {
    const auto size = preferredSize();
    auto metrics = editLayoutMetrics();
    metrics.setMinimumSize(Size{std::min(size.width(), Coordinate{2}), 1}).setPreferredSize(size).setFixedHeight(1);
}

void Button::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    theme::ThemePainter{buffer, context.theme().forPart(theme::Part::Background)}.fill(context.surfaceRect());
    buffer.drawText(context.surfaceRect().topLeft(), renderButton(context.themeContext()));
}

void Button::onKeyPress(KeyPressEvent &keyPressEvent) noexcept {
    Surface::onKeyPress(keyPressEvent);
    if (keyPressEvent.isHandled()) {
        return;
    }
    if (keyPressEvent.key() == Key::Enter || keyPressEvent.key() == Key::Space) {
        if (triggerFromKey(keyPressEvent.key())) {
            keyPressEvent.setHandled();
        }
    }
}

auto Button::renderButton(const ThemeContext &themeContext) const -> String {
    const auto themeAccessor = themeContext.theme();
    const auto textStyle = themeAccessor.forPart(theme::Part::Text).style();
    auto result = String{};
    result.append(themeAccessor.forPart(theme::Part::Border).block(theme::BlockRole::HorizontalWest));
    result.append(static_cast<std::size_t>(cHorizontalPadding), U' ', textStyle);
    if (_action != nullptr) {
        result.appendWithBaseStyle(String{_action->help().name()}, textStyle);
    }
    const auto key = keyText();
    if (!key.empty()) {
        result.append(static_cast<std::size_t>(cGapBeforeKey), U' ', textStyle);
        result.append(themeAccessor.forPart(theme::Part::KeyBracket).block(theme::BlockRole::HorizontalWest));
        result.appendWithBaseStyle(key, themeAccessor.forPart(theme::Part::Key).style());
        result.append(themeAccessor.forPart(theme::Part::KeyBracket).block(theme::BlockRole::HorizontalEast));
    }
    result.append(static_cast<std::size_t>(cHorizontalPadding), U' ', textStyle);
    result.append(themeAccessor.forPart(theme::Part::Border).block(theme::BlockRole::HorizontalEast));
    return result;
}

auto Button::keyText() const -> String {
    if (_action == nullptr) {
        return {};
    }
    const auto mainKeys = _action->keys().mainKeys();
    if (mainKeys.empty()) {
        return {};
    }
    return String{mainKeys.front().toDisplayText(false)};
}

auto Button::triggerFromKey(const Key &key) -> bool {
    if (_action == nullptr) {
        return false;
    }
    return _action->trigger(ActionTriggerContext{key, weak_from_this()});
}

}
