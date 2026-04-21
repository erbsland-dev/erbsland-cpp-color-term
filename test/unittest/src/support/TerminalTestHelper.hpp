// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "BufferTestHelper.hpp"
#include "TerminalTestBackend.hpp"

#include <memory>
#include <string_view>

class TerminalTestHelper : public BufferTestHelper {
public:
    auto createTerminal(const std::shared_ptr<TerminalTestBackend> &backend, const Size size = Size{80, 25})
        -> std::unique_ptr<Terminal> {
        auto terminal = std::make_unique<Terminal>(size);
        terminal->setBackend(backend);
        return terminal;
    }

    auto createMinimumSizeWarningSettings(const Size minimumSize, const std::string_view message) -> UpdateSettings {
        auto settings = UpdateSettings{};
        settings.setMinimumSize(minimumSize);
        settings.setMinimumSizeBackground(Char{U'.'});
        settings.setMinimumSizeMessage(String{message});
        settings.setSwitchToAlternateBuffer(false);
        return settings;
    }

    [[nodiscard]] static auto exactTerminalSizeLimits() noexcept -> ui::Display::SizeLimits {
        return ui::Display::SizeLimits{.minimumDisplaySize = {}, .hardMinimumDisplaySize = {}};
    }

    static void useExactTerminalSize(ui::Display &display) {
        display.setHardMinimumDisplaySize({});
        display.setMinimumDisplaySize({});
    }
};
