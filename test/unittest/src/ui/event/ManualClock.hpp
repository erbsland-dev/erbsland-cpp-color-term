// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/ColorTermIncludes.hpp"

#include <erbsland/cterm/ui/all.hpp>

class ManualClock final {
public:
    [[nodiscard]] auto now() const noexcept -> ui::EventTime { return _now; }

    void advance(const ui::EventClock::duration duration) noexcept { _now += duration; }

private:
    ui::EventTime _now{};
};
