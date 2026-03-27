// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "Main.hpp"

#include "UpdateScreenModesApp.hpp"

auto main(const int argc, char **argv) -> int {
    return demo::Main<demo::updatescreenmodes::UpdateScreenModesApp>{}.run(argc, argv);
}
