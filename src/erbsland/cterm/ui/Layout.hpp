// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Surface.hpp"

#include <memory>


namespace erbsland::cterm::ui {

/// A layout is a surface that arranges other surfaces in a specific way.
class Layout : public Surface {
    /// The default geometry for layouts
    constexpr static auto cDefaultGeometry = Geometry{Size{}, Size::maximum(), Size{}, SizePolicy{SizePolicy::Grow}};

protected:
    /// Create the base layout surface with the default layout geometry.
    Layout() noexcept;

public:
    ~Layout() override = default;

public:
    /// Set the optional background fill.
    /// @param background The background character.
    void setBackground(Char background) noexcept;
    /// Clear the optional background fill.
    void clearBackground() noexcept;
    /// Get the optional background fill.
    /// @return The configured background character, if any.
    [[nodiscard]] auto background() const noexcept -> std::optional<Char>;

public: // implement Surface
    /// Test if the layout paints every cell in its rectangle.
    /// @return `true` if a background is configured.
    [[nodiscard]] auto isOpaque() const noexcept -> bool override;
    /// Paint the layout background and all child surfaces.
    /// @param buffer The target buffer.
    /// @param context The paint context for the current paint pass.
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

private:
    std::optional<Char> _background;
};

}
