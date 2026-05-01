// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "HeaderLine.hpp"

namespace erbsland::cterm::ui::surface {

HeaderLine::HeaderLine(ProtectedTag protectedTag) noexcept : DynamicTextLine{protectedTag} {
}

auto HeaderLine::create() -> HeaderLinePtr {
    auto result = std::make_shared<HeaderLine>(ProtectedTag{});
    result->initializeUi();
    return result;
}

void HeaderLine::initializeUi() {
    DynamicTextLine::initializeUi();
    themeAttributes().setElement(theme::Element::HeaderLine);
    dynamicText(Section::Left)->themeAttributes().setElement(theme::Element::HeaderLine);
    dynamicText(Section::Middle)->themeAttributes().setElement(theme::Element::HeaderLine);
    dynamicText(Section::Right)->themeAttributes().setElement(theme::Element::HeaderLine);
}

}
