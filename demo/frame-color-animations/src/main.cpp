// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "Main.hpp"

#include "FrameColorAnimationsApp.hpp"

auto main(const int argc, char **argv) -> int {
    return demo::Main<demo::framecoloranimations::FrameColorAnimationsApp>{}.run(argc, argv);
}
