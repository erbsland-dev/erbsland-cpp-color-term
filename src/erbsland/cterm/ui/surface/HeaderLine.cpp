// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "HeaderLine.hpp"

namespace erbsland::cterm::ui::surface {

HeaderLine::HeaderLine(ProtectedTag protectedTag) noexcept : TextLine{protectedTag} {
    themeAttributes().setElement(theme::Element::HeaderLine);
}

auto HeaderLine::create() noexcept -> HeaderLinePtr {
    return std::make_shared<HeaderLine>(ProtectedTag{});
}

}
