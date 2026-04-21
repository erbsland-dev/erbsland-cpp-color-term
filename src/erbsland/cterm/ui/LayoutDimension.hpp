// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "DimensionPolicy.hpp"

#include "../geometry/Coordinate.hpp"

#include <cstdint>

namespace erbsland::cterm::ui {

/// A layout proposal for one dimension.
class LayoutDimension final {
public:
    /// The kind of proposed size.
    enum class Type : uint8_t {
        Unspecified, ///< No limit is known for this dimension.
        AtMost,      ///< The surface may use up to the stored size.
        Exact,       ///< The surface is measured for exactly the stored size.
    };

public:
    /// Create an unspecified proposal.
    LayoutDimension() = default;
    /// Create a proposal with explicit type and value.
    /// @param type The proposal type.
    /// @param value The proposed size, clamped to zero or larger.
    constexpr LayoutDimension(const Type type, const Coordinate value) noexcept :
        _type{type}, _value{value < 0 ? Coordinate{0} : value} {}

    // defaults
    ~LayoutDimension() = default;
    LayoutDimension(const LayoutDimension &) = default;
    LayoutDimension(LayoutDimension &&) = default;
    auto operator=(const LayoutDimension &) -> LayoutDimension & = default;
    auto operator=(LayoutDimension &&) -> LayoutDimension & = default;

public:
    /// Create an unspecified proposal.
    [[nodiscard]] static constexpr auto unspecified() noexcept -> LayoutDimension { return {}; }
    /// Create an upper-bounded proposal.
    /// @param value The upper bound.
    [[nodiscard]] static constexpr auto atMost(Coordinate value) noexcept -> LayoutDimension {
        return {Type::AtMost, value};
    }
    /// Create an exact proposal.
    /// @param value The exact size.
    [[nodiscard]] static constexpr auto exact(Coordinate value) noexcept -> LayoutDimension {
        return {Type::Exact, value};
    }

public:
    /// Test if this proposal is unspecified.
    [[nodiscard]] constexpr auto isUnspecified() const noexcept -> bool { return _type == Type::Unspecified; }
    /// Test if this proposal is upper-bounded.
    [[nodiscard]] constexpr auto isAtMost() const noexcept -> bool { return _type == Type::AtMost; }
    /// Test if this proposal is exact.
    [[nodiscard]] constexpr auto isExact() const noexcept -> bool { return _type == Type::Exact; }
    /// Test if this proposal carries a usable bound.
    [[nodiscard]] constexpr auto hasBound() const noexcept -> bool { return _type != Type::Unspecified; }
    /// Access the proposal type.
    [[nodiscard]] constexpr auto type() const noexcept -> Type { return _type; }
    /// Access the stored size. The value is meaningful only when `hasBound()` is true.
    [[nodiscard]] constexpr auto value() const noexcept -> Coordinate { return _value; }
    /// Access the stored size or a fallback for unspecified proposals.
    /// @param fallback The fallback value.
    [[nodiscard]] constexpr auto valueOr(Coordinate fallback) const noexcept -> Coordinate {
        return hasBound() ? _value : fallback;
    }
    /// Resolve this proposal against one dimension's layout metrics.
    /// @param minimum The minimum size.
    /// @param maximum The maximum size.
    /// @param preferred The preferred size.
    /// @param policy The size policy.
    /// @return The resolved size.
    [[nodiscard]] auto
    resolve(Coordinate minimum, Coordinate maximum, Coordinate preferred, DimensionPolicy policy) const noexcept
        -> Coordinate;

private:
    Type _type{Type::Unspecified};
    Coordinate _value{};
};

}
