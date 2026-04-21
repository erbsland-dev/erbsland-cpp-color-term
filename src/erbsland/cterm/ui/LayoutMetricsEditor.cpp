// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "LayoutMetricsEditor.hpp"

#include "Surface.hpp"

#include <utility>

namespace erbsland::cterm::ui {

auto LayoutMetricsEditor::apply(LayoutMetrics layoutMetrics) -> LayoutMetricsEditor & {
    _surface.setLayoutMetrics(std::move(layoutMetrics));
    return *this;
}

auto LayoutMetricsEditor::setMinimumSize(const Size size) -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setMinimum(size);
    return apply(std::move(layoutMetrics));
}

auto LayoutMetricsEditor::setMinimumWidth(const Coordinate width) -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setMinimumWidth(width);
    return apply(std::move(layoutMetrics));
}

auto LayoutMetricsEditor::setMinimumHeight(const Coordinate height) -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setMinimumHeight(height);
    return apply(std::move(layoutMetrics));
}

auto LayoutMetricsEditor::setMaximumSize(const Size size) -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setMaximum(size);
    return apply(std::move(layoutMetrics));
}

auto LayoutMetricsEditor::setMaximumWidth(const Coordinate width) -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setMaximumWidth(width);
    return apply(std::move(layoutMetrics));
}

auto LayoutMetricsEditor::setMaximumHeight(const Coordinate height) -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setMaximumHeight(height);
    return apply(std::move(layoutMetrics));
}

auto LayoutMetricsEditor::setPreferredSize(const Size size) -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setPreferred(size);
    return apply(std::move(layoutMetrics));
}

auto LayoutMetricsEditor::setPreferredWidth(const Coordinate width) -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setPreferredWidth(width);
    return apply(std::move(layoutMetrics));
}

auto LayoutMetricsEditor::setPreferredHeight(const Coordinate height) -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setPreferredHeight(height);
    return apply(std::move(layoutMetrics));
}

auto LayoutMetricsEditor::setSizePolicy(const SizePolicy policy) -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setSizePolicy(policy);
    return apply(std::move(layoutMetrics));
}

auto LayoutMetricsEditor::setSizePolicy(const DimensionPolicy::Type policy) -> LayoutMetricsEditor & {
    return setSizePolicy(SizePolicy{policy});
}

auto LayoutMetricsEditor::setSizePolicyForWidth(const DimensionPolicy::Type policyType, const int factor)
    -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setSizePolicyForWidth(policyType, factor);
    return apply(std::move(layoutMetrics));
}

auto LayoutMetricsEditor::setSizePolicyForHeight(const DimensionPolicy::Type policyType, const int factor)
    -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setSizePolicyForHeight(policyType, factor);
    return apply(std::move(layoutMetrics));
}

auto LayoutMetricsEditor::setFixedWidth(const Coordinate width) -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setFixedWidth(width);
    return apply(std::move(layoutMetrics));
}

auto LayoutMetricsEditor::setFixedHeight(const Coordinate height) -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setFixedHeight(height);
    return apply(std::move(layoutMetrics));
}

auto LayoutMetricsEditor::setFixedSize(const Size size) -> LayoutMetricsEditor & {
    auto layoutMetrics = _surface._layoutMetrics;
    layoutMetrics.setFixedSize(size);
    return apply(std::move(layoutMetrics));
}

}
