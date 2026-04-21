// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/ColorTermIncludes.hpp"

#include <memory>
#include <string>
#include <vector>

class RecordingSurface final : public ui::Surface {
public:
    explicit RecordingSurface(
        std::shared_ptr<std::vector<std::string>> callLog, std::string name, bool handleKeyPress, ProtectedTag) :
        _callLog(std::move(callLog)), _name(std::move(name)), _handleKeyPress(handleKeyPress) {
        flags().setFocusable(true);
    }

    [[nodiscard]] static auto
    create(const std::shared_ptr<std::vector<std::string>> &callLog, std::string name, bool handleKeyPress = false)
        -> std::shared_ptr<RecordingSurface> {
        return std::make_shared<RecordingSurface>(callLog, std::move(name), handleKeyPress, ProtectedTag{});
    }

public:
    void onKeyPress(ui::KeyPressEvent &keyPressEvent) noexcept override {
        _callLog->push_back(_name);
        Surface::onKeyPress(keyPressEvent);
        if (!keyPressEvent.isHandled() && _handleKeyPress) {
            keyPressEvent.setHandled();
        }
    }

private:
    std::shared_ptr<std::vector<std::string>> _callLog;
    std::string _name;
    bool _handleKeyPress{false};
};
