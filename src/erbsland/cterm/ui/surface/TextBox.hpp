// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Surface.hpp"

#include <optional>

namespace erbsland::cterm::ui::surface {

class TextBox;
using TextBoxPtr = std::shared_ptr<TextBox>;

/// A surface that renders one string using alignment and wrapping rules.
class TextBox : public Surface {
public:
    /// Create a text box for the given text and alignment.
    /// @param text The initial text to render.
    /// @param alignment The alignment inside the allocated rectangle.
    TextBox(String text, Alignment alignment, ProtectedTag);

    // defaults
    ~TextBox() override = default;

public:
    /// Create a text box from a terminal string.
    /// @param text The initial text to render.
    /// @param alignment The alignment inside the allocated rectangle.
    /// @return The new text box instance.
    [[nodiscard]] static auto create(String text, Alignment alignment = Alignment::TopLeft) -> TextBoxPtr;
    /// Create a text box from UTF-8 text.
    /// @param text The initial text to render.
    /// @param alignment The alignment inside the allocated rectangle.
    /// Invalid UTF-8 bytes are replaced with the Unicode replacement character.
    /// @return The new text box instance.
    [[nodiscard]] static auto create(std::string_view text, Alignment alignment = Alignment::TopLeft) -> TextBoxPtr;

public:
    /// Get the current text content.
    /// @return The text rendered by this text box.
    [[nodiscard]] auto text() const noexcept -> const String &;
    /// Replace the current text content.
    /// @param text The new text to render.
    void setText(String text);
    /// Get the text alignment.
    [[nodiscard]] auto alignment() const noexcept -> Alignment;
    /// Set the text alignment.
    /// @param alignment The new text alignment.
    void setAlignment(Alignment alignment);
    /// Access the preferred content line width.
    /// @return The preferred line width, or an empty value for natural unwrapped width.
    [[nodiscard]] auto preferredLineWidth() const noexcept -> std::optional<Coordinate>;
    /// Replace the preferred content line width.
    /// @param width The preferred line width, or an empty value for natural unwrapped width.
    void setPreferredLineWidth(std::optional<Coordinate> width);

public:
    /// Measure this text box for a proposed width.
    [[nodiscard]] auto onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept
        -> LayoutMetrics override;
    /// Render the text into the target rectangle.
    /// @param buffer The destination buffer.
    /// @param context The paint context.
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

private:
    void initializeUi() override;
    void updatePreferredSize();
    [[nodiscard]] auto preferredTextSize() const noexcept -> Size;
    [[nodiscard]] auto textSizeForWidth(Coordinate width) const noexcept -> Size;

private:
    String _text;                                  ///< The text to be displayed in the text box.
    Alignment _alignment;                          ///< The text alignment.
    std::optional<Coordinate> _preferredLineWidth; ///< Preferred readable content width.
};

}
