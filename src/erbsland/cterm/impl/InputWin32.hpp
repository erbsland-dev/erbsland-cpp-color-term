// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Input.hpp"


namespace erbsland::cterm {


class InputWin32 final : public Input {
protected:
    [[nodiscard]] auto readKey(std::chrono::milliseconds timeout) const -> Key override;
};


}
