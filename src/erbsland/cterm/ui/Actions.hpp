// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Action.hpp"

#include "event/KeyPressEvent.hpp"

#include <vector>

namespace erbsland::cterm::ui {

class Actions;
using ActionsPtr = std::unique_ptr<Actions>;

/// Ordered action container attached to a surface or page.
class Actions final {
public:
    using Container = std::vector<ActionPtr>;
    using iterator = Container::iterator;
    using const_iterator = Container::const_iterator;

public:
    /// Create an action container for a surface.
    /// @param owner The surface that owns this container.
    explicit Actions(SurfaceWeakPtr owner) noexcept;
    ~Actions() = default;

    // defaults
    Actions(const Actions &) = delete;
    Actions(Actions &&) = delete;
    auto operator=(const Actions &) -> Actions & = delete;
    auto operator=(Actions &&) -> Actions & = delete;

public:
    /// Add an action to this container.
    /// @param action The action to attach.
    /// @throws std::invalid_argument If the action is null or already attached here.
    void add(ActionPtr action);
    /// Remove an action from this container.
    /// @param action The action to remove.
    void remove(const ActionPtr &action) noexcept;
    /// Remove all actions.
    void clear() noexcept;
    /// Test if this container has no actions.
    [[nodiscard]] auto empty() const noexcept -> bool { return _actions.empty(); }
    /// Get the number of actions.
    [[nodiscard]] auto size() const noexcept -> std::size_t { return _actions.size(); }
    /// Test if an action is already attached to this container.
    [[nodiscard]] auto contains(const ActionPtr &action) const noexcept -> bool;
    /// Handle a key press event using the first enabled matching action.
    /// @param keyPressEvent The key press event to handle.
    void onKeyPress(KeyPressEvent &keyPressEvent);
    /// Refresh repaint-driven action enablement.
    void refreshForRepaint();

public: // iterators
    [[nodiscard]] auto begin() noexcept -> iterator { return _actions.begin(); }
    [[nodiscard]] auto end() noexcept -> iterator { return _actions.end(); }
    [[nodiscard]] auto begin() const noexcept -> const_iterator { return _actions.begin(); }
    [[nodiscard]] auto end() const noexcept -> const_iterator { return _actions.end(); }

private:
    SurfaceWeakPtr _owner; ///< The owning surface.
    Container _actions;    ///< The actions in dispatch order.
};

}
