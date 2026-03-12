// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Char.hpp"
#include "Rectangle.hpp"
#include "Size.hpp"

#include <memory>


namespace erbsland::cterm {


class ReadableBuffer;
using ReadableBufferPtr = std::shared_ptr<ReadableBuffer>;
class Bitmap;
class WritableBuffer;
using WritableBufferPtr = std::shared_ptr<WritableBuffer>;


/// A readable buffer.
class ReadableBuffer {
public:
    virtual ~ReadableBuffer() = default;

public: // API
    /// Get the configured size of the buffer.
    /// @return The width and height of the buffer.
    [[nodiscard]] virtual auto size() const noexcept -> Size = 0;
    /// Get a rectangle representing this buffer.
    /// @return The rectangle for this buffer.
    [[nodiscard]] virtual auto rect() const noexcept -> Rectangle = 0;
    /// Read the block stored at the given position.
    /// @param pos The coordinates within the buffer.
    /// @return A reference to the stored block.
    [[nodiscard]] virtual auto get(Position pos) const noexcept -> const Char & = 0;
    /// Create a writeable copy of this buffer.
    /// This will copy every block from this buffer into a new independent instance.
    [[nodiscard]] virtual auto clone() const -> WritableBufferPtr = 0;

public: // ----- convenience methods -----
    /// Count the differences from this to another buffer.
    /// If the size of `other` is smaller or larger than this buffer, the size change counts to the difference.
    /// @param other The other buffer to compare with.
    /// @return The number of blocks that differ between the two buffers.
    [[nodiscard]] virtual auto countDifferencesTo(const ReadableBuffer &other) const noexcept -> std::size_t;
    /// Create a mask from this buffer.
    /// All characters that match one of the given characters result in a pixel set in the mask.
    /// @param characters The characters to match. Only one code-point characters are supported.
    /// @param invert If true, *not*-matching characters result in a pixel set in the mask.
    /// @return A bitmap mask with the same size as this buffer.
    [[nodiscard]] virtual auto toMask(const std::u32string &characters, bool invert = false) -> Bitmap;
    /// @overload
    [[nodiscard]] virtual auto toMask(std::initializer_list<char32_t> characters, bool invert = false) -> Bitmap;
};


}
