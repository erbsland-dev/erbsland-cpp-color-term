// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "AbstractHelpSection.hpp"

namespace erbsland::cterm::ui::surface {

AbstractHelpSection::AbstractHelpSection(ProtectedTag) noexcept :
    Surface{LayoutMetrics{Size{}, Size::maximum(), Size{}, SizePolicy{SizePolicy::Grow}}} {
}

void AbstractHelpSection::scrollUp([[maybe_unused]] const Coordinate count) noexcept {
}

void AbstractHelpSection::scrollDown([[maybe_unused]] const Coordinate count) noexcept {
}

void AbstractHelpSection::pageUp() noexcept {
}

void AbstractHelpSection::pageDown() noexcept {
}

void AbstractHelpSection::scrollToTop() noexcept {
}

void AbstractHelpSection::scrollToBottom() noexcept {
}

}
