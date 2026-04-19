// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Surface.hpp"

#include "../Char.hpp"

#include <memory>

namespace erbsland::cterm::ui {

class Page;
using PagePtr = std::shared_ptr<Page>;

/// A single page on the screen.
class Page : public Surface {
public:
    /// Create a page instance through the protected construction path.
    explicit Page(ProtectedTag) {}
    ~Page() override = default;

public:
    /// Create a new page.
    [[nodiscard]] static auto create() -> PagePtr;

public:
    /// Get the background character.
    [[nodiscard]] auto background() const noexcept -> const Char &;
    /// Set the background character.
    void setBackground(Char background) noexcept;
    /// Test if the page has a focus surface.
    [[nodiscard]] auto hasFocusSurface() const noexcept -> bool;
    /// Get the current focus surface.
    [[nodiscard]] auto focusSurface() const noexcept -> const SurfaceWeakPtr &;
    /// Move the focus to another surface.
    /// Passing an expired or foreign surface clears the current focus.
    /// @param surface The new focus surface.
    void focusTo(const SurfaceWeakPtr &surface) noexcept;

public: // implement Surface
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;
    /// Route the key press to the focused surface chain and finally to the page itself.
    /// @param keyPressEvent The key press event.
    void onKeyPress(KeyPressEvent &keyPressEvent) noexcept override;
    [[nodiscard]] auto isOpaque() const noexcept -> bool override;
    [[nodiscard]] auto isPage() const noexcept -> bool override;

private:
    /// Test if the given surface belongs to this page.
    /// @param surface The surface to test.
    /// @return `true` if the surface is a descendant of this page.
    [[nodiscard]] auto isFocusSurfaceValid(const SurfacePtr &surface) const noexcept -> bool;
    Char _background{U' ', Color::reset()}; ///< The background for this page.
    SurfaceWeakPtr _focusSurface;           ///< The surface that currently has the main focus.
};

}
