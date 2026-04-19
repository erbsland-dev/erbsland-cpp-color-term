// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "SizePolicy.hpp"

#include "../Rectangle.hpp"
#include "../Size.hpp"

namespace erbsland::cterm::ui {

/// The geometry and size policy of a surface.
class Geometry {
    constexpr static auto cDefaultSize = Size{20, 1};

public:
    /// Create a geometry description with default limits, preferred size, and size policy.
    Geometry() = default;
    /// Create a geometry description with explicit limits, preferred size, and size policy.
    /// @param minimum The minimum size.
    /// @param maximum The maximum size.
    /// @param preferred The preferred size.
    /// @param policy The size policy.
    constexpr Geometry(const Size minimum, const Size maximum, const Size preferred, const SizePolicy policy) noexcept :
        _minimum{minimum}, _maximum{maximum}, _preferred{preferred}, _policy{policy} {}

    // defaults
    ~Geometry() = default;
    Geometry(const Geometry &) = default;
    Geometry(Geometry &&) = default;
    auto operator=(const Geometry &) -> Geometry & = default;
    auto operator=(Geometry &&) -> Geometry & = default;

public:
    /// Get the minimum size.
    /// @return The minimum size.
    [[nodiscard]] auto minimum() const noexcept -> const Size & { return _minimum; }
    /// Set the minimum size.
    /// @param size The new minimum size.
    void setMinimum(const Size &size) noexcept { _minimum = size; }
    /// Set the minimum height.
    /// @param height The new minimum height.
    void setMinimumHeight(const int height) noexcept { _minimum.setHeight(height); }
    /// Set the minimum width.
    /// @param width The new minimum width.
    void setMinimumWidth(const int width) noexcept { _minimum.setWidth(width); }
    /// Get the maximum size.
    /// @return The maximum size.
    [[nodiscard]] auto maximum() const noexcept -> const Size & { return _maximum; }
    /// Set the maximum size.
    /// @param size The new maximum size.
    void setMaximum(const Size &size) noexcept { _maximum = size; }
    /// Set the maximum height.
    /// @param height The new maximum height.
    void setMaximumHeight(const int height) noexcept { _maximum.setHeight(height); }
    /// Set the maximum width.
    /// @param width The new maximum width.
    void setMaximumWidth(const int width) noexcept { _maximum.setWidth(width); }
    /// Get the preferred size.
    /// @return The preferred size.
    [[nodiscard]] auto preferred() const noexcept -> const Size & { return _preferred; }
    /// Set the preferred size.
    /// @param size The new preferred size.
    void setPreferred(const Size &size) noexcept { _preferred = size; }
    /// Set the preferred height.
    /// @param height The new preferred height.
    void setPreferredHeight(const int height) noexcept { _preferred.setHeight(height); }
    /// Set the preferred width.
    /// @param width The new preferred width.
    void setPreferredWidth(const int width) noexcept { _preferred.setWidth(width); }
    /// Get the size policy.
    /// @return The size policy for width and height.
    [[nodiscard]] auto sizePolicy() const noexcept -> const SizePolicy & { return _policy; }
    /// Set the size policy for both dimensions.
    /// @param policy The new size policy.
    void setSizePolicy(const SizePolicy &policy) noexcept { _policy = policy; }
    /// Set the width size policy.
    /// @param policyType The width policy type.
    /// @param factor The relative distribution factor.
    void setSizePolicyForWidth(DimensionPolicy::Type policyType, int factor = 1) noexcept {
        _policy.setWidth({policyType, factor});
    }
    /// Set the height size policy.
    /// @param policyType The height policy type.
    /// @param factor The relative distribution factor.
    void setSizePolicyForHeight(DimensionPolicy::Type policyType, int factor = 1) noexcept {
        _policy.setHeight({policyType, factor});
    }
    /// Fix the height to a single value.
    /// @param height The fixed height.
    void setFixedHeight(const Coordinate height) noexcept {
        _minimum.setHeight(height);
        _maximum.setHeight(height);
        _preferred.setHeight(height);
        _policy.setHeight({DimensionPolicy::Type::Preferred});
    }
    /// Fix the width to a single value.
    /// @param width The fixed width.
    void setFixedWidth(const Coordinate width) noexcept {
        _minimum.setWidth(width);
        _maximum.setWidth(width);
        _preferred.setWidth(width);
        _policy.setWidth({DimensionPolicy::Type::Preferred});
    }
    /// Fix both dimensions to a single size.
    /// @param size The fixed size.
    void setFixedSize(const Size size) noexcept {
        _minimum = size;
        _maximum = size;
        _preferred = size;
        _policy = SizePolicy{DimensionPolicy::Type::Preferred};
    }

private:
    Size _minimum;                  ///< The minimum size.
    Size _maximum{Size::maximum()}; ///< The maximum size.
    Size _preferred{cDefaultSize};  ///< The preferred size.
    SizePolicy _policy;             ///< The size policy.
};

}
