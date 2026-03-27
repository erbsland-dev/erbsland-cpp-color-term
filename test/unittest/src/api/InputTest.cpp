// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <chrono>


class InputProbe final : public Input {
public:
    [[nodiscard]] auto mode() const noexcept -> Mode override { return _mode; }
    void setMode(const Mode mode) override { _mode = mode; }
    [[nodiscard]] auto readLine() -> std::string override { return _line; }

protected:
    [[nodiscard]] auto readKeyImpl(const std::chrono::milliseconds timeout) const -> Key override {
        _lastTimeout = timeout;
        _readKeyCallCount += 1;
        return _readResult;
    }

    [[nodiscard]] auto waitForKeyImpl() const -> Key override {
        _waitForKeyCallCount += 1;
        return _waitResult;
    }

public:
    Mode _mode{Mode::ReadLine};
    std::string _line{"typed line"};
    mutable std::chrono::milliseconds _lastTimeout{-1};
    mutable int _readKeyCallCount{0};
    mutable int _waitForKeyCallCount{0};
    Key _readResult{Key::Escape};
    Key _waitResult{Key::Enter};
};


TESTED_TARGETS(Input)
class InputTest final : public el::UnitTest {
public:
    void testModeAndReadLineUseTheImplementationHooks() {
        auto input = InputProbe{};

        REQUIRE_EQUAL(input.mode(), Input::Mode::ReadLine);
        REQUIRE_EQUAL(input.readLine(), std::string{"typed line"});

        input.setMode(Input::Mode::Key);

        REQUIRE_EQUAL(input.mode(), Input::Mode::Key);
    }

    void testReadKeyClampsNegativeTimeoutsToZero() {
        const auto input = InputProbe{};

        const auto key = input.readKey(std::chrono::milliseconds{-25});

        REQUIRE_EQUAL(key, Key::Escape);
        REQUIRE_EQUAL(input._lastTimeout, std::chrono::milliseconds{0});
        REQUIRE_EQUAL(input._readKeyCallCount, 1);
    }

    void testReadKeyPassesThroughPositiveTimeouts() {
        const auto input = InputProbe{};

        const auto key = input.readKey(std::chrono::milliseconds{125});

        REQUIRE_EQUAL(key, Key::Escape);
        REQUIRE_EQUAL(input._lastTimeout, std::chrono::milliseconds{125});
        REQUIRE_EQUAL(input._readKeyCallCount, 1);
    }

    void testWaitForKeyUsesTheBlockingHook() {
        const auto input = InputProbe{};

        const auto key = input.waitForKey();

        REQUIRE_EQUAL(key, Key::Enter);
        REQUIRE_EQUAL(input._waitForKeyCallCount, 1);
        REQUIRE_EQUAL(input._readKeyCallCount, 0);
    }
};
