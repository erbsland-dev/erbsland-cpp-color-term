// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Color.hpp"


namespace erbsland::cterm {


auto Color::overlayWith(const Color &overlay) const -> Color {
    auto result = *this;
    if (overlay.fg() != Foreground::Inherited) {
        result.setFg(overlay.fg());
    }
    if (overlay.bg() != Background::Inherited) {
        result.setBg(overlay.bg());
    }
    return result;
}


auto Color::fromString(const std::string_view str) -> Color {
    if (const auto splitPos = str.find(':'); splitPos != std::string_view::npos) {
        const auto fgStr = str.substr(0, splitPos);
        const auto bgStr = str.substr(splitPos + 1);
        return {Foreground::fromString(fgStr), Background::fromString(bgStr)};
    }
    return {Foreground::fromString(str)};
}


}
