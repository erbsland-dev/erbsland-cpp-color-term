// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Alignment.hpp"
#include "ColorSequence.hpp"
#include "Font.hpp"
#include "Rectangle.hpp"
#include "String.hpp"
#include "TextAnimation.hpp"

#include <utility>


namespace erbsland::cterm {


/// Describes a text block to render into a `Buffer`.
class Text final {
public: // ctors/dtor/assign/move
    /// Create an empty text description.
    Text() = default;
    /// Create a text description for the given content and target rectangle.
    /// @param text The text content to render.
    /// @param rect The target rectangle.
    /// @param alignment The text alignment inside the rectangle.
    Text(String text, Rectangle rect, const Alignment alignment = Alignment::TopLeft) noexcept :
        _text{std::move(text)}, _rectangle{rect}, _alignment{alignment} {}

public: // accessors
    /// Access the text content.
    [[nodiscard]] auto text() const noexcept -> const String & { return _text; }
    /// Access the target rectangle.
    [[nodiscard]] auto rectangle() const noexcept -> const Rectangle & { return _rectangle; }
    /// Access the text alignment inside the rectangle.
    [[nodiscard]] auto alignment() const noexcept -> Alignment { return _alignment; }
    /// Access the optional text color sequence.
    [[nodiscard]] auto colorSequence() const noexcept -> const ColorSequence & { return _colorSequence; }
    /// Get the first text color from the configured sequence.
    /// @return The first sequence color, or the default color if no sequence is configured.
    [[nodiscard]] auto color() const noexcept -> Color {
        if (_colorSequence.empty()) {
            return {};
        }
        return _colorSequence.color(0);
    }
    /// Access the optional font. If this is empty, regular text rendering is used.
    [[nodiscard]] auto font() const noexcept -> const FontPtr & { return _font; }
    /// Access the animation mode.
    [[nodiscard]] auto animation() const noexcept -> TextAnimation { return _animation; }

public: // modifiers
    /// Replace the text content.
    void setText(String text) noexcept { _text = std::move(text); }
    /// Replace the target rectangle.
    void setRectangle(const Rectangle rect) noexcept { _rectangle = rect; }
    /// Change the text alignment.
    void setAlignment(const Alignment alignment) noexcept { _alignment = alignment; }
    /// Replace the text color sequence.
    void setColorSequence(ColorSequence colorSequence) noexcept { _colorSequence = std::move(colorSequence); }
    /// Set a single text color.
    void setColor(const Color color) noexcept { _colorSequence = ColorSequence{color}; }
    /// Change the font.
    void setFont(const FontPtr &font) noexcept { _font = font; }
    /// Change the animation mode.
    void setAnimation(const TextAnimation animation) noexcept { _animation = animation; }

private:
    String _text;
    Rectangle _rectangle;
    Alignment _alignment{Alignment::TopLeft};
    ColorSequence _colorSequence;
    FontPtr _font;
    TextAnimation _animation{TextAnimation::None};
};


}
