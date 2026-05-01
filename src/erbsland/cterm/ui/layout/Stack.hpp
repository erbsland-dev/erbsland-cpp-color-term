// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Layout.hpp"

#include "../../geometry/Orientation.hpp"

#include <memory>

namespace erbsland::cterm::ui::layout {

class Stack;
using StackPtr = std::shared_ptr<Stack>;

/// A layout that stacks surfaces vertically or horizontally.
class Stack : public Layout {
public:
    /// Create a stack layout with the given orientation.
    /// @param orientation The stacking direction for child surfaces.
    explicit Stack(Orientation orientation, ProtectedTag) noexcept;
    ~Stack() override = default;

    /// Create a new stack layout.
    /// @param orientation The stacking direction for child surfaces.
    /// @return The new stack layout instance.
    [[nodiscard]] static auto create(Orientation orientation) -> StackPtr;

public: // implement Layout
    /// Measure this stack from its visible children.
    /// @param scope Measurement access.
    /// @param proposal The proposed stack size.
    [[nodiscard]] auto onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept
        -> LayoutMetrics override;
    /// Recalculate all child rectangles inside this stack.
    /// @param scope The layout scope.
    void onLayout(LayoutScope &scope) noexcept override;

private:
    Orientation _orientation{Orientation::Vertical};
};

}
