// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../Layout.hpp"

#include "../../Orientation.hpp"

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

public:
    /// Recalculate all child rectangles inside this stack.
    /// @param newSize The available stack size.
    void onLayout(Size newSize) noexcept override;

private:
    Orientation _orientation{Orientation::Vertical};
};


}
