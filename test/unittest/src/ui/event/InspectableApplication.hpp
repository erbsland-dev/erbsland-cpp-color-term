// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/ColorTermIncludes.hpp"

#include <chrono>
#include <memory>

class InspectableApplication final : public ui::Application {
public:
    using milliseconds = std::chrono::milliseconds;

public:
    explicit InspectableApplication(erbsland::cterm::TerminalPtr terminal) : ui::Application(std::move(terminal)) {}

    [[nodiscard]] auto managedThreadCountForTest() const -> std::size_t { return managedEventThreadCount(); }

    void setManagedThreadShutdownTimeoutForTest(const milliseconds timeout) { _managedThreadShutdownTimeout = timeout; }

protected:
    [[nodiscard]] auto managedEventThreadShutdownTimeout() const -> milliseconds override {
        return _managedThreadShutdownTimeout;
    }

private:
    milliseconds _managedThreadShutdownTimeout{std::chrono::milliseconds{1500}};
};
