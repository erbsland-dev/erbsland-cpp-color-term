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

#include <string_view>
#include <vector>


namespace erbsland::cterm {


/// A mutable 2D buffer storing characters and colors for rendering.
///
/// Handling of non-1-width blocks:
/// - Blocks with zero display width are ignored.
/// - Blocks with a display width of 2 will overwrite two adjacent cells.
///   - The first cell will contain the set character, the next cell a zero-width (empty) character.
///   - Both cells will have the same color.
///   - A 2-width block at the right edge is ignored.
/// - Blocks with a display width > 2 are ignored.
class Buffer {
public:
    /// Construct a buffer with the given size and fill it with an initial block.
    /// @param size The dimensions of the buffer.
    explicit Buffer(Size size) noexcept;

public: // low-level access
    /// Get the configured size of the buffer.
    /// @return The width and height of the buffer.
    [[nodiscard]] auto size() const noexcept -> Size;
    /// Get a rectangle representing this buffer.
    /// @return The rectangle for this buffer.
    [[nodiscard]] auto rect() const noexcept -> Rectangle;
    /// Read the block stored at the given position.
    /// @param pos The coordinates within the buffer.
    /// @return A reference to the stored block.
    [[nodiscard]] auto get(Position pos) const noexcept -> const Char &;
    /// Write a block at the given position.
    /// @param pos The coordinates within the buffer.
    /// @param block The block value to store.
    /// @note Writes outside the buffer are ignored.
    void set(Position pos, const Char &block) noexcept;
    /// @overload
    void set(Position pos, Char &&block) noexcept;
    /// Write a block at the given position using a combination style
    /// @param pos The coordinates within the buffer.
    /// @param block The block value to store.
    /// @param combinationStyle The combination style for overwriting existing characters.
    /// @note Writes outside the buffer are ignored.
    void set(Position pos, const Char &block, const CharCombinationStylePtr &combinationStyle) noexcept;

public: // drawing methods
    /// Fill/clear the buffer with the given character.
    /// @param fillBlock The block to use to fill the buffer.
    void fill(const Char &fillBlock) noexcept;
    /// Fill the given rectangle.
    /// Positions outside the buffer are ignored.
    /// @param rect The rectangle to be filled.
    /// @param fillBlock The block for filling.
    /// @param combinationStyle The combination style for overwriting existing characters.
    void fill(Rectangle rect, const Char &fillBlock, const CharCombinationStylePtr &combinationStyle = {}) noexcept;
    /// Fill the given rectangle using a repeating 9-tile style.
    /// Positions outside the buffer are ignored.
    /// @param rect The rectangle to be filled.
    /// @param style The tile style to repeat across the rectangle.
    /// @param baseColor The base color underneath the style colors.
    /// @param combinationStyle The combination style for overwriting existing characters.
    void fill(
        Rectangle rect,
        const Tile9StylePtr &style,
        Color baseColor = {},
        const CharCombinationStylePtr &combinationStyle = {}) noexcept;
    /// Draw a frame inside a given rectangle
    /// This will set all blocks at the edge, *inside* the given rectangle
    /// @param rect The rectangle for the frame.
    /// @param frameBlock The block for the frame.
    /// @param combinationStyle The combination style for overwriting existing characters.
    void
    drawFrame(Rectangle rect, const Char &frameBlock, const CharCombinationStylePtr &combinationStyle = {}) noexcept;
    /// Draw a frame inside a given rectangle
    /// This will set all blocks at the edge, *inside* the given rectangle
    /// @param rect The rectangle for the frame.
    /// @param frameStyle A custom frame style.
    /// @param combinationStyle The combination style for overwriting existing characters.
    /// @param frameColor The base frame color. Any color from the frame style overlays this base color.
    void drawFrame(
        Rectangle rect,
        const Char16StylePtr &frameStyle,
        const CharCombinationStylePtr &combinationStyle = {},
        Color frameColor = {}) noexcept;
    /// Draw a frame inside a given rectangle using a repeating 9-tile style.
    /// This will set all blocks at the edge, *inside* the given rectangle.
    /// @param rect The rectangle for the frame.
    /// @param style The tile style for the frame.
    /// @param frameColor The base frame color. Any color from the style overlays this base color.
    /// @param combinationStyle The combination style for overwriting existing characters.
    void drawFrame(
        Rectangle rect,
        const Tile9StylePtr &style,
        Color frameColor = {},
        const CharCombinationStylePtr &combinationStyle = {}) noexcept;
    /// Draw a frame inside a given rectangle
    /// This will set all blocks at the edge, *inside* the given rectangle
    /// @param rect The rectangle for the frame.
    /// @param frameStyle The predefined frame style.
    /// @param frameColor The base frame color. Any color from the frame style overlays this base color.
    void drawFrame(Rectangle rect, FrameStyle frameStyle, Color frameColor = {}) noexcept;
    /// Draw a frame inside a given rectangle with configurable style, fill, and animated colors.
    /// This will set all blocks at the edge, *inside* the given rectangle.
    /// If `options.fillBlock()` is empty and no `Tile9Style` is active, the interior is left unchanged.
    /// @param rect The rectangle for the frame.
    /// @param options Frame drawing options.
    /// @param animationCycle Animation cycle for frame and fill color animations.
    void drawFrame(
        Rectangle rect,
        const FrameDrawOptions &options = FrameDrawOptions::defaultOptions(),
        std::size_t animationCycle = 0) noexcept;
    /// Draw a box and fill it.
    /// @param rect The rectangle for the frame.
    /// @param frameBlock The block for the frame.
    /// @param fillBlock The block for filling.
    /// @param combinationStyle The combination style for overwriting existing characters.
    void drawFilledFrame(
        Rectangle rect,
        const Char &frameBlock,
        const Char &fillBlock,
        const CharCombinationStylePtr &combinationStyle = {}) noexcept;
    /// Draw a box and fill it.
    /// @param rect The rectangle for the frame.
    /// @param frameStyle A custom frame style.
    /// @param fillBlock The block for filling.
    /// @param combinationStyle The combination style for overwriting existing characters.
    /// @param frameColor The base frame color. Any color from the frame style overlays this base color.
    void drawFilledFrame(
        Rectangle rect,
        const Char16StylePtr &frameStyle,
        const Char &fillBlock,
        const CharCombinationStylePtr &combinationStyle = {},
        Color frameColor = {}) noexcept;
    /// Draw a box and fill it using a repeating 9-tile style for the frame.
    /// @param rect The rectangle for the frame.
    /// @param style The tile style for the frame.
    /// @param fillBlock The block for filling.
    /// @param combinationStyle The combination style for overwriting existing characters.
    /// @param frameColor The base frame color. Any color from the style overlays this base color.
    void drawFilledFrame(
        Rectangle rect,
        const Tile9StylePtr &style,
        const Char &fillBlock,
        const CharCombinationStylePtr &combinationStyle = {},
        Color frameColor = {}) noexcept;
    /// Draw a box and fill it.
    /// @param rect The rectangle for the frame.
    /// @param frameStyle The predefined frame style.
    /// @param fillBlock The block for filling.
    /// @param frameColor The base frame color. Any color from the frame style overlays this base color.
    void drawFilledFrame(Rectangle rect, FrameStyle frameStyle, const Char &fillBlock, Color frameColor = {}) noexcept;
    /// Draw simple text into a rectangle.
    /// If fg or bg is set to `Inherited`, the current color from the buffer is used.
    /// @param text The text description.
    /// @param animationCycle Animation cycle for animated text.
    void drawText(const Text &text, std::size_t animationCycle = 0);
    /// Draw simple text into a rectangle.
    /// If fg or bg is set to `Inherited`, the current color from the buffer is used.
    /// @param text The UTF-8 text to render.
    /// @param alignment The alignment inside the rectangle.
    /// @param rect The target rectangle.
    /// @param color The text color.
    /// @param animationCycle Animation cycle for animated text.
    void drawText(
        std::string_view text, Alignment alignment, Rectangle rect, Color color = {}, std::size_t animationCycle = 0);
    /// Draw simple text into a rectangle.
    /// If fg or bg is set to `Inherited`, the current color from the buffer is used.
    /// @param text The UTF-8 text to render.
    /// @param rect The target rectangle.
    /// @param alignment The alignment inside the rectangle.
    /// @param color The text color.
    /// @param animationCycle Animation cycle for animated text.
    void drawText(
        std::string_view text,
        Rectangle rect,
        Alignment alignment = Alignment::TopLeft,
        Color color = {},
        std::size_t animationCycle = 0);
    /// Draw a bitmap at a given position.
    /// The bitmap is rendered according to `options.scaleMode()`. If `options.char16Style()` is set,
    /// it overrides the scale mode and renders one terminal cell per bitmap pixel.
    /// Pixels or rendered cells outside the buffer are ignored.
    /// @param bitmap The bitmap to draw.
    /// @param pos The position of the top left corner.
    /// @param options Bitmap drawing options.
    /// @param animationCycle Animation cycle for color animations.
    void drawBitmap(
        const Bitmap &bitmap,
        Position pos,
        const BitmapDrawOptions &options = BitmapDrawOptions::defaultOptions(),
        std::size_t animationCycle = 0) noexcept;
    /// Draw a bitmap into the given rectangle.
    /// The rendered bitmap is aligned inside `rect`. If it is larger than `rect`, it is cropped according to the
    /// alignment.
    /// @note For half-block drawing mode, alignment and cropping happen at rendered cell boundaries, not per pixel.
    /// @param bitmap The bitmap to draw.
    /// @param rect The rectangle to draw the bitmap into.
    /// @param alignment Alignment of the bitmap within the rectangle.
    /// @param options Bitmap drawing options.
    /// @param animationCycle Animation cycle for color animations.
    void drawBitmap(
        const Bitmap &bitmap,
        Rectangle rect,
        Alignment alignment = Alignment::TopLeft,
        const BitmapDrawOptions &options = BitmapDrawOptions::defaultOptions(),
        std::size_t animationCycle = 0) noexcept;

private:
    [[nodiscard]] static auto applyBaseColor(const Char &block, Color baseColor) -> Char;
    [[nodiscard]] static auto blockForFrame(Rectangle rect, Position pos, const Char16StylePtr &frameStyle) -> Char;
    [[nodiscard]] auto buildTextLines(const Text &text) const -> BlockStringLines;
    [[nodiscard]] auto buildFontTextLines(const Text &text) const -> BlockStringLines;
    void applyTextLines(const Text &text, const BlockStringLines &lines, std::size_t animationCycle) noexcept;
    [[nodiscard]] static auto bitmapRenderSize(const Bitmap &bitmap, const BitmapDrawOptions &options) noexcept
        -> Size;
    [[nodiscard]] static auto alignedBitmapOffset(
        int renderedSize,
        int availableSize,
        Alignment alignment,
        Alignment alignmentMask) noexcept -> int;
    [[nodiscard]] auto
    colorForBitmapPosition(const BitmapDrawOptions &options, Position bitmapPosition, std::size_t animationCycle) const
        noexcept -> Color;
    void drawBitmapBlock(Position pos, const Char &block, Color baseColor, const BitmapDrawOptions &options) noexcept;
    void drawFrameBlock(
        Position pos, const Char &block, Color baseColor, const CharCombinationStylePtr &combinationStyle) noexcept;
    [[nodiscard]] static auto colorForFramePosition(
        const ColorSequence &colorSequence,
        FrameColorMode colorMode,
        Rectangle rect,
        Position pos,
        std::size_t animationCycle,
        std::size_t animationOffset) noexcept -> Color;
    [[nodiscard]] auto colorForTextPosition(
        const Text &text, const Char &character, Position position, std::size_t animationCycle) const noexcept -> Color;

private:
    Size _size;
    std::vector<Char> _data;
};


}
