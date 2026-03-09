// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Char16Style.hpp"
#include "CharCombinationStyle.hpp"
#include "Color.hpp"
#include "ColorPart.hpp"
#include "ColorSequence.hpp"
#include "String.hpp"


namespace erbsland::cterm {


/// The mode how color is applied to the bitmap.
enum class BitmapColorMode : uint8_t {
    /// Uses one color from the sequence for the whole bitmap.
    /// The selected sequence entry is `animationCycle + colorAnimationOffset`.
    OneColor,
    /// Uses the color sequence in vertical stripes.
    /// The selected sequence entry is `x + animationCycle + colorAnimationOffset`.
    VerticalStripes,
    /// Uses the color sequence in horizontal stripes.
    /// The selected sequence entry is `y + animationCycle + colorAnimationOffset`.
    HorizontalStripes,
    /// Uses the color sequence in forward diagonal stripes.
    /// The selected sequence entry is `x + y + animationCycle + colorAnimationOffset`.
    ForwardDiagonalStripes,
    /// Uses the color sequence in backward diagonal stripes.
    /// The selected sequence entry is `-x + y + animationCycle + colorAnimationOffset`.
    BackwardDiagonalStripes,
};

/// The mode how the bitmap is scaled.
enum class BitmapScaleMode : uint8_t {
    /// Draw the bitmap with half-blocks.
    /// This mode uses the 16 characters from `halfBlocks()`.
    /// Each 2x2 pixel block creates a 4-bit index in this order:
    /// bit 0 = top-left, bit 1 = top-right, bit 2 = bottom-left, bit 3 = bottom-right.
    /// This renders the bitmap at half width and half height, rounded up.
    /// Character colors are overlaid on the color from the color mode.
    HalfBlock,
    /// Draw the bitmap with full-blocks.
    /// This mode uses `fullBlock()` to draw each set pixel of the bitmap.
    /// Character colors are overlaid on the color from the color mode.
    /// To color the unset pixels, you must fill the bitmap area first.
    FullBlock,
    /// Draw the bitmap with double-blocks.
    /// This mode uses `doubleBlocks()` to draw each set pixel of the bitmap.
    /// The bitmap is twice as large in the X axis, to compensate for the rectangular shape of terminal characters.
    /// Character colors are overlaid on the color from the color mode.
    /// To color the unset pixels, you must fill the bitmap area first.
    DoubleBlock,
};


/// The options to draw a bitmap.
/// These options define how `Buffer::drawBitmap()` converts bitmap pixels into terminal cells.
/// For color animation and stripe modes, the color position is calculated in the rendered bitmap grid:
/// `FullBlock`, `DoubleBlock`, and `Char16Style` use one logical position per bitmap pixel,
/// while `HalfBlock` uses one logical position per 2x2 pixel cell.
/// The rectangle overload of `drawBitmap()` aligns this rendered grid inside the target rectangle and crops it if
/// needed.
/// @note Creating custom option instances is expensive. For that reason, create them once and
///       keep the instances for multiple `drawBitmap` calls.
class BitmapDrawOptions final {
public:
    /// Create default bitmap draw options.
    BitmapDrawOptions() = default;
    /// Create options for one fixed color.
    /// @param color The base color for the bitmap.
    BitmapDrawOptions(Color color);
    /// Create options with only a foreground color.
    /// @param foreground The base foreground color.
    BitmapDrawOptions(Foreground foreground);
    /// @overload
    BitmapDrawOptions(Foreground::Hue foreground);
    /// Create options with explicit foreground and background colors.
    /// @param foreground The base foreground color.
    /// @param background The base background color.
    BitmapDrawOptions(Foreground foreground, Background background);
    /// Create options with only a background color.
    /// @param background The base background color.
    BitmapDrawOptions(Background background);
    /// @overload
    BitmapDrawOptions(Background::Hue background);
    /// Create options from a color sequence.
    /// @param colorSequence The base colors for the bitmap.
    /// @param colorMode The mode used to pick colors from the sequence.
    BitmapDrawOptions(ColorSequence colorSequence, BitmapColorMode colorMode = BitmapColorMode::OneColor);

