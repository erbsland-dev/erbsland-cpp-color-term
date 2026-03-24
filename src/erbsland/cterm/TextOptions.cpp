// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TextOptions.hpp"


namespace erbsland::cterm {

auto TextOptions::defaultOptions() noexcept -> const TextOptions & {
    static const auto cDefaultOptions = TextOptions{};
    return cDefaultOptions;
}

}
