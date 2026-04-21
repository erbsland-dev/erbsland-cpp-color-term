// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "LayoutMetrics.hpp"
#include "LayoutProposal.hpp"
#include "SurfacePtr.hpp"
#include "ThemeContext.hpp"

#include <functional>

namespace erbsland::cterm::ui {

/// Gives a surface access to framework-driven child measurement.
class MeasureScope final {
public:
    /// The function used to measure one surface.
    using MeasureFunction = std::function<LayoutMetrics(const SurfacePtr &, const LayoutProposal &)>;

public:
    /// Create a scope without measurement support.
    MeasureScope() = default;
    /// Create a scope with a measurement callback.
    /// @param measureFunction The callback used by `measure()`.
    explicit MeasureScope(MeasureFunction measureFunction, ThemeContext themeContext = {}) noexcept;

    // defaults
    ~MeasureScope() = default;
    MeasureScope(const MeasureScope &) = default;
    MeasureScope(MeasureScope &&) = default;
    auto operator=(const MeasureScope &) -> MeasureScope & = delete;
    auto operator=(MeasureScope &&) -> MeasureScope & = delete;

public:
    /// Measure a child surface for a proposal.
    /// @param surface The surface to measure.
    /// @param proposal The proposed size.
    /// @return The measured metrics, or empty metrics for null surfaces.
    [[nodiscard]] auto measure(const SurfacePtr &surface, const LayoutProposal &proposal) const noexcept
        -> LayoutMetrics;
    /// Access the theme context of the surface currently being measured.
    [[nodiscard]] auto themeContext() const noexcept -> const ThemeContext & { return _themeContext; }
    /// Access the theme of the surface currently being measured.
    [[nodiscard]] auto theme() const noexcept -> theme::ThemeAccessor { return _themeContext.theme(); }

private:
    MeasureFunction _measureFunction; ///< The framework-provided measurement callback.
    ThemeContext _themeContext;       ///< The resolved theme context for the measured surface.
};

}