    // defaults
    ~BitmapDrawOptions() = default;
    BitmapDrawOptions(const BitmapDrawOptions &) = default;
    BitmapDrawOptions(BitmapDrawOptions &&) noexcept = default;
    auto operator=(const BitmapDrawOptions &) -> BitmapDrawOptions & = default;
    auto operator=(BitmapDrawOptions &&) -> BitmapDrawOptions & = default;

public:
    /// The color to use for drawing the bitmap.
    /// The color can be either a single color or a sequence of colors.
    /// If this is an empty sequence, the color is inherited from the buffer.
    /// Colors are applied using the `colorMode()`.
    /// If the characters in `fullBlock()`, `doubleBlocks()` or `halfBlocks()` have colors set,
    /// these colors are overlaid *after* calculating this base color.
    /// @note For full-block and double-block mode, the background color is only applied to set pixels.
    ///       Fill the bitmap area if you need a custom background color for unset pixels.
    [[nodiscard]] auto color() const noexcept -> const ColorSequence &;
    /// Set a single color.
    /// Replaces the current color sequence with one entry.
    void setColor(Color color) noexcept;
    /// @overload
    void setColor(Foreground foreground, Background background) noexcept;
    /// Set only a foreground color.
    /// The background remains inherited from the buffer below.
    void setColor(Foreground foreground) noexcept;
    /// @overload
    void setColor(Foreground::Hue foreground) noexcept;
    /// Set only a background color.
    /// The foreground remains inherited from the buffer below.
    void setColor(Background background) noexcept;
    /// @overload
    void setColor(Background::Hue background) noexcept;
    /// Set a color sequence.
    /// Pass an empty `ColorSequence{}` to inherit the complete color from the buffer below.
    void setColorSequence(ColorSequence colorSequence, BitmapColorMode colorMode = BitmapColorMode::OneColor) noexcept;
    /// The color mode.
    /// This mode controls how colors are applied to the bitmap.
    /// See `BitmapColorMode` for more information.
    [[nodiscard]] auto colorMode() const noexcept -> BitmapColorMode;
    /// Set the color mode.
    void setColorMode(BitmapColorMode colorMode) noexcept;
    /// The offset for color animations.
    /// This offset is added to the `animationCycle` passed to `drawBitmap`.
    /// Animate `animationCycle` and keep this offset static.
    [[nodiscard]] auto colorAnimationOffset() const noexcept -> std::size_t;
    /// Set the offset for color animations.
    void setColorAnimationOffset(std::size_t offset) noexcept;
    /// The Char16Style instance.
    /// If a Char16Style instance is set, it *overrides* the scale mode and renders one terminal cell for each set
    /// bitmap pixel. The selected block depends on the four direct neighbors of the set pixel:
    /// east=bit 0, south=bit 1, west=bit 2, north=bit 3.
    [[nodiscard]] auto char16Style() const noexcept -> const Char16StylePtr &;
    /// Set a Char16Style instance.
    void setChar16Style(Char16StylePtr char16Style) noexcept;
    /// The combination style.
    /// If a combination style is set, every block that is set in the buffer is first passed to
    /// this combination style. This happens for every mode used to draw the bitmap.
    [[nodiscard]] auto combinationStyle() const noexcept -> const CharCombinationStylePtr &;
    /// Set the combination style.
    void setCombinationStyle(CharCombinationStylePtr combinationStyle) noexcept;
    /// The full block.
    /// The full block is only used when the scale mode `FullBlock` is used and no `Char16Style` is set.
    /// Character colors are overlaid on the color from the color mode.
    [[nodiscard]] auto fullBlock() const noexcept -> const Char &;
    /// Set the full block.
    /// The full block must have a display width of 1.
    void setFullBlock(Char fullBlock);
    /// The double blocks.
    /// The double blocks are only used when the scale mode `DoubleBlock` is used and no `Char16Style` is set.
    /// Character index 0 is used for the left half and index 1 for the right half of each set bitmap pixel.
    /// Character colors are overlaid on the color from the color mode.
    [[nodiscard]] auto doubleBlocks() const noexcept -> const String &;
    /// Set the double block String.
    /// The string must have exactly two characters.
    void setDoubleBlocks(String doubleBlocks);
    /// The string with the half-blocks.
    /// The half-blocks are only used when the scale mode `HalfBlock` is used and no `Char16Style` is set.
    /// Entry `0` is used for an empty 2x2 block and entry `15` for a full 2x2 block.
    /// Character colors are overlaid on the color from the color mode.
    [[nodiscard]] auto halfBlocks() const noexcept -> const String &;
    /// Set the half-blocks string.
    /// The half-blocks string must have exactly 16 characters.
    void setHalfBlocks(String halfBlocks);
    /// The scale mode.
    /// See `BitmapScaleMode` for more details.
    [[nodiscard]] auto scaleMode() const noexcept -> BitmapScaleMode;
    /// Set the scale mode.
    void setScaleMode(BitmapScaleMode scaleMode) noexcept;

public:
    /// Access the shared object with the default options.
    /// Default options use default terminal colors, half-block rendering, and the standard Unicode half-block
    /// characters.
    [[nodiscard]] static auto defaultOptions() noexcept -> const BitmapDrawOptions &;

private:
    ColorSequence _color = {Color{fg::Default, bg::Default}};
    BitmapColorMode _colorMode = BitmapColorMode::OneColor;
    std::size_t _colorAnimationOffset = 0;
    Char16StylePtr _char16Style;
    CharCombinationStylePtr _combinationStyle;
    Char _fullBlock{"█"};
    String _doubleBlocks{"██"};
    String _halfBlocks{" ▘▝▀▖▌▞▛▗▚▐▜▄▙▟█"};
    BitmapScaleMode _scaleMode = BitmapScaleMode::HalfBlock;
};


}
