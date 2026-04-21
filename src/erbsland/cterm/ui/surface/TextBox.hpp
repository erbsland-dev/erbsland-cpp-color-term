// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Surface.hpp"

#include <optional>

namespace erbsland::cterm::ui::surface {

class TextBox;
using TextBoxPtr = std::shared_ptr<TextBox>;

/// A surface that renders one string using TextOptions-derived alignment and wrapping rules.
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
    [[nodiscard]] static auto create(String text, Alignment alignment = Alignment::TopLeft) noexcept -> TextBoxPtr;
    /// Create a text box from UTF-8 text.
    /// @param text The initial text to render.
    /// @param alignment The alignment inside the allocated rectangle.
    /// Invalid UTF-8 bytes are replaced with the Unicode replacement character.
    /// @return The new text box instance.
    [[nodiscard]] static auto create(std::string_view text, Alignment alignment = Alignment::TopLeft) noexcept
        -> TextBoxPtr;

public:
    /// Get the current text content.
    /// @return The text rendered by this text box.
    [[nodiscard]] auto text() const noexcept -> const String &;
    /// Replace the current text content.
    /// @param text The new text to render.
    void setText(String text);
    /// Get the current text rendering options.
    /// @return The text options used during painting.
    [[nodiscard]] auto textOptions() const noexcept -> const TextOptions &;
    /// Replace the current text rendering options.
    /// @param textOptions The new text options.
    void setTextOptions(const TextOptions &textOptions);
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
    void updatePreferredSize();
    [[nodiscard]] auto measuredTextSize(const LayoutProposal &proposal, const ThemeContext &themeContext) const noexcept
        -> Size;
    [[nodiscard]] auto preferredContentSize() const noexcept -> Size;
    [[nodiscard]] auto contentSizeForWidth(Coordinate width) const noexcept -> Size;

private:
    String _text;                                  ///< The text to be displayed in the text box.
    TextOptions _textOptions;                      ///< The text options for rendering the text.
    std::optional<Coordinate> _preferredLineWidth; ///< Preferred readable content width.
};

}
