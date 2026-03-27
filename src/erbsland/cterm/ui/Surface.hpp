// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Geometry.hpp"
#include "KeyBindings.hpp"
#include "PaintContext.hpp"
#include "ui_namespace.hpp"

#include "event/KeyPressEvent.hpp"
#include "event/Scheduler.hpp"

#include "../Rectangle.hpp"
#include "../WritableBuffer.hpp"

#include <memory>


namespace erbsland::cterm::ui {

class Application;
class Surface;
using SurfacePtr = std::shared_ptr<Surface>;
using SurfaceWeakPtr = std::weak_ptr<Surface>;

/// A surface is a rectangular area on a page.
/// The base class for all UI elements that can be displayed in the terminal.
class Surface : public std::enable_shared_from_this<Surface> {
    friend class Application;

protected:
    /// Create a surface with default geometry.
    Surface() = default;
    /// Create a surface with explicit geometry.
    /// @param geometry The initial surface geometry.
    explicit Surface(Geometry geometry);

public:
    // defaults
    virtual ~Surface() = default;
    Surface(const Surface &) = delete;
    Surface(Surface &&) = delete;
    auto operator=(const Surface &) -> Surface & = delete;
    auto operator=(Surface &&) -> Surface & = delete;

public:
    /// Get the parent surface.
    /// @return The weak pointer to the parent surface, or an empty weak pointer for top-level pages.
    [[nodiscard]] auto parent() const noexcept -> const SurfaceWeakPtr &;
    /// Get all direct child surfaces.
    /// @return The list of child surfaces.
    [[nodiscard]] auto children() const noexcept -> const std::vector<SurfacePtr> &;
    /// Add a child surface.
    /// @param child The child surface to add.
    /// @throws std::invalid_argument If the child would create a cycle in the surface tree.
    void addChild(SurfacePtr child);
    /// Remove a child surface.
    /// @param child The child surface to remove.
    void removeChild(const SurfacePtr &child);
    /// Get the effective rectangle in page coordinates.
    /// @return The rectangle of this surface relative to the page.
    [[nodiscard]] auto effectiveRect() const noexcept -> Rectangle;
    /// Translate a child rectangle into this surface's coordinate space.
    /// @param childRect The child rectangle relative to this surface.
    /// @return The child rectangle in the effective coordinate space of this surface.
    [[nodiscard]] auto effectiveChildRect(Rectangle childRect) const noexcept -> Rectangle;
    /// Test if this surface requires a layout pass.
    /// @return `true` if the layout is outdated.
    [[nodiscard]] auto isLayoutOutdated() const noexcept -> bool;
    /// Mark this surface and its ancestors as requiring layout.
    void setLayoutOutdated();
    /// Test if this surface requires repainting.
    /// @return `true` if the paint state is outdated.
    [[nodiscard]] auto isPaintOutdated() const noexcept -> bool;
    /// Mark this surface and its ancestors as requiring repainting.
    void setPaintOutdated();
    /// Get the assigned rectangle of this surface.
    /// @return The current surface rectangle.
    [[nodiscard]] auto rectangle() const noexcept -> const Rectangle & { return _rectangle; }
    /// Set the assigned rectangle of this surface.
    /// @param rect The new rectangle.
    void setRectangle(const Rectangle &rect) noexcept { _rectangle = rect; }
    /// Get mutable access to the geometry description.
    /// @return The geometry description.
    [[nodiscard]] auto geometry() noexcept -> Geometry &;
    /// Get read-only access to the geometry description.
    /// @return The geometry description.
    [[nodiscard]] auto geometry() const noexcept -> const Geometry &;
    /// Replace the geometry description.
    /// @param geometry The new geometry description.
    void setGeometry(Geometry geometry);
    /// Access and create key bindings for this surface.
    /// A call of this method creates a new key bindings instance for this surface.
    /// Key presses are handled by the key bindings object.
    [[nodiscard]] auto keyBindings() noexcept -> KeyBindings &;
    /// Access and create a scheduler for this surface.
    /// A call of this method creates a new scheduler for this surface.
    /// Scheduled actions are handled by the scheduler and callbacks run on the application UI thread.
    [[nodiscard]] auto scheduler() noexcept -> Scheduler &;

    /// Test if this surface is a page.
    /// @return `true` for `Page`, otherwise `false`.
    [[nodiscard]] virtual auto isPage() const noexcept -> bool;
    /// Test if this surface paints every cell in its rectangle.
    /// @return `true` if the surface is opaque.
    [[nodiscard]] virtual auto isOpaque() const noexcept -> bool;

public: // events
    /// Layout all direct children within the given parent size.
    /// @param newParentSize The available size inside this surface.
    virtual void onLayout(Size newParentSize) noexcept;
    /// Paint this surface and its children into the target buffer.
    /// @param buffer The target buffer.
    /// @param context The paint context for the current paint pass.
    virtual void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept;
    /// Handle a key press event.
    /// This is the low-level event handler for key press events.
    /// If a surface handles a key press event, it should mark it as handled.
    /// @param keyPressEvent The key press event to handle.
    virtual void onKeyPress(KeyPressEvent &keyPressEvent) noexcept;

protected:
    /// Set the parent surface pointer.
    /// @param parent The new parent surface.
    void setParent(const SurfacePtr &parent);
    /// Protected tag for safe construction of all surface classes.
    struct ProtectedTag {};

private:
    /// Tests if the given surface is in the parent chain of this surface.
    [[nodiscard]] auto isInParentChain(const SurfacePtr &surface) const noexcept -> bool;
    /// Get the existing scheduler without creating one.
    /// @return The existing scheduler, or `nullptr` if none was created yet.
    [[nodiscard]] auto schedulerIfExists() noexcept -> Scheduler *;
    /// @overload
    [[nodiscard]] auto schedulerIfExists() const noexcept -> const Scheduler *;

protected:
    SurfaceWeakPtr _parent;            ///< The parent surface. `Page` is the only surface without a parent.
    std::vector<SurfacePtr> _children; ///< All child surfaces.
    Rectangle _rectangle;              ///< The current rectangle of the surface.
    Geometry _geometry;                ///< The geometry of the surface relative to the parent surface.
    bool _isLayoutOutdated{false};     ///< Indicates if the layout needs to be recalculated.
    bool _isPaintOutdated{false};      ///< Indicates if the surface needs to be repainted on the display.

private:
    KeyBindingsPtr _keyBindings; ///< The key bindings object.
    SchedulerPtr _scheduler;     ///< The scheduler.
};

}
