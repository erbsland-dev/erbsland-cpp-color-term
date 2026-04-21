// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "LayoutDimension.hpp"

#include "../geometry/Size.hpp"

namespace erbsland::cterm::ui {

/// A proposed size passed to measurement.
class LayoutProposal final {
public:
    /// Create an unconstrained proposal.
    LayoutProposal() = default;
    /// Create a proposal from explicit dimensions.
    /// @param width The width proposal.
    /// @param height The height proposal.
    constexpr LayoutProposal(LayoutDimension width, LayoutDimension height) noexcept : _width{width}, _height{height} {}

    // defaults
    ~LayoutProposal() = default;
    LayoutProposal(const LayoutProposal &) = default;
    LayoutProposal(LayoutProposal &&) = default;
    auto operator=(const LayoutProposal &) -> LayoutProposal & = default;
    auto operator=(LayoutProposal &&) -> LayoutProposal & = default;

public:
    /// Create an unconstrained proposal.
    [[nodiscard]] static constexpr auto unconstrained() noexcept -> LayoutProposal { return {}; }
    /// Create a proposal that bounds both dimensions by the given size.
    /// @param size The maximum available size.
    [[nodiscard]] static constexpr auto atMost(Size size) noexcept -> LayoutProposal {
        return {LayoutDimension::atMost(size.width()), LayoutDimension::atMost(size.height())};
    }
    /// Create a proposal with an exact size.
    /// @param size The exact size.
    [[nodiscard]] static constexpr auto exact(Size size) noexcept -> LayoutProposal {
        return {LayoutDimension::exact(size.width()), LayoutDimension::exact(size.height())};
    }

public:
    /// Access the width proposal.
    [[nodiscard]] constexpr auto width() const noexcept -> LayoutDimension { return _width; }
    /// Access the height proposal.
    [[nodiscard]] constexpr auto height() const noexcept -> LayoutDimension { return _height; }
    /// Create a copy with a different width proposal.
    /// @param width The new width proposal.
    [[nodiscard]] constexpr auto withWidth(LayoutDimension width) const noexcept -> LayoutProposal {
        return {width, _height};
    }
    /// Create a copy with a different height proposal.
    /// @param height The new height proposal.
    [[nodiscard]] constexpr auto withHeight(LayoutDimension height) const noexcept -> LayoutProposal {
        return {_width, height};
    }
    /// Convert all bounded dimensions to a size, using `Size::maximum()` for unbounded dimensions.
    [[nodiscard]] constexpr auto maximumSize() const noexcept -> Size {
        return Size{_width.valueOr(Size::maximum().width()), _height.valueOr(Size::maximum().height())};
    }

private:
    LayoutDimension _width;
    LayoutDimension _height;
};

}
