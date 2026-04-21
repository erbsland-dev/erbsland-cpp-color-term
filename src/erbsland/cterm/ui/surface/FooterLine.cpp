// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "FooterLine.hpp"

#include <algorithm>
#include <utility>

namespace erbsland::cterm::ui::surface {

FooterLine::FooterLine(ProtectedTag protectedTag) noexcept : Panel{protectedTag} {
    themeAttributes().setElement(theme::Element::FooterLine);
    editLayoutMetrics().setFixedHeight(1);
}

auto FooterLine::create() -> FooterLinePtr {
    auto result = std::make_shared<FooterLine>(ProtectedTag{});
    result->initializeChildren();
    return result;
}

void FooterLine::setText(String text) {
    _leftText->setText(std::move(text));
}

void FooterLine::setText(const std::string_view text) {
    _leftText->setText(text);
}

void FooterLine::displayMessage(String message, const CharStyle style, const milliseconds timeout) {
    _messageQueue.push_back(Message{.text = std::move(message), .style = style, .timeout = timeout});
    if (!_hasMessage) {
        showNextMessage();
    }
}

void FooterLine::displayMessage(const std::string_view message, const CharStyle style, const milliseconds timeout) {
    displayMessage(String{message, EncodingErrors::Replace}, style, timeout);
}

void FooterLine::hideMessage() {
    _messageGeneration += 1;
    _hasMessage = false;
    _messageText->flags().setVisible(false);
    _messageText->setText(String{});
    if (!_messageQueue.empty()) {
        showNextMessage();
    }
}

void FooterLine::onLayout(LayoutScope &scope) noexcept {
    const auto newParentSize = scope.size();
    const auto width = std::max(newParentSize.width(), 0);
    _leftText->updateForWidth(width);
    const auto leftWidth = std::min(_leftText->textDisplayWidth(), width);
    const auto actionHelpWidth = std::max(width - leftWidth, 0);
    scope.place(_leftText, Rectangle{{0, 0}, Size{leftWidth, 1}});
    scope.place(_actionHelp, Rectangle{{leftWidth, 0}, Size{actionHelpWidth, 1}});
    if (_messageText->flags().isVisible()) {
        scope.place(_messageText, Rectangle{{0, 0}, Size{width, 1}});
    }
}

void FooterLine::initializeChildren() {
    _leftText = DynamicText::create();
    _actionHelp = ActionHelp::create();
    _messageText = DynamicText::create(String{}, Alignment::TopCenter);
    _messageText->flags().setVisible(false);
    addSurface(_leftText);
    addSurface(_actionHelp);
    addSurface(_messageText);
}

void FooterLine::showNextMessage() {
    if (_messageQueue.empty()) {
        return;
    }
    const auto message = std::move(_messageQueue.front());
    _messageQueue.pop_front();
    _hasMessage = true;
    _messageGeneration += 1;
    const auto generation = _messageGeneration;
    _messageText->setText(styledMessage(message));
    _messageText->flags().setVisible(true);
    flags().setPaintOutdated();
    if (message.timeout <= milliseconds{0}) {
        return;
    }
    const auto weakThis = std::weak_ptr<FooterLine>{std::static_pointer_cast<FooterLine>(shared_from_this())};
    scheduler().addSingleShot(
        [weakThis, generation]() -> void {
            if (const auto footer = weakThis.lock(); footer != nullptr && footer->_messageGeneration == generation) {
                footer->hideMessage();
            }
        },
        message.timeout);
}

auto FooterLine::styledMessage(const Message &message) noexcept -> String {
    auto result = String{};
    result.append(message.style, message.text);
    return result;
}

}
