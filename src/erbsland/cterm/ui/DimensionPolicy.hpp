// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>


namespace erbsland::cterm::ui {

/// The size policy of a single surface dimension.
/// The resolved size should always stay within `minimum <= value <= maximum`.
class DimensionPolicy {
public:
    /// The supported dimension policy strategies.
    enum Type : uint8_t {
        /// Use the preferred size and only grow or shrink if necessary.
        Preferred,
        /// Start with the preferred size and grow as much as possible.
        Grow,
        /// Start with the preferred size and shrink as much as possible.
        Shrink,
    };

public:
    /// Create the default preferred policy with factor `1`.
    DimensionPolicy() = default;
    /// Create a policy with explicit type and distribution factor.
    /// @param type The policy type.
    /// @param factor The relative factor used when distributing extra space or shrinkage.
    constexpr DimensionPolicy(const Type type, const int factor = 1) noexcept : // NOLINT(*-explicit-constructor)
        _type{type}, _factor{factor} {}

    // defaults
    ~DimensionPolicy() = default;
    DimensionPolicy(const DimensionPolicy &) = default;
    DimensionPolicy(DimensionPolicy &&) = default;
    auto operator=(const DimensionPolicy &) -> DimensionPolicy & = default;
    auto operator=(DimensionPolicy &&) -> DimensionPolicy & = default;

public:
    /// Get the policy type.
    /// @return The configured policy type.
    [[nodiscard]] auto type() const noexcept -> Type { return _type; }
    /// Set the policy type.
    /// @param type The new policy type.
    void setType(const Type type) noexcept { _type = type; }
    /// Get the relative distribution factor.
    /// @return The configured factor.
    [[nodiscard]] auto factor() const noexcept -> int { return _factor; }
    /// Set the relative distribution factor.
    /// @param factor The new factor.
    void setFactor(const int factor) noexcept { _factor = factor; }

private:
    Type _type{Preferred};
    int _factor{1};
};

}
