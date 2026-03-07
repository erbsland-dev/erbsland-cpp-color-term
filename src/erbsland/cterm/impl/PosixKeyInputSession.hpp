// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


namespace erbsland::cterm {


class PosixKeyInputSession final {
public:
    PosixKeyInputSession() = delete;
    ~PosixKeyInputSession() = delete;

public:
    [[nodiscard]] static auto acquire() noexcept -> bool;
    static void release() noexcept;
};


}
