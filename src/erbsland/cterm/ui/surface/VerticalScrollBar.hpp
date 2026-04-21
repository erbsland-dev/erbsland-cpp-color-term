// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "AbstractScrollBar.hpp"

namespace erbsland::cterm::ui::surface {

class VerticalScrollBar;
using VerticalScrollBarPtr = std::shared_ptr<VerticalScrollBar>;

/// A one-column vertical scroll bar that grows along its height.
class VerticalScrollBar final : public AbstractScrollBar {
public:
    /// Create a vertical scroll bar.
    explicit VerticalScrollBar(ProtectedTag) noexcept;
    ~VerticalScrollBar() override = default;

public:
    /// Create a vertical scroll bar.
    /// @return The new vertical scroll bar.
    [[nodiscard]] static auto create() noexcept -> VerticalScrollBarPtr;
};

}
