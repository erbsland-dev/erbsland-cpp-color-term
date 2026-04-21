// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "LayoutConstraints.hpp"

namespace erbsland::cterm::ui::layout::impl {

auto LayoutConstraints::fromMetrics(const LayoutMetrics &metrics) noexcept -> LayoutConstraints {
    return {
        DimensionConstraints::fromMetrics(metrics, Orientation::Horizontal),
        DimensionConstraints::fromMetrics(metrics, Orientation::Vertical)};
}

auto LayoutConstraints::resolve(const Size availableSize) const noexcept -> Size {
    return {
        _widthConstraints.resolve(availableSize.width()),
        _heightConstraints.resolve(availableSize.height()),
    };
}

}
