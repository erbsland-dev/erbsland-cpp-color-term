// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cctype>
#include <string>
#include <string_view>


namespace erbsland::cterm::impl {


[[nodiscard]] inline auto toNormalizedIdentifier(const std::string_view text) -> std::string {
    std::string result;
    result.reserve(text.size());
    bool seenContent = false;
    bool pendingSpace = false;
    for (char ch : text) {
        const auto unsignedCharacter = static_cast<unsigned char>(ch);
        if (unsignedCharacter < 128U) {
            ch = static_cast<char>(std::tolower(unsignedCharacter));
        }
        if (ch == ' ' || ch == '\t') {
            if (seenContent) {
                pendingSpace = true;
            }
            continue;
        }
        if (pendingSpace && !result.empty()) {
            result += '_';
            pendingSpace = false;
        }
        result += ch;
        seenContent = true;
    }
    return result;
}


}
