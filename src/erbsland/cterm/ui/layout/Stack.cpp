// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Stack.hpp"

#include "impl/StackLayoutItems.hpp"

namespace erbsland::cterm::ui::layout {

Stack::Stack(const Orientation orientation, ProtectedTag) noexcept : _orientation{orientation} {
}

auto Stack::create(Orientation orientation) -> StackPtr {
    return std::make_shared<Stack>(orientation, ProtectedTag{});
}

void Stack::onLayout(const Size newSize) noexcept {
    auto items = impl::StackLayoutItems::fromChildren(children(), _orientation, newSize);
    items.resolveMainSizes(newSize.coordinate(_orientation));
    items.applyLayout(_orientation);
    _isLayoutOutdated = false;
}

}
