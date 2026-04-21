// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "MeasureScope.hpp"

#include <utility>

namespace erbsland::cterm::ui {

MeasureScope::MeasureScope(MeasureFunction measureFunction, ThemeContext themeContext) noexcept :
    _measureFunction{std::move(measureFunction)}, _themeContext{std::move(themeContext)} {
}

auto MeasureScope::measure(const SurfacePtr &surface, const LayoutProposal &proposal) const noexcept -> LayoutMetrics {
    if (surface == nullptr || !_measureFunction) {
        return {};
    }
    return _measureFunction(surface, proposal);
}

}
