// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "DynamicText.hpp"

#include <algorithm>
#include <utility>

namespace erbsland::cterm::ui::surface {

DynamicText::DynamicText(String text, const Alignment alignment, ProtectedTag protectedTag) :
    StaticText{theme::Element::DynamicText, std::move(text), alignment, protectedTag} {
}

auto DynamicText::create(String text, Alignment alignment) -> DynamicTextPtr {
    auto result = std::make_shared<DynamicText>(std::move(text), alignment, ProtectedTag{});
    result->initializeUi();
    return result;
}

auto DynamicText::create(const std::string_view text, const Alignment alignment) -> DynamicTextPtr {
    return create(String{text, EncodingErrors::Replace}, alignment);
}

void DynamicText::setUpdateFn(UpdateFn updateFn) {
    _updateFn = std::move(updateFn);
}

void DynamicText::clearUpdateFn() noexcept {
    _updateFn = {};
}

void DynamicText::updateText() {
    if (!_updateFn) {
        return;
    }
    auto updatedText = text();
    _updateFn(updatedText, std::max(rectangle().width(), 0));
    setText(std::move(updatedText));
}

void DynamicText::setUpdateInterval(const milliseconds interval) {
    _updateInterval = interval;
    if (interval <= milliseconds{0}) {
        if (_scheduledUpdateRef.isValid()) {
            scheduler().remove(_scheduledUpdateRef);
            _scheduledUpdateRef = {};
        }
        return;
    }
    if (_scheduledUpdateRef.isValid()) {
        scheduler().setDelayOrInterval(_scheduledUpdateRef, interval);
        return;
    }
    const auto weakThis = std::weak_ptr<DynamicText>{std::static_pointer_cast<DynamicText>(shared_from_this())};
    _scheduledUpdateRef = scheduler().addRepeated(
        [weakThis]() -> void {
            if (const auto surface = weakThis.lock(); surface != nullptr) {
                surface->updateText();
            }
        },
        interval);
}

}
