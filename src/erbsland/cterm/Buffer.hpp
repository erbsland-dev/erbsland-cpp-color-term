// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Alignment.hpp"
#include "BitmapDrawOptions.hpp"
#include "Char.hpp"
#include "Char16Style.hpp"
#include "CharCombinationStyle.hpp"
#include "ColorSequence.hpp"
#include "FrameDrawOptions.hpp"
#include "FrameStyle.hpp"
#include "Position.hpp"
#include "Rectangle.hpp"
#include "Size.hpp"
#include "String.hpp"
#include "Text.hpp"
#include "Tile9Style.hpp"
#include "WritableBuffer.hpp"

#include <string_view>
#include <vector>


namespace erbsland::cterm {


class Buffer;
using BufferPtr = std::shared_ptr<Buffer>;


/// A mutable 2D buffer storing characters and colors for rendering.
///
/// Handling of non-1-width blocks:
/// - Blocks with zero display width are ignored.
/// - Blocks with a display width of 2 will overwrite two adjacent cells.
///   - The first cell will contain the set character, the next cell a zero-width (empty) character.
///   - Both cells will have the same color.
///   - A 2-width block at the right edge is ignored.
/// - Blocks with a display width > 2 are ignored.
class Buffer final : public WritableBuffer {
public:
    /// Largest buffer size accepted for this buffer.
    constexpr static auto cMaximumSize = Size{10'000, 10'000};
    /// Smallest valid buffer size.
    constexpr static auto cMinimumSize = Size{1, 1};

    using WritableBuffer::drawBitmap;
    using WritableBuffer::drawText;
    using WritableBuffer::fill;
    using WritableBuffer::get;
    using WritableBuffer::resize;
    using WritableBuffer::set;

public:
    /// Creates a 1x1 buffer filled with a space.
    /// Usually only used as a placeholder until resized.
    Buffer();

    /// Construct a buffer with the given size and fill it with an initial block.
    /// @param size The dimensions of the buffer. Size must be at least 1x1.
    /// @param fillChar The optional fill character for the buffer.
    /// @throws std::invalid_argument if size is invalid
    explicit Buffer(Size size, Char fillChar = Char::space());

    // defaults
    ~Buffer() override = default;
    Buffer(const Buffer &) = default;
    Buffer(Buffer &&) = default;
    auto operator=(const Buffer &) -> Buffer & = default;
    auto operator=(Buffer &&) -> Buffer & = default;

public: // implement ReadableBuffer
    [[nodiscard]] auto size() const noexcept -> Size override;
    [[nodiscard]] auto rect() const noexcept -> Rectangle override;
    [[nodiscard]] auto get(Position pos) const noexcept -> const Char & override;
    [[nodiscard]] auto clone() const -> WritableBufferPtr override;

public: // implement WritableBuffer
    void resize(Size newSize) override;
    void set(Position pos, const Char &block) noexcept override;
    void setAndResizeFrom(const ReadableBuffer &other) override;

public:
    /// Resize this buffer.
    /// Resizing the buffer *will not clear* it.
    /// If `reorder` is `true`, the content will either be expanded with space blocks or cropped.
    /// If `reorder` is `false`, the state of the resized buffer is undefined (you must clear it yourself).
    /// @param size The new size.
    /// @param reorder Whether to reorder the content when resizing.
    /// @param fillChar The character to fill the new space with.
    /// @throws std::invalid_argument if size is invalid
    void resize(Size size, bool reorder, Char fillChar = Char::space());

public: // faster implementations
    /// Fill/clear the buffer with the given character.
    /// @param fillBlock The block to use to fill the buffer.
    void fill(const Char &fillBlock) noexcept override;

public: // builders
    /// Creates a buffer from the lines in a string.
    /// This function splits the given string into lines and creates a buffer with a matching size.
    /// @param text The string to split into lines and create a buffer from. Must not be empty.
    /// @return A buffer containing the lines from the input string.
    [[nodiscard]] static auto fromLinesInString(const String &text) -> Buffer;

    /// Creates a buffer from the lines in a string.
    /// @param lines The lines to create the buffer from. Must not be empty.
    /// @return A buffer containing the lines from the input string.
    [[nodiscard]] static auto fromLines(const StringLines &lines) -> Buffer;

public: // compatibility
    /// Draw text into a rectangle using the legacy parameter order.
    /// @deprecated Use `drawText(std::string_view, Rectangle, ...)` instead.
    /// @param text The text to render.
    /// @param alignment The alignment inside the rectangle.
    /// @param rect The target rectangle.
    /// @param color The text color.
    /// @param animationCycle Animation cycle for animated text.
    [[deprecated("Use drawText(std::string_view, Rectangle, ...) instead")]]
    void drawText(
        std::string_view text, Alignment alignment, Rectangle rect, Color color = {}, std::size_t animationCycle = 0);

private:
    static void validateBufferSize(Size size);

private:
    Size _size{cMinimumSize};
    std::vector<Char> _data{Char::space()};
};


}
