// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Input.hpp"


namespace erbsland::cterm {


class InputPosix final : public Input {
public:
    InputPosix() = default;
    ~InputPosix() override;

protected:
    void handleModeChange(Mode previousMode, Mode newMode) noexcept override;
    [[nodiscard]] auto readKey(std::chrono::milliseconds timeout) const -> Key override;

private:
    bool _keyInputSessionActive{false};
};


}
