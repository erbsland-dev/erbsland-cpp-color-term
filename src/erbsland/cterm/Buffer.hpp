// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Alignment.hpp"
#include "Char.hpp"
#include "Char16Style.hpp"
#include "CharCombinationStyle.hpp"
#include "ColorSequence.hpp"
#include "FrameStyle.hpp"
#include "Position.hpp"
#include "Rectangle.hpp"
#include "Size.hpp"
#include "String.hpp"
#include "Text.hpp"

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
    /// Draw a frame inside a given rectangle
    /// This will set all blocks at the edge, *inside* the given rectangle
    /// @param rect The rectangle for the frame.
    /// @param frameStyle The predefined frame style.
    /// @param frameColor The base frame color. Any color from the frame style overlays this base color.
    void drawFrame(Rectangle rect, FrameStyle frameStyle, Color frameColor = {}) noexcept;
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
    /// Draw a box and fill it.
    /// @param rect The rectangle for the frame.
    /// @param frameStyle The predefined frame style.
    /// @param fillBlock The block for filling.
    /// @param frameColor The base frame color. Any color from the frame style overlays this base color.
    void drawFilledFrame(Rectangle rect, FrameStyle frameStyle, const Char &fillBlock, Color frameColor = {}) noexcept;
    /// Draw simple text into a rectangle.
    /// @param text The text description.
    /// @param animationCycle Animation cycle for animated text.
    void drawText(const Text &text, std::size_t animationCycle = 0);
    /// Draw simple text into a rectangle.
    /// @param text The UTF-8 text to render.
    /// @param alignment The alignment inside the rectangle.
    /// @param rect The target rectangle.
    /// @param color The text color.
    /// @param animationCycle Animation cycle for animated text.
    void drawText(
        std::string_view text, Alignment alignment, Rectangle rect, Color color = {}, std::size_t animationCycle = 0);
    /// Draw simple text into a rectangle.
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

private:
    [[nodiscard]] static auto applyFrameColor(const Char &frameBlock, Color frameColor) -> Char;
    [[nodiscard]] static auto blockForFrame(Rectangle rect, Position pos, const Char16StylePtr &frameStyle, Color frameColor)
        -> Char;
    [[nodiscard]] static auto outerHalfBlockForFrame(Rectangle rect, Position pos, Color frameColor) -> Char;
    [[nodiscard]] static auto innerHalfBlockForFrame(Rectangle rect, Position pos, Color frameColor) -> Char;
    [[nodiscard]] auto buildTextLines(const Text &text) const -> BlockStringLines;
    [[nodiscard]] auto buildFontTextLines(const Text &text) const -> BlockStringLines;
    void applyTextLines(const Text &text, const BlockStringLines &lines, std::size_t animationCycle) noexcept;
    [[nodiscard]] auto colorForTextPosition(
        const Text &text, const Char &character, Position position, std::size_t animationCycle) const noexcept -> Color;

private:
    Size _size;
    std::vector<Char> _data;
};


}
