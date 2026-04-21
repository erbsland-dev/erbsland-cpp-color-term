// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ReadableBuffer.hpp"

#include "Bitmap.hpp"

namespace erbsland::cterm {

auto ReadableBuffer::countDifferencesTo(const ReadableBuffer &other) const noexcept -> std::size_t {
    const auto thisRect = rect();
    const auto otherRect = other.rect();
    const auto overlap = thisRect & otherRect;
    auto differences = thisRect.size().area() + otherRect.size().area() - 2 * overlap.size().area();
    overlap.forEach([&](const Position pos) -> void {
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
    const auto sourceRect = rect();
    auto bitmap = Bitmap{sourceRect.size()};
    if (characters.empty()) {
        return bitmap;
    }
    sourceRect.forEach([&](const Position pos) -> void {
        auto isSet = get(pos).isOneOf(characters);
        if (invert) {
            isSet = !isSet;
        }
        bitmap.setPixel(pos - sourceRect.topLeft(), isSet);
    });
    return bitmap;
}

}
