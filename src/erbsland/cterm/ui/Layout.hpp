// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Surface.hpp"
#include "SurfaceManager.hpp"

#include <memory>

namespace erbsland::cterm::ui {

/// A layout is a surface that arranges other surfaces in a specific way.
class Layout : public Surface, protected SurfaceManager {
    /// The default layout metrics for layouts.
    constexpr static auto cDefaultLayoutSize =
        LayoutMetrics{Size{}, Size::maximum(), Size{}, SizePolicy{SizePolicy::Grow}};

protected:
    /// Create the base layout surface with the default layout metrics.
    Layout() noexcept;

public:
    ~Layout() override = default;

public: // implement Surface
    [[nodiscard]] auto isOpaque() const noexcept -> bool override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;
};

}
