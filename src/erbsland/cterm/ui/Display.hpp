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
    using PageStack = std::vector<PagePtr>;

public:
    /// Create a new display instance.
    /// @param terminal The terminal to use.
    /// @param mainPage The main page to show initially.
    explicit Display(TerminalPtr terminal, PagePtr mainPage);

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

private:
    /// Test if any currently visible page needs a render pass.
    /// @return `true` if a visible page needs layout or paint.
    [[nodiscard]] auto hasVisibleDirtyPage() const noexcept -> bool;
    /// Calculate the layout of all displayed pages.
    void calculateLayout();

    /// Render the current page to the terminal.
    void render();

private:
    TerminalPtr _terminal;               ///< The terminal to display on.
    Buffer _displayBuffer;               ///< The buffer to render to the terminal.
    Size _terminalSize;                  ///< The last terminal size.
    EventTime _nextTerminalSizePollTime; ///< The earliest next time the terminal size is tested.
    PageStack _pageStack;                ///< The stack with all pages.
    bool _hasPendingResize{true};        ///< The terminal size has changed.
    EventTime _nextRenderTime;           ///< The earliest next time a render is triggered.
};

}
