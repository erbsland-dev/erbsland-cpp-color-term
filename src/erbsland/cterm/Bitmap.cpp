// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Bitmap.hpp"


namespace erbsland::cterm {


auto Bitmap::pixel(const Position pos) const noexcept -> bool {
    if (!_size.contains(pos)) {
        return false;
    }
    return _data[toDataIndex(_size.index(pos))];
}

auto Bitmap::pixelQuad(const Position pos) const noexcept -> uint8_t {
    const auto base = Position{pos.x() * 2, pos.y() * 2};
    constexpr auto positions = std::array<std::pair<Position, uint8_t>, 4U>{
        std::pair{Position{0, 0}, static_cast<uint8_t>(0b0001U)},
        std::pair{Position{1, 0}, static_cast<uint8_t>(0b0010U)},
        std::pair{Position{0, 1}, static_cast<uint8_t>(0b0100U)},
        std::pair{Position{1, 1}, static_cast<uint8_t>(0b1000U)},
    };
    uint8_t result = 0;
    for (const auto &[delta, mask] : positions) {
        if (pixel(base + delta)) {
            result |= mask;
        }
    }
    return result;
}

void Bitmap::setPixel(const Position pos, const bool value) noexcept {
    if (!_size.contains(pos)) {
        return;
    }
    _data[toDataIndex(_size.index(pos))] = value;
}

void Bitmap::flipHorizontal() noexcept {
    for (auto y = 0; y < _size.height(); ++y) {
        for (auto x = 0; x < _size.width() / 2; ++x) {
            const auto p1 = Position{x, y};
            const auto p2 = Position{_size.width() - 1 - x, y};
            if (p1 != p2) {
                const auto pixel1 = static_cast<bool>(pixelRef(p1));
                const auto pixel2 = static_cast<bool>(pixelRef(p2));
                pixelRef(p1) = pixel2;
                pixelRef(p2) = pixel1;
            }
        }
    }
}

}
