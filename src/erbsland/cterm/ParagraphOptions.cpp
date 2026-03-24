// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ParagraphOptions.hpp"

namespace erbsland::cterm {

auto ParagraphOptions::defaultOptions() noexcept -> const ParagraphOptions & {
    static const auto cDefaultOptions = ParagraphOptions{};
    return cDefaultOptions;
}

}
