// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "LayoutScope.hpp"

#include <utility>

namespace erbsland::cterm::ui {

LayoutScope::LayoutScope(
    const Size size,
    ThemeContext themeContext,
    MeasureScope::MeasureFunction measureFunction,
    PlaceFunction placeFunction) noexcept :
    _size{size},
    _themeContext{std::move(themeContext)},
    _measureScope{std::move(measureFunction), _themeContext},
    _placeFunction{std::move(placeFunction)} {
}

auto LayoutScope::measure(const SurfacePtr &surface, const LayoutProposal &proposal) const noexcept -> LayoutMetrics {
    return _measureScope.measure(surface, proposal);
}

void LayoutScope::place(const SurfacePtr &surface, const Rectangle rectangle) const noexcept {
    if (surface == nullptr || !_placeFunction) {
        return;
    }
    _placeFunction(surface, rectangle);
}

}
