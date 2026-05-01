// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "AbstractScrollBar.hpp"

namespace erbsland::cterm::ui::surface {

class HorizontalScrollBar;
using HorizontalScrollBarPtr = std::shared_ptr<HorizontalScrollBar>;

/// A one-row horizontal scroll bar that grows along its width.
class HorizontalScrollBar final : public AbstractScrollBar {
public:
    /// Create a horizontal scroll bar.
    explicit HorizontalScrollBar(ProtectedTag) noexcept;
    ~HorizontalScrollBar() override = default;

public:
    /// Create a horizontal scroll bar.
    /// @return The new horizontal scroll bar.
    [[nodiscard]] static auto create() -> HorizontalScrollBarPtr;

private:
    /// Initialize theme attributes after construction.
    void initializeUi() override;
};

}
