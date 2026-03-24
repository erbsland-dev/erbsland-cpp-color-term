// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ReadableBuffer.hpp"


#include "Bitmap.hpp"


namespace erbsland::cterm {

auto ReadableBuffer::countDifferencesTo(const ReadableBuffer &other) const noexcept -> std::size_t {
    auto minSize = size().componentMin(other.size());
    auto differences = std::abs(size().area() - other.size().area());
    minSize.forEach([&](const Position pos) -> void {
        if (get(pos) != other.get(pos)) {
            differences += 1;
        }
    });
    return static_cast<std::size_t>(differences);
}

auto ReadableBuffer::toMask(const std::u32string &characters, const bool invert) -> Bitmap {
    return toMaskImpl(characters, invert);
}

auto ReadableBuffer::toMask(std::initializer_list<char32_t> characters, const bool invert) -> Bitmap {
    return toMaskImpl(std::u32string{characters}, invert);
}

auto ReadableBuffer::toMaskImpl(const std::u32string &characters, const bool invert) -> Bitmap {
    auto bitmap = Bitmap{size()};
    if (characters.empty()) {
        return bitmap;
    }
    size().forEach([&](const Position pos) -> void {
        auto isSet = get(pos).isOneOf(characters);
        if (invert) {
            isSet = !isSet;
        }
        bitmap.setPixel(pos, isSet);
    });
    return bitmap;
}

}
