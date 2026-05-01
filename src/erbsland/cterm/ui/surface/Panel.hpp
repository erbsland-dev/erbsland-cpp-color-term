// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Surface.hpp"

#include <memory>

namespace erbsland::cterm::ui::surface {

class Panel;
using PanelPtr = std::shared_ptr<Panel>;

/// A fill surface that paints a themed background behind its child surfaces.
class Panel : public Surface {
public:
    /// Create a panel surface.
    explicit Panel(ProtectedTag) noexcept;
    ~Panel() override = default;

    /// Create a panel that grows to fill free space in layouts.
    /// @return The new panel instance.
    [[nodiscard]] static auto create() -> PanelPtr;

public: // implement Surface
    [[nodiscard]] auto isOpaque() const noexcept -> bool override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

protected: // implement Surface
    void initializeUi() override;
};

}
