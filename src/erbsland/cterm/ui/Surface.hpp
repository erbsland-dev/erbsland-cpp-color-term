// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Actions.hpp"
#include "FocusChange.hpp"
#include "LayoutContext.hpp"
#include "LayoutMetrics.hpp"
#include "LayoutMetricsEditor.hpp"
#include "LayoutProposal.hpp"
#include "LayoutScope.hpp"
#include "MeasureScope.hpp"
#include "PaintContext.hpp"
#include "SurfaceContainer.hpp"
#include "SurfaceFlags.hpp"
#include "SurfacePtr.hpp"
#include "SurfaceThemeAttributes.hpp"
#include "ui_namespace.hpp"

#include "event/KeyPressEvent.hpp"
#include "event/Scheduler.hpp"

#include "../geometry/Rectangle.hpp"
#include "../WritableBuffer.hpp"

namespace erbsland::cterm::ui {

class Application;
class Page;
using PagePtr = std::shared_ptr<Page>;
class SurfaceContainer;

/// A surface is a rectangular area on a page.
/// The base class for all UI elements that can be displayed in the terminal.
class Surface : public std::enable_shared_from_this<Surface> {
    friend class Application;
    friend class Display;
    friend class LayoutMetricsEditor;
    friend class Page;
    friend class SurfaceContainer;
    friend class SurfaceFlags;

protected:
    /// Create a surface with default layout metrics.
    Surface() = default;
    /// Create a surface with a theme element.
    /// @param themeElement The theme element for this surface.
    explicit Surface(theme::Element themeElement) noexcept;
    /// Create a surface with explicit layout metrics.
    /// @param layoutMetrics The initial surface layout metrics.
    explicit Surface(LayoutMetrics layoutMetrics);
    /// Create a surface with a theme element and explicit layout metrics.
    /// @param themeElement The theme element for this surface.
    /// @param layoutMetrics The initial surface layout metrics.
    Surface(theme::Element themeElement, LayoutMetrics layoutMetrics);

public:
    // defaults
    virtual ~Surface() = default;
    Surface(const Surface &) = delete;
    Surface(Surface &&) = delete;
    auto operator=(const Surface &) -> Surface & = delete;
    auto operator=(Surface &&) -> Surface & = delete;

public: // child surface management
    /// Get the parent surface.
    /// @return The weak pointer to the parent surface, or an empty weak pointer for top-level pages.
    [[nodiscard]] auto parent() const noexcept -> const SurfaceWeakPtr &;
    /// Get mutable access to the child surface container.
    /// @return The child surface container.
    [[nodiscard]] virtual auto surfaces() noexcept -> AbstractSurfaceContainer &;
    /// Get read-only access to the child surface container.
    /// @return The child surface container.
    [[nodiscard]] virtual auto surfaces() const noexcept -> const AbstractSurfaceContainer &;
    /// Add a child surface.
    /// @param surface The child surface to add.
    /// @throws std::invalid_argument If the child is invalid or would create a cycle in the surface tree.
    void addSurface(SurfacePtr surface);

public:
    /// Get the full local rectangle of this surface.
    /// @return The local rectangle with its top-left corner at `0,0`.
    [[nodiscard]] auto localSurfaceRect() const noexcept -> Rectangle {
        return Rectangle{Position{}, _rectangle.size()};
    }
    /// Convert a rectangle from local surface coordinates to screen coordinates.
    /// @param localRect The rectangle in local surface coordinates.
    /// @return The rectangle in screen coordinates.
    [[nodiscard]] auto localToScreen(Rectangle localRect) const noexcept -> Rectangle;
    /// Convert a rectangle from screen coordinates to local surface coordinates.
    /// @param screenRect The rectangle in screen coordinates.
    /// @return The rectangle in local surface coordinates.
    [[nodiscard]] auto screenToLocal(Rectangle screenRect) const noexcept -> Rectangle;
    /// Get this surface's visible rectangle in screen coordinates.
    /// @param screenRect The screen rectangle that limits the result.
    /// @return The part of this surface visible through all ancestors and inside `screenRect`.
    [[nodiscard]] auto visibleScreenRect(Rectangle screenRect) const noexcept -> Rectangle;
    /// Find the nearest opaque surface at or above this surface.
    /// @return The nearest opaque surface, or an empty pointer if this surface is not owned by a shared pointer.
    [[nodiscard]] auto nearestOpaqueSurface() noexcept -> SurfacePtr;
    /// Test if this surface is an ancestor of, or equal to, another surface.
    /// @param surface The surface to test.
    /// @return `true` if this surface is an ancestor of, or equal to, `surface`.
    [[nodiscard]] auto isAncestorOrSame(const SurfacePtr &surface) const noexcept -> bool;
    /// Get mutable access to the invalidation flags.
    /// @return The surface flags.
    [[nodiscard]] auto flags() noexcept -> SurfaceFlags &;
    /// Get read-only access to the invalidation flags.
    /// @return The surface flags.
    [[nodiscard]] auto flags() const noexcept -> const SurfaceFlags &;
    /// Get the assigned rectangle of this surface.
    /// @return The current surface rectangle.
    [[nodiscard]] auto rectangle() const noexcept -> const Rectangle & { return _rectangle; }
    /// Set the assigned rectangle of this surface.
    /// @param rect The new rectangle.
    void setRectangle(const Rectangle &rect) noexcept;
    /// Get read-only access to the configured layout metrics.
    /// @return The layout metrics for this surface.
    [[nodiscard]] auto layoutMetrics() const noexcept -> const LayoutMetrics &;
    /// Create an editor for the configured layout metrics.
    /// @return The editor that invalidates layout when metrics change.
    [[nodiscard]] auto editLayoutMetrics() noexcept -> LayoutMetricsEditor;
    /// Request direct keyboard focus for this surface from its containing page.
    /// @return `true` if the focus request was accepted.
    [[nodiscard]] auto requestFocus() noexcept -> bool;
    /// Access and create actions for this surface.
    /// A call of this method creates a new action container for this surface.
    /// Key presses are handled by the action container.
    [[nodiscard]] auto actions() noexcept -> Actions &;
    /// Access the existing action container without creating one.
    /// @return The existing actions, or a null pointer if no actions were created yet.
    [[nodiscard]] auto actionsPtr() const noexcept -> const ActionsPtr &;
    /// Access and create a scheduler for this surface.
    /// A call of this method creates a new scheduler for this surface.
    /// Scheduled actions are handled by the scheduler and callbacks run on the application UI thread.
    [[nodiscard]] auto scheduler() noexcept -> Scheduler &;
    /// Run a local layout pass for tests and compatibility.
    /// This does not recursively layout child surfaces; production layout is driven by `Display`.
    /// @param size The assigned size inside this surface.
    /// @param context The resolved layout context for this surface.
    void layout(Size size, const LayoutContext &context) noexcept;
    /// Get mutable access to theme attributes.
    /// @return The surface theme attributes.
    [[nodiscard]] auto themeAttributes() noexcept -> SurfaceThemeAttributes &;
    /// Get read-only access to theme attributes.
    /// @return The surface theme attributes.
    [[nodiscard]] auto themeAttributes() const noexcept -> const SurfaceThemeAttributes &;
    /// Create the effective theme context for this surface from a parent context.
    /// @param parentContext The inherited context.
    /// @return The effective context for this surface.
    [[nodiscard]] auto themeContextFrom(const ThemeContext &parentContext) const -> ThemeContext;

