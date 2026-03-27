// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../Surface.hpp"

#include <memory>


namespace erbsland::cterm::ui::surface {

class Panel;
using PanelPtr = std::shared_ptr<Panel>;

/// A fill surface that paints an optional background behind its child surfaces.
class Panel : public Surface {
public:
    /// Create a panel surface.
    explicit Panel(ProtectedTag) noexcept;
    ~Panel() override = default;

    /// Create a panel that grows to fill free space in layouts.
    /// @return The new panel instance.
    [[nodiscard]] static auto create() noexcept -> PanelPtr;

public:
    /// Set the optional panel background.
    /// @param background The background fill character.
    void setBackground(Char background) noexcept;
    /// Remove the panel background.
    void clearBackground() noexcept;
    /// Get the optional panel background.
    /// @return The configured background, if any.
    [[nodiscard]] auto background() const noexcept -> std::optional<Char>;

public: // implement Surface
    [[nodiscard]] auto isOpaque() const noexcept -> bool override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

private:
    std::optional<Char> _background; ///< The background character for the panel
};

}
