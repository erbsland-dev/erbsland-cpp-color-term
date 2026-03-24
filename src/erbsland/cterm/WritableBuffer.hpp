// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "BitmapDrawOptions.hpp"
#include "Char16Style.hpp"
#include "CharCombinationStyle.hpp"
#include "FrameDrawOptions.hpp"
#include "ReadableBuffer.hpp"
#include "String.hpp"
#include "Text.hpp"
#include "Tile9Style.hpp"

#include <optional>


namespace erbsland::cterm {

/// Abstract writable terminal buffer interface.
///
/// This base class combines the read-only `ReadableBuffer` API with mutation and higher-level drawing helpers such as
/// frames, text, and bitmap rendering. Concrete implementations like `Buffer` provide the actual storage.
class WritableBuffer : public ReadableBuffer {
public:
    ~WritableBuffer() override = default;

public: // abstract API
    /// Resize this buffer in a memory-efficient way.
    /// The content of the resized buffer is undefined and must be filled with new content.
    /// @param newSize The new size for the buffer.
    virtual void resize(Size newSize) = 0;
    /// Write a block at the given position.
    /// @param pos The coordinates within the buffer.
    /// @param block The block value to store.
    /// @note Writes outside the buffer are ignored.
    virtual void set(Position pos, const Char &block) noexcept = 0;

public: // convenience methods
    /// Copy the content from another buffer and match its size.
    /// This buffer is completely overwritten and resized to the size of `other`.
    /// @param other The buffer to copy from.
    virtual void setAndResizeFrom(const ReadableBuffer &other);
    /// Write a block at the given position using a combination style
    /// @param pos The coordinates within the buffer.
    /// @param block The block value to store.
    /// @param combinationStyle The combination style for overwriting existing characters.
    /// @note Writes outside the buffer are ignored.
    virtual void set(Position pos, const Char &block, const CharCombinationStylePtr &combinationStyle) noexcept;
    /// Write a string at the given position.
    /// NL jumps to the next row. Other control and zero-width characters are ignored.
    /// Color (even inherited) overwrites the existing characters. Use `drawText(pos, text)` for a color overlay.
    /// @param pos The coordinates within the buffer.
    /// @param str The string to write.
    virtual void set(Position pos, const String &str) noexcept;
    /// Copy the content from another buffer into this one.
    /// This buffer is completely overwritten but not resized.
    /// If there is a size mismatch, the contents are either cut off or filled using `fillChar`.
    /// @param other The buffer to copy from.
    /// @param fillChar The character to use for filling if the sizes differ.
    void setFrom(const ReadableBuffer &other, Char fillChar = Char::space());

public: // drawing methods
    /// Fill/clear the buffer with the given character.
    /// @param fillBlock The block to use to fill the buffer.
    virtual void fill(const Char &fillBlock) noexcept;
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
    /// Draw a text without warping from the given position.
    /// A newline breaks to the next line, starting at `pos.x`.
    /// Characters outside this buffer are cut off.
    /// @param pos The start position (top-left corner).
    /// @param str The text to draw on this buffer.
    virtual void drawText(Position pos, const String &str);
    /// If fg or bg is set to `Inherited`, the current color from the buffer is used.
    /// Draw simple text into a rectangle.
    /// If fg or bg is set to `Inherited`, the current color from the buffer is used.
    /// @param text The text description.
    /// @param animationCycle Animation cycle for animated text.
    void drawText(const Text &text, std::size_t animationCycle = 0);
    /// Draw simple text into a rectangle.
    /// If fg or bg is set to `Inherited`, the current color from the buffer is used.
    /// @param text The text to render.
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
    /// @overload
    void drawText(
        String text,
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

protected: // implementation
    /// Implement `setFrom()`.
    /// The public `setFrom()` wrapper forwards to this method.
    /// @param other The buffer to copy from.
    /// @param fillChar The character to use for filling if the sizes differ.
    virtual void setFromImpl(const ReadableBuffer &other, Char fillChar);
    /// Implement `fill(Rectangle, const Char &, ...)`.
    /// The public overload forwards to this method.
    /// @param rect The rectangle to be filled.
    /// @param fillBlock The block for filling.
    /// @param combinationStyle The combination style for overwriting existing characters.
    virtual void
    fillImpl(Rectangle rect, const Char &fillBlock, const CharCombinationStylePtr &combinationStyle) noexcept;
    /// Implement `fill(Rectangle, const Tile9StylePtr &, ...)`.
    /// The public overload forwards to this method.
    /// @param rect The rectangle to be filled.
    /// @param style The tile style to repeat across the rectangle.
    /// @param baseColor The base color underneath the style colors.
    /// @param combinationStyle The combination style for overwriting existing characters.
    virtual void fillImpl(
        Rectangle rect,
        const Tile9StylePtr &style,
        Color baseColor,
        const CharCombinationStylePtr &combinationStyle) noexcept;
    /// Implement the public frame drawing overloads using explicit frame blocks.
    /// The public block-based overloads forward to this method.
    /// @param rect The rectangle for the frame.
    /// @param frameBlock The block for the frame.
    /// @param fillBlock Optional fill block for the interior.
    /// @param combinationStyle The combination style for overwriting existing characters.
    virtual void drawFrameImpl(
        Rectangle rect,
        const Char &frameBlock,
        std::optional<Char> fillBlock,
        const CharCombinationStylePtr &combinationStyle) noexcept;
    /// Implement the public frame drawing overloads using a Char16 style.
    /// The public Char16-based overloads forward to this method.
    /// @param rect The rectangle for the frame.
    /// @param frameStyle The custom frame style.
    /// @param fillBlock Optional fill block for the interior.
    /// @param combinationStyle The combination style for overwriting existing characters.
    /// @param frameColor The base frame color. Any color from the frame style overlays this base color.
    virtual void drawFrameImpl(
        Rectangle rect,
        const Char16StylePtr &frameStyle,
        std::optional<Char> fillBlock,
        const CharCombinationStylePtr &combinationStyle,
        Color frameColor) noexcept;
    /// Implement the public frame drawing overloads using a Tile9 style.
    /// The public Tile9-based overloads forward to this method.
    /// @param rect The rectangle for the frame.
    /// @param style The tile style for the frame.
    /// @param fillBlock Optional fill block for the interior.
    /// @param combinationStyle The combination style for overwriting existing characters.
    /// @param frameColor The base frame color. Any color from the style overlays this base color.
    virtual void drawFrameImpl(
        Rectangle rect,
        const Tile9StylePtr &style,
        std::optional<Char> fillBlock,
        const CharCombinationStylePtr &combinationStyle,
        Color frameColor) noexcept;
    /// Implement `drawFrame(Rectangle, const FrameDrawOptions &, ...)`.
    /// The public options overload forwards to this method.
    /// @param rect The rectangle for the frame.
    /// @param options Frame drawing options.
    /// @param animationCycle Animation cycle for frame and fill color animations.
    virtual void drawFrameImpl(Rectangle rect, const FrameDrawOptions &options, std::size_t animationCycle) noexcept;
    /// Implement `drawText(const Text &, ...)`.
    /// The public overload forwards to this method.
    /// @param text The text description.
    /// @param animationCycle Animation cycle for animated text.
    virtual void drawTextImpl(const Text &text, std::size_t animationCycle);
    /// Implement `drawText(String, Rectangle, ...)`.
    /// The public overload forwards to this method.
    /// @param text The text to render.
    /// @param rect The target rectangle.
    /// @param alignment The alignment inside the rectangle.
    /// @param color The text color.
    /// @param animationCycle Animation cycle for animated text.
    virtual void
    drawTextImpl(String text, Rectangle rect, Alignment alignment, Color color, std::size_t animationCycle);
    /// Implement `drawBitmap(const Bitmap &, Position, ...)`.
    /// The public overload forwards to this method.
    /// @param bitmap The bitmap to draw.
    /// @param pos The position of the top left corner.
    /// @param options Bitmap drawing options.
    /// @param animationCycle Animation cycle for color animations.
    virtual void drawBitmapImpl(
        const Bitmap &bitmap, Position pos, const BitmapDrawOptions &options, std::size_t animationCycle) noexcept;
    /// Implement `drawBitmap(const Bitmap &, Rectangle, ...)`.
    /// The public overload forwards to this method.
    /// @param bitmap The bitmap to draw.
    /// @param rect The rectangle to draw the bitmap into.
    /// @param alignment Alignment of the bitmap within the rectangle.
    /// @param options Bitmap drawing options.
    /// @param animationCycle Animation cycle for color animations.
    virtual void drawBitmapImpl(
        const Bitmap &bitmap,
        Rectangle rect,
        Alignment alignment,
        const BitmapDrawOptions &options,
        std::size_t animationCycle) noexcept;
};


}
