// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "LayoutMetrics.hpp"

namespace erbsland::cterm::ui {

void LayoutMetrics::setFixedHeight(const Coordinate height) noexcept {
    _minimum.setHeight(height);
    _maximum.setHeight(height);
    _preferred.setHeight(height);
    _policy.setHeight({DimensionPolicy::Preferred});
}

void LayoutMetrics::setFixedWidth(const Coordinate width) noexcept {
    _minimum.setWidth(width);
    _maximum.setWidth(width);
    _preferred.setWidth(width);
    _policy.setWidth({DimensionPolicy::Preferred});
}

void LayoutMetrics::setFixedSize(const Size size) noexcept {
    _minimum = size;
    _maximum = size;
    _preferred = size;
    _policy = SizePolicy{DimensionPolicy::Preferred};
}

auto LayoutMetrics::resolvedSize(const LayoutProposal &proposal) const noexcept -> Size {
    return Size{
        proposal.width().resolve(_minimum.width(), _maximum.width(), _preferred.width(), _policy.width()),
        proposal.height().resolve(_minimum.height(), _maximum.height(), _preferred.height(), _policy.height())};
}

}
