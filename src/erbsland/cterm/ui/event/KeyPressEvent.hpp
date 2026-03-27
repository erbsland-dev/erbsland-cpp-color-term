// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "EventData.hpp"

#include "../../Key.hpp"

#include <memory>


namespace erbsland::cterm::ui {

class KeyPressEvent;
using KeyPressEventPtr = std::shared_ptr<KeyPressEvent>;

/// A key press event.
class KeyPressEvent : public EventData {
public:
    /// Create a key press event for the given key.
    /// @param key The pressed key.
    explicit KeyPressEvent(const Key &key) : _key{key} {}

public:
    /// Get the pressed key.
    [[nodiscard]] auto key() const noexcept -> const Key & { return _key; }

private:
    Key _key;
};

}