    /// Test if this surface is a page.
    /// @return `true` for `Page`, otherwise `false`.
    [[nodiscard]] virtual auto isPage() const noexcept -> bool;
    /// Test if this surface paints every cell in its rectangle.
    /// @return `true` if the surface is opaque.
    [[nodiscard]] virtual auto isOpaque() const noexcept -> bool;

public: // events
    /// Measure this surface for a proposed size.
    /// @param scope Measurement access for child surfaces.
    /// @param proposal The proposed size.
    /// @return The measured layout metrics.
    [[nodiscard]] virtual auto onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept -> LayoutMetrics;
    /// Layout direct children within this surface.
    /// @param scope Layout access for measuring and placing child surfaces.
    virtual void onLayout(LayoutScope &scope) noexcept;
    /// Paint this surface into the target buffer.
    /// @param buffer The target buffer.
    /// @param context The paint context for the current paint pass.
    virtual void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept;
    /// Handle a key press event.
    /// This is the low-level event handler for key press events.
    /// If a surface handles a key press event, it should mark it as handled.
    /// @param keyPressEvent The key press event to handle.
    virtual void onKeyPress(KeyPressEvent &keyPressEvent) noexcept;
    /// React to a focus relation change.
    /// @param focusChange The focus transition for this surface.
    virtual void onFocus(FocusChange focusChange) noexcept;

protected:
    /// Protected tag for safe construction of all surface classes.
    struct ProtectedTag {};
    /// Get mutable access to the real child surface storage.
    /// @return The unfiltered child surface storage.
    [[nodiscard]] auto childStorage() noexcept -> SurfaceContainer &;
    /// Get read-only access to the real child surface storage.
    /// @return The unfiltered child surface storage.
    [[nodiscard]] auto childStorage() const noexcept -> const SurfaceContainer &;
    /// Create the inherited theme context for a child surface.
    /// @param child The child surface.
    /// @param parentContext The theme context of this surface.
    /// @return The resolved theme context for the child.
    [[nodiscard]] auto childLayoutContext(const SurfacePtr &child, const LayoutContext &parentContext) const
        -> LayoutContext;

private:
    /// Get the existing scheduler without creating one.
    /// @return The existing scheduler.
    [[nodiscard]] auto schedulerPtr() const noexcept -> const SchedulerPtr &;
    /// Replace the configured layout metrics if they changed.
    /// @param layoutMetrics The new layout metrics.
    void setLayoutMetrics(LayoutMetrics layoutMetrics);
    /// Apply one focus relation transition.
    /// @param focusChange The transition to apply.
    void applyFocusChange(FocusChange focusChange) noexcept;
    /// Clear page focus if the current focused surface is this surface.
    void clearDirectFocusIfNeeded() noexcept;
    /// Clear page focus if the current focused surface is in this subtree.
    void clearSubtreeFocusIfNeeded() noexcept;
    /// Find the nearest page that contains this surface.
    /// @return The page, or null when this surface is detached.
    [[nodiscard]] auto nearestPage() noexcept -> PagePtr;

protected:
    SurfaceContainer _surfaces{*this};       ///< All child surfaces.
    SurfaceFlags _flags{*this};              ///< Framework-managed surface state flags.
    Rectangle _rectangle;                    ///< The current rectangle of the surface.
    LayoutMetrics _layoutMetrics;            ///< The configured layout metrics of the surface.
    SurfaceThemeAttributes _themeAttributes; ///< Theme attributes of this surface.

private:
    SurfaceWeakPtr _parent;  ///< The parent surface, or empty for top-level pages.
    ActionsPtr _actions;     ///< The action container.
    SchedulerPtr _scheduler; ///< The scheduler.
};

}
