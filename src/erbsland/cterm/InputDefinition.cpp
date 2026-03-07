// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "InputDefinition.hpp"


namespace erbsland::cterm {


InputDefinition::InputDefinition(const Key keyPress, const ForMode forMode) noexcept :
    _keyPress{keyPress}, _forMode{forMode} {
}

auto InputDefinition::fromString(std::string text) noexcept -> InputDefinition {
    auto forMode = ForMode::Both;
    if (text.starts_with(">")) {
        text.erase(0, 1);
        forMode = ForMode::ReadLine;
    } else if (text.starts_with("+")) {
        text.erase(0, 1);
        forMode = ForMode::Key;
    }
    return {Key::fromString(std::move(text)), forMode};
}

auto InputDefinition::toString() const -> std::string {
    auto text = _keyPress.toString();
    if (text.empty()) {
        return {};
    }
    switch (_forMode) {
    case ForMode::ReadLine:
        return ">" + text;
    case ForMode::Key:
        return "+" + text;
    case ForMode::Both:
    default:
        return text;
    }
}

auto InputDefinition::toDisplayText(const bool useBrackets) const -> std::string {
    return _keyPress.toDisplayText(useBrackets);
}


}
