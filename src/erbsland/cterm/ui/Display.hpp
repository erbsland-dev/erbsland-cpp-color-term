// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Page.hpp"

#include "event/Event.hpp"

#include "../Buffer.hpp"
#include "../Terminal.hpp"

#include <memory>
#include <vector>

namespace erbsland::cterm::ui {

class Display;
using DisplayPtr = std::unique_ptr<Display>;

/// A display is a way to show pages on a terminal.
class Display {
    constexpr static auto cTerminalSizePollInterval = std::chrono::milliseconds{100};
    constexpr static auto cMinimumRenderInterval = std::chrono::milliseconds{50};
    constexpr static auto cMaximumPages = std::size_t{20};
    constexpr static auto cMinimumDisplaySize = Size{40, 20};
    constexpr static auto cHardMinimumDisplaySize = Size{32, 12};
    using PageStack = std::vector<PagePtr>;

    struct PaintRequest final {
        SurfacePtr baseSurface; ///< The opaque base surface where repainting starts.
        Rectangle screenRect;   ///< The damaged screen rectangle.
    };

    using PaintRequestList = std::vector<PaintRequest>;

public:
    /// Display size limits used to protect interactive applications from unusably small terminals.
    struct SizeLimits final {
        Size minimumDisplaySize{cMinimumDisplaySize};         ///< The size where crop marks start.
        Size hardMinimumDisplaySize{cHardMinimumDisplaySize}; ///< The size where only the warning is rendered.
    };

public:
    /// Create a new display instance.
    /// @param terminal The terminal to use.
    /// @param mainPage The main page to show initially.
    explicit Display(TerminalPtr terminal, PagePtr mainPage);
    /// Create a new display instance with explicit size limits.
    /// @param terminal The terminal to use.
    /// @param mainPage The main page to show initially.
    /// @param sizeLimits The size limits for this display.
    explicit Display(TerminalPtr terminal, PagePtr mainPage, SizeLimits sizeLimits);
    /// Create a new display instance with an explicit theme scope.
    /// @param terminal The terminal to use.
    /// @param mainPage The main page to show initially.
    /// @param theme The application theme.
    explicit Display(TerminalPtr terminal, PagePtr mainPage, theme::ThemeConstPtr theme);
    /// Create a new display instance with an explicit theme scope and size limits.
    /// @param terminal The terminal to use.
    /// @param mainPage The main page to show initially.
    /// @param theme The application theme.
    /// @param sizeLimits The size limits for this display.
    explicit Display(TerminalPtr terminal, PagePtr mainPage, theme::ThemeConstPtr theme, SizeLimits sizeLimits);

    // defaults
    ~Display() = default;
    Display(const Display &) = delete;
    Display(Display &&) = delete;
    auto operator=(const Display &) -> Display & = delete;
    auto operator=(Display &&) -> Display & = delete;

public:
    /// Push a new page to the page stack.
    /// @throws std::logic_error If you push more than 20 pages or a duplicate page to the stack.
    void pushPage(PagePtr page);
    /// Pop the top page from the page stack.
    /// @throws std::logic_error If you try to pop the main page from the stack.
    void popPage();
    /// Poll for terminal size changes.
    void pollTerminalResize();
    /// Poll for render work.
    /// This triggers a render for visible dirty pages as soon as the minimum render interval allows it.
    void pollRender();
    /// Get the earliest next wake-up time required by the display.
    /// @return The earliest wake-up time.
    [[nodiscard]] auto nextWakeTime() const noexcept -> EventTime;
    /// Forward a key press to the active page.
    /// @param keyPressEvent The key press event to forward.
    void onKeyPress(KeyPressEvent &keyPressEvent) noexcept;
    /// Replace the display theme.
    /// @param theme The new theme. A null pointer restores the default dark theme.
    void setTheme(const theme::ThemeConstPtr &theme);
    /// Set the minimum display size for the application.
    /// If the terminal size is smaller, the display is cropped and crop marks indicate to increase the
    /// terminal size.
    void setMinimumDisplaySize(Size minimumDisplaySize);
    /// Set the hard minimum display size for the application.
    /// If the terminal size is smaller than this hard limit, no UI is displayed - just a warning.
    void setHardMinimumDisplaySize(Size hardMinimumDisplaySize);

private:
    /// Test if any currently visible page needs a render pass.
    /// @return `true` if a visible page needs layout or paint.
    [[nodiscard]] auto hasVisibleDirtyPage() const noexcept -> bool;
    /// Calculate the layout of all displayed pages.
    void calculateLayout();
    /// Measure one surface with the given proposal.
    [[nodiscard]] auto
    measureSurface(const SurfacePtr &surface, const LayoutProposal &proposal, const LayoutContext &context) noexcept
        -> LayoutMetrics;
    /// Layout a visible surface and then continue through its visible children.
    void layoutSurfaceSubtree(const SurfacePtr &surface, const LayoutContext &context) noexcept;

    /// Render the current page to the terminal.
    void render();
    /// Render all visible pages from the nearest opaque base page upward.
    void renderVisiblePageStack();
    /// Collect visible paint requests from a page.
    void collectPaintRequests(const SurfacePtr &surface, Rectangle parentVisibleRect, PaintRequestList &requests) const;
    /// Add a paint request while coalescing covered requests.
    void addPaintRequest(PaintRequestList &requests, PaintRequest request) const;
    /// Paint one request into the display buffer.
    void paintRequest(const PaintRequest &request);
    /// Paint a surface and all intersecting child surfaces.
    void paintSurfaceSubtree(const SurfacePtr &surface, Rectangle damagedScreenRect, Rectangle visibleScreenRect);
    /// Paint later siblings of a surface on the path back to the page.
    void paintLaterSiblings(const SurfacePtr &surface, Rectangle damagedScreenRect);
    /// Paint one surface into the display buffer.
    void paintSurface(const SurfacePtr &surface, Rectangle damagedScreenRect, Rectangle visibleScreenRect);
    /// Test if a visible surface in a tree requires layout or paint work.
    [[nodiscard]] auto hasVisibleDirtySurface(const SurfacePtr &surface, Rectangle parentVisibleRect) const noexcept
        -> bool;
    /// Create the resolved theme context for one surface.
    /// @param surface The surface to resolve.
    /// @return The resolved theme context.
    [[nodiscard]] auto themeContextForSurface(const SurfacePtr &surface) const -> ThemeContext;
    /// Refresh the effective terminal size after terminal or display-size limit changes.
    void refreshTerminalSize();

private:
    TerminalPtr _terminal;               ///< The terminal to display on.
    UpdateSettings _updateSettings;      ///< The update settings.
    Buffer _displayBuffer;               ///< The buffer to render to the terminal.
    Size _terminalSize;                  ///< The last terminal size.
    Size _minimumDisplaySize;            ///< The minimum size for the UI display (starts cropping)
    Size _hardMinimumDisplaySize;        ///< The hard minimum size for the UI display (no display, warning)
    EventTime _nextTerminalSizePollTime; ///< The earliest next time the terminal size is tested.
    PageStack _pageStack;                ///< The stack with all pages.
    bool _hasPendingResize{true};        ///< The terminal size has changed.
    EventTime _nextRenderTime;           ///< The earliest next time a render is triggered.
    theme::ThemeConstPtr _theme;         ///< The display theme.
};

}
