// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "PosixKeyInputSession.hpp"


#include "PosixKeyInputSessionStorage.hpp"


namespace erbsland::cterm {


auto PosixKeyInputSession::acquire() noexcept -> bool {
    return PosixKeyInputSessionStorage::instance().acquire();
}


void PosixKeyInputSession::release() noexcept {
    PosixKeyInputSessionStorage::instance().release();
}


}
