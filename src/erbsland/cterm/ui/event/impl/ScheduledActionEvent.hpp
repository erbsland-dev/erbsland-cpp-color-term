// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../EventData.hpp"
#include "../ScheduledActionRef.hpp"

#include "../../Surface.hpp"

#include <cstdint>


namespace erbsland::cterm::ui::impl {

/// Internal event payload for scheduled surface actions.
class ScheduledActionEvent final : public EventData {
public:
    /// Create a scheduled action event.
    /// @param surface The target surface.
    /// @param actionRef The action reference.
    /// @param generation The generation of the scheduled action.
    ScheduledActionEvent(SurfaceWeakPtr surface, ScheduledActionRef actionRef, uint64_t generation) :
        _surface{std::move(surface)}, _actionRef{actionRef}, _generation{generation} {
    }

public:
    /// Get the target surface.
    /// @return The target surface.
    [[nodiscard]] auto surface() const noexcept -> const SurfaceWeakPtr & { return _surface; }
    /// Get the scheduled action reference.
    /// @return The action reference.
    [[nodiscard]] auto actionRef() const noexcept -> ScheduledActionRef { return _actionRef; }
    /// Get the action generation.
    /// @return The action generation.
    [[nodiscard]] auto generation() const noexcept -> uint64_t { return _generation; }

private:
    SurfaceWeakPtr _surface;             ///< The target surface.
    ScheduledActionRef _actionRef;       ///< The action reference.
    uint64_t _generation = 0;            ///< The generation of the scheduled action.
};

}
