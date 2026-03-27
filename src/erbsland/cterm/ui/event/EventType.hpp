// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>


namespace erbsland::cterm::ui {

/// The event type
enum class EventType : uint8_t {
    NoEvent,    ///< Special flag for error handling.
    Quit,       ///< A quit event. Stops a running event loop.
    KeyPress,        ///< A key was pressed.
    Resize,          ///< The terminal was resized.
    ScheduledAction, ///< A scheduled action event.
    Invocation,      ///< A function was invoked.
};

}
