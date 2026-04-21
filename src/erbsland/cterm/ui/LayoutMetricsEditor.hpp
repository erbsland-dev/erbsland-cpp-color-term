// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "LayoutMetrics.hpp"

namespace erbsland::cterm::ui {

class Surface;

/// Edit the layout metrics of a surface and invalidate layout when they change.
class LayoutMetricsEditor {
public:
    /// Create a layout metrics editor for a surface.
    /// @param surface The surface whose layout metrics are edited.
    explicit constexpr LayoutMetricsEditor(Surface &surface) noexcept : _surface{surface} {}

    // defaults
    ~LayoutMetricsEditor() = default;
    LayoutMetricsEditor(const LayoutMetricsEditor &) = delete;
    LayoutMetricsEditor(LayoutMetricsEditor &&) = delete;
    auto operator=(const LayoutMetricsEditor &) -> LayoutMetricsEditor & = delete;
    auto operator=(LayoutMetricsEditor &&) -> LayoutMetricsEditor & = delete;

public:
    /// Replace the minimum size.
    /// @param size The new minimum size.
    auto setMinimumSize(Size size) -> LayoutMetricsEditor &;
    /// Replace the minimum width.
    /// @param width The new minimum width.
    auto setMinimumWidth(Coordinate width) -> LayoutMetricsEditor &;
    /// Replace the minimum height.
    /// @param height The new minimum height.
    auto setMinimumHeight(Coordinate height) -> LayoutMetricsEditor &;
    /// Replace the maximum size.
    /// @param size The new maximum size.
    auto setMaximumSize(Size size) -> LayoutMetricsEditor &;
    /// Replace the maximum width.
    /// @param width The new maximum width.
    auto setMaximumWidth(Coordinate width) -> LayoutMetricsEditor &;
    /// Replace the maximum height.
    /// @param height The new maximum height.
    auto setMaximumHeight(Coordinate height) -> LayoutMetricsEditor &;
    /// Replace the preferred size.
    /// @param size The new preferred size.
    auto setPreferredSize(Size size) -> LayoutMetricsEditor &;
    /// Replace the preferred width.
    /// @param width The new preferred width.
    auto setPreferredWidth(Coordinate width) -> LayoutMetricsEditor &;
    /// Replace the preferred height.
    /// @param height The new preferred height.
    auto setPreferredHeight(Coordinate height) -> LayoutMetricsEditor &;
    /// Replace the size policy.
    /// @param policy The new size policy.
    auto setSizePolicy(SizePolicy policy) -> LayoutMetricsEditor &;
    /// Replace both size policies with the same policy type.
    /// @param policy The new policy type.
    auto setSizePolicy(DimensionPolicy::Type policy) -> LayoutMetricsEditor &;
    /// Replace the width size policy.
    /// @param policyType The new width policy type.
    /// @param factor The relative distribution factor.
    auto setSizePolicyForWidth(DimensionPolicy::Type policyType, int factor = 1) -> LayoutMetricsEditor &;
    /// Replace the height size policy.
    /// @param policyType The new height policy type.
    /// @param factor The relative distribution factor.
    auto setSizePolicyForHeight(DimensionPolicy::Type policyType, int factor = 1) -> LayoutMetricsEditor &;
    /// Fix the width to a single value.
    /// @param width The fixed width.
    auto setFixedWidth(Coordinate width) -> LayoutMetricsEditor &;
    /// Fix the height to a single value.
    /// @param height The fixed height.
    auto setFixedHeight(Coordinate height) -> LayoutMetricsEditor &;
    /// Fix both dimensions to a single size.
    /// @param size The fixed size.
    auto setFixedSize(Size size) -> LayoutMetricsEditor &;

private:
    [[nodiscard]] auto apply(LayoutMetrics layoutMetrics) -> LayoutMetricsEditor &;

private:
    Surface &_surface; ///< The edited surface.
};

}
