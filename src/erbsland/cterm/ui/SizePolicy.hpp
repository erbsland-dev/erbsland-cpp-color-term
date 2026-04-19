// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "DimensionPolicy.hpp"

namespace erbsland::cterm::ui {

/// The size policy for both width and height of a surface.
class SizePolicy {
public:
    /// Shortcut for the dimension policy type enum.
    using Type = DimensionPolicy::Type;
    /// Preferred size without active growth or shrink behavior.
    static constexpr auto Preferred = Type::Preferred;
    /// Prefer to grow when extra space is available.
    static constexpr auto Grow = Type::Grow;
    /// Prefer to shrink when space is constrained.
    static constexpr auto Shrink = Type::Shrink;

public:
    /// Create the default preferred policy for both dimensions.
    SizePolicy() = default;
    /// Create a uniform size policy for both dimensions.
    /// @param size The policy to use for width and height.
    constexpr explicit SizePolicy(const DimensionPolicy size) noexcept : _width{size}, _height{size} {}
    /// Create explicit policies for width and height.
    /// @param width The width policy.
    /// @param height The height policy.
    constexpr SizePolicy(const DimensionPolicy width, const DimensionPolicy height) noexcept :
        _width{width}, _height{height} {}

    // defaults
    ~SizePolicy() = default;
    SizePolicy(const SizePolicy &) = default;
    SizePolicy(SizePolicy &&) = default;
    auto operator=(const SizePolicy &) -> SizePolicy & = default;
    auto operator=(SizePolicy &&) -> SizePolicy & = default;

public:
    /// Get the width policy.
    /// @return The width policy.
    [[nodiscard]] auto width() const noexcept -> DimensionPolicy { return _width; }
    /// Set the width policy.
    /// @param width The new width policy.
    void setWidth(const DimensionPolicy width) noexcept { _width = width; }
    /// Get the height policy.
    /// @return The height policy.
    [[nodiscard]] auto height() const noexcept -> DimensionPolicy { return _height; }
    /// Set the height policy.
    /// @param height The new height policy.
    void setHeight(const DimensionPolicy height) noexcept { _height = height; }

private:
    DimensionPolicy _width;
    DimensionPolicy _height;
};

}
