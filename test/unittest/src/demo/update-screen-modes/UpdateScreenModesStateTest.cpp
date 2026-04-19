// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "update-screen-modes/src/FlushSpeedTracker.hpp"
#include "update-screen-modes/src/UpdateScreenModesState.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <chrono>

using namespace std::chrono_literals;

TESTED_TARGETS(UpdateScreenModesState)
class UpdateScreenModesStateTest final : public el::UnitTest {
public:
    void testDefaultsEnableOverwriteLineAndBackBuffer() {
        auto state = demo::updatescreenmodes::UpdateScreenModesState{};

        REQUIRE(state.overwriteModeEnabled());
        REQUIRE(state.lineBufferEnabled());
        REQUIRE(state.backBufferEnabled());
        REQUIRE(state.backBufferActive());
    }

    void testBackBufferIsOnlyActiveWhenOverwriteModeIsEnabled() {
        auto state = demo::updatescreenmodes::UpdateScreenModesState{};

        REQUIRE(state.backBufferActive());

        state.toggleOverwriteMode();

        REQUIRE_FALSE(state.overwriteModeEnabled());
        REQUIRE(state.backBufferEnabled());
        REQUIRE_FALSE(state.backBufferActive());
    }

    void testToggleFlags() {
        auto state = demo::updatescreenmodes::UpdateScreenModesState{};

        REQUIRE(state.overwriteModeEnabled());
        REQUIRE(state.lineBufferEnabled());
        REQUIRE(state.backBufferEnabled());

        state.toggleOverwriteMode();
        state.toggleLineBuffer();
        state.toggleBackBuffer();

        REQUIRE_FALSE(state.overwriteModeEnabled());
        REQUIRE_FALSE(state.lineBufferEnabled());
        REQUIRE_FALSE(state.backBufferEnabled());
        REQUIRE_FALSE(state.backBufferActive());
    }

    void testFlushSpeedTrackerCalculatesTheAverage() {
        auto tracker = demo::updatescreenmodes::FlushSpeedTracker{};

        REQUIRE_EQUAL(tracker.sampleCount(), std::size_t{0});
        REQUIRE_EQUAL(tracker.lastDuration(), std::chrono::nanoseconds{0});
        REQUIRE_EQUAL(tracker.averageDuration(), std::chrono::nanoseconds{0});

        tracker.addSample(1200us);
        tracker.addSample(800us);

        REQUIRE_EQUAL(tracker.sampleCount(), std::size_t{2});
        REQUIRE_EQUAL(tracker.lastDuration(), 800us);
        REQUIRE_EQUAL(tracker.averageDuration(), 1ms);
        REQUIRE(tracker.lastMilliseconds() > 0.799);
        REQUIRE(tracker.lastMilliseconds() < 0.801);
        REQUIRE(tracker.averageMilliseconds() > 0.999);
        REQUIRE(tracker.averageMilliseconds() < 1.001);
    }
};
