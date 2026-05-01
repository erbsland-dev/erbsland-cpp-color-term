// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Button.hpp"

#include "../../theme/LayoutHelper.hpp"
#include "../../theme/ThemePainter.hpp"

#include <stdexcept>
#include <utility>

namespace erbsland::cterm::ui::surface {

Button::Button(ButtonActionPtr action, ProtectedTag) : _action{std::move(action)} {
    if (_action == nullptr) {
        throw std::invalid_argument{"Button action must not be null."};
    }
    editLayoutMetrics().setFixedHeight(1);
}

auto Button::create(ButtonActionPtr action) -> ButtonPtr {
    auto result = std::make_shared<Button>(std::move(action), ProtectedTag{});
    result->initializeUi();
    return result;
}

void Button::initializeUi() {
    Surface::initializeUi();
    themeAttributes().setElement(theme::Element::Button);
    flags().setFocusable(true);
    _action->bindButton(std::static_pointer_cast<Button>(shared_from_this()));
    actions().add(_action);
}

auto Button::action() const noexcept -> const ButtonActionPtr & {
    return _action;
}

auto Button::isEnabled() const noexcept -> bool {
    return flags().isEnabled();
}

auto Button::onMeasure(MeasureScope &scope, [[maybe_unused]] const LayoutProposal &proposal) noexcept -> LayoutMetrics {
    const auto buttonText = renderButtonText(scope.theme());
    const auto size = Size{buttonText.displayWidth(), 1};
    auto result = layoutMetrics();
    result.setPreferred(size);
    result.setMinimum(size.limitedWith(Size{2, 1}, Orientation::Horizontal));
    result.setMargins(buttonText.margins());
    return result;
}

void Button::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    theme::ThemePainter{buffer, context.theme().forPart(theme::Part::Background)}.fill(context.surfaceRect());
    auto buttonText = renderButtonText(context.theme());
    buffer.drawText(Position{}, buttonText.text());
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

auto Button::renderButtonText(const theme::ThemeAccessor &themeAccessor) const -> theme::StringWithMargins {
    auto text = theme::layout::stylePaddingAndMargins(_action->help().name(), themeAccessor, theme::Part::Text);
    auto keyLabels = _action->keys().mainKeyLabels();
    auto keysWithBrackets =
        theme::layout::encloseInBrackets(keyLabels, themeAccessor, theme::Part::KeyBracket, theme::Part::Key);
    text.join(keysWithBrackets);
    return theme::layout::encloseInBrackets(text.text(), themeAccessor, theme::Part::Border, theme::Part::None);
}

auto Button::triggerFromKey(const Key &key) -> bool {
    return _action->trigger(ActionTriggerContext{key, weak_from_this()});
}

}
