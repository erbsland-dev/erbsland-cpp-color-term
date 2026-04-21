// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "LayoutDimension.hpp"

#include <algorithm>

namespace erbsland::cterm::ui {

auto LayoutDimension::resolve(
    const Coordinate minimum,
    const Coordinate maximum,
    const Coordinate preferred,
    const DimensionPolicy policy) const noexcept -> Coordinate {
    const auto boundedMinimum = std::max(Coordinate{0}, std::min(minimum, maximum));
    const auto boundedMaximum = std::max(boundedMinimum, maximum);
    if (isExact()) {
        return _value;
    }
    const auto boundedPreferred = std::clamp(preferred, boundedMinimum, boundedMaximum);
    if (isAtMost()) {
        if (policy.type() == DimensionPolicy::Grow) {
            return std::clamp(_value, boundedMinimum, boundedMaximum);
        }
        return std::clamp(std::min(boundedPreferred, _value), boundedMinimum, boundedMaximum);
    }
    return boundedPreferred;
}

}
