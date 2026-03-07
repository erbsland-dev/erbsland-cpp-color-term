// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Position.hpp"
#include "Size.hpp"

#include <array>
#include <bit>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>


namespace erbsland::cterm {


/// A mutable bitmap storing boolean pixels in row-major order.
class Bitmap {
protected:
    using Data = std::vector<bool>;

public: // ctors/dtor/assign/move
    /// Create an empty bitmap with the size `(0,0)`.
    Bitmap() = default;
    /// Create a bitmap with the given size and all pixels cleared.
    /// @param size The bitmap dimensions.
    explicit Bitmap(const Size size) : _size{size}, _data(static_cast<Data::size_type>(size.area()), false) {}
    /// Destroy the bitmap.
    ~Bitmap() = default;

public: // accessors
    /// Get the bitmap dimensions.
    [[nodiscard]] auto size() const noexcept -> Size { return _size; }
    /// Access the raw pixel storage.
    [[nodiscard]] auto data() const noexcept -> const std::vector<bool> & { return _data; }
    /// Access the raw pixel storage for modification.
    [[nodiscard]] auto data() noexcept -> std::vector<bool> & { return _data; }
    /// Convert a signed pixel index to the storage index type.
    /// @param index The row-major pixel index.
    /// @return The corresponding storage index.
    [[nodiscard]] static auto toDataIndex(const int index) noexcept -> Data::size_type {
        return static_cast<Data::size_type>(index);
    }
    /// Read one pixel.
    /// @param pos The pixel position.
    /// @return The pixel state, or `false` if `pos` is outside the bitmap.
    [[nodiscard]] auto pixel(Position pos) const noexcept -> bool;
    /// Read a 2x2 pixel block encoded as four bits.
    /// @param pos The quad position in half-resolution coordinates.
    /// @return Bit mask with top-left/top-right/bottom-left/bottom-right pixels in bits `0..3`.
    [[nodiscard]] auto pixelQuad(Position pos) const noexcept -> uint8_t;

public: // modifiers
    /// Set one pixel in the bitmap.
    /// @param pos The pixel position.
    /// @param value The new pixel state.
    /// @note Positions outside the bitmap are ignored.
    void setPixel(Position pos, bool value) noexcept;
    /// Mirror the bitmap horizontally in-place.
    void flipHorizontal() noexcept;

public: // tools
    /// Draw pixels from a numeric bit mask.
    /// @tparam T The unsigned integer type.
    /// @param position The top left corner where to draw the bit mask.
    /// @param data The array with the data.
    template <typename T>
        requires std::is_integral_v<T> && std::is_unsigned_v<T>
    void draw(Position position, const std::vector<T> &data) noexcept;
    /// Draw another bitmap to the given position.
    /// @param position The top left corner of the bitmap to draw.
    /// @param bitmap The bitmap to draw.
    void draw(const Position position, const Bitmap &bitmap) noexcept {
        bitmap.size().forEach([&](const Position delta) -> void { setPixel(position + delta, bitmap.pixel(delta)); });
    }

protected:
    [[nodiscard]] auto pixelRef(const Position pos) -> Data::reference {
        if (!_size.contains(pos)) {
            throw std::out_of_range{"Position out of bounds."};
        }
        return _data[toDataIndex(_size.index(pos))];
    }

protected:
    Size _size;
    Data _data;
};


template <typename T>
    requires std::is_integral_v<T> && std::is_unsigned_v<T>
void Bitmap::draw(const Position position, const std::vector<T> &data) noexcept {
    for (std::size_t y = 0; y < data.size(); ++y) {
        std::bitset<sizeof(T) * 8> row(data[y]);
        for (int x = 0; x < _size.width(); ++x) {
            setPixel(position + Position{x, static_cast<int>(y)}, row[static_cast<std::size_t>(x)]);
        }
    }
}


}
