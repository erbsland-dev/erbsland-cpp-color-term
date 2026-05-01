// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "LayoutProposal.hpp"
#include "SizePolicy.hpp"

#include "../geometry/Margins.hpp"
#include "../geometry/Rectangle.hpp"
#include "../geometry/Size.hpp"

namespace erbsland::cterm::ui {

/// The measured and configured layout metrics of a surface.
///
/// The minimum, maximum, and preferred sizes describe the surface content rectangle, excluding margins. Margins are
/// non-negative recommendations for parent-owned outer spacing. Parent layouts decide whether and how to apply or
/// propagate them.
class LayoutMetrics {
    constexpr static auto cDefaultSize = Size{20, 1};

public:
    /// Create layout metrics with default limits, preferred size, and size policy.
    LayoutMetrics() = default;
    /// Create explicit layout metrics.
    /// @param minimum The minimum size.
    /// @param maximum The maximum size.
    /// @param preferred The preferred size.
    /// @param policy The size policy.
    /// @param margins The recommended outer margins. Negative values are clamped to zero.
    constexpr LayoutMetrics(
        const Size minimum,
        const Size maximum,
        const Size preferred,
        const SizePolicy policy,
        const Margins margins = {}) noexcept :
        _minimum{minimum},
        _maximum{maximum},
        _preferred{preferred},
        _policy{policy},
        _margins{clampedMargins(margins)} {}

    // defaults
    ~LayoutMetrics() = default;
    LayoutMetrics(const LayoutMetrics &) = default;
    LayoutMetrics(LayoutMetrics &&) = default;
    auto operator=(const LayoutMetrics &) -> LayoutMetrics & = default;
    auto operator=(LayoutMetrics &&) -> LayoutMetrics & = default;

public: // operators
    auto operator==(const LayoutMetrics &other) const noexcept -> bool = default;
    auto operator!=(const LayoutMetrics &other) const noexcept -> bool = default;

public:
    /// Get the minimum size.
    [[nodiscard]] auto minimum() const noexcept -> const Size & { return _minimum; }
    /// Set the minimum size.
    void setMinimum(const Size &size) noexcept { _minimum = size; }
    /// Set the minimum height.
    void setMinimumHeight(Coordinate height) noexcept { _minimum.setHeight(height); }
    /// Set the minimum width.
    void setMinimumWidth(Coordinate width) noexcept { _minimum.setWidth(width); }
    /// Get the maximum size.
    [[nodiscard]] auto maximum() const noexcept -> const Size & { return _maximum; }
    /// Set the maximum size.
    void setMaximum(const Size &size) noexcept { _maximum = size; }
    /// Set the maximum height.
    void setMaximumHeight(Coordinate height) noexcept { _maximum.setHeight(height); }
    /// Set the maximum width.
    void setMaximumWidth(Coordinate width) noexcept { _maximum.setWidth(width); }
    /// Get the preferred size.
    [[nodiscard]] auto preferred() const noexcept -> const Size & { return _preferred; }
    /// Set the preferred size.
    void setPreferred(const Size &size) noexcept { _preferred = size; }
    /// Set the preferred height.
    void setPreferredHeight(Coordinate height) noexcept { _preferred.setHeight(height); }
    /// Set the preferred width.
    void setPreferredWidth(Coordinate width) noexcept { _preferred.setWidth(width); }
    /// Get the size policy.
    [[nodiscard]] auto sizePolicy() const noexcept -> const SizePolicy & { return _policy; }
    /// Set the size policy for both dimensions.
    void setSizePolicy(const SizePolicy &policy) noexcept { _policy = policy; }
    /// @overload
    void setSizePolicy(const DimensionPolicy::Type &policy) noexcept { _policy = SizePolicy{policy}; }
    /// Set the width size policy.
    void setSizePolicyForWidth(DimensionPolicy::Type policyType, int factor = 1) noexcept {
        _policy.setWidth({policyType, factor});
    }
    /// Set the height size policy.
    void setSizePolicyForHeight(DimensionPolicy::Type policyType, int factor = 1) noexcept {
        _policy.setHeight({policyType, factor});
    }
    /// Fix the height to a single value.
    void setFixedHeight(Coordinate height) noexcept;
    /// Fix the width to a single value.
    void setFixedWidth(Coordinate width) noexcept;
    /// Fix both dimensions to a single size.
    void setFixedSize(Size size) noexcept;
    /// Get the recommended outer margins.
    /// Margins are always zero or positive and are not part of the layout sizes.
    [[nodiscard]] auto margins() const noexcept -> const Margins & { return _margins; }
    /// Replace the recommended outer margins.
    /// Negative sides are clamped to zero.
    void setMargins(Margins margins) noexcept { _margins = clampedMargins(margins); }
    /// Resolve a concrete size for this metrics object against a proposal.
    [[nodiscard]] auto resolvedSize(const LayoutProposal &proposal) const noexcept -> Size;

private:
    [[nodiscard]] constexpr static auto clampedMargins(const Margins margins) noexcept -> Margins {
        return margins.expandedPositive();
    }

private:
    Size _minimum;                  ///< The minimum size.
    Size _maximum{Size::maximum()}; ///< The maximum size.
    Size _preferred{cDefaultSize};  ///< The preferred size.
    SizePolicy _policy;             ///< The size policy.
    Margins _margins;               ///< Recommended parent-owned outer margins.
};

}
