// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Surface.hpp"

#include "../../StringView.hpp"

#include <memory>
#include <string_view>

namespace erbsland::cterm::ui::surface {

class StaticText;
using StaticTextPtr = std::shared_ptr<StaticText>;

/// A one-line surface that displays static text and crops it to the assigned width.
class StaticText : public Surface {
public:
    /// Create static text with an initial text and alignment.
    /// @param text The initial text.
    /// @param alignment The alignment used when painting.
    StaticText(const String &text, Alignment alignment, ProtectedTag);
    /// Create static text with a custom theme element.
    /// @param themeElement The theme element for this surface.
    /// @param text The initial text.
    /// @param alignment The alignment used when painting.
    StaticText(theme::Element themeElement, const String &text, Alignment alignment, ProtectedTag);

    // defaults
    ~StaticText() override = default;

public:
    /// Create static text with an initial terminal string.
    /// Newlines and tabs are replaced with spaces.
    /// @param text The initial text.
    /// @param alignment The alignment used when painting.
    /// @return The new static text.
    [[nodiscard]] static auto create(const String &text = {}, Alignment alignment = Alignment::TopLeft)
        -> StaticTextPtr;
    /// Create static text with an initial UTF-8 text.
    /// Newlines and tabs are replaced with spaces.
    /// @param text The initial text.
    /// @param alignment The alignment used when painting.
    /// @return The new static text.
    [[nodiscard]] static auto create(std::string_view text, Alignment alignment = Alignment::TopLeft) -> StaticTextPtr;

public: // accessors
    /// Get the current text.
    [[nodiscard]] auto text() const noexcept -> const String & { return _text; }
    /// Replace the current text.
    /// Newlines and tabs are replaced with spaces.
    /// @param text The new text.
    void setText(String text);
    /// Replace the current text from UTF-8 text.
    /// Newlines and tabs are replaced with spaces.
    /// @param text The new text.
    void setText(std::string_view text);
    /// Replace the current text from UTF-8 text with a uniform style.
    /// Newlines and tabs are replaced with spaces.
    /// @param text The new text.
    /// @param style The style to apply to every character.
    void setText(std::string_view text, CharStyle style);
    /// Get the text alignment.
    [[nodiscard]] auto alignment() const noexcept -> Alignment { return _alignment; }
    /// Set the text alignment.
    /// @param alignment The new alignment.
    void setAlignment(Alignment alignment) noexcept;

public: // implement Surface
    [[nodiscard]] auto onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept
        -> LayoutMetrics override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

protected: // implement Surface
    void initializeUi() override;

private:
    /// Update the natural preferred size from the current text.
    void updatePreferredSize();

private:
    String _text;                           ///< The text to display.
    Alignment _alignment = Alignment::Left; ///< The alignment of the text within the surface.
    theme::Element _themeElement;           ///< The theme element assigned during UI initialization.
};

}
