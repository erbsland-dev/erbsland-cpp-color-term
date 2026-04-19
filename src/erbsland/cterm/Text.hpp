// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Rectangle.hpp"
#include "TextOptions.hpp"

#include <utility>

namespace erbsland::cterm {

/// Describes a text block to render into a `Buffer`.
/// @note Creating and copying text instances is expensive. Please keep and reuse created instances.
class Text final {
public:
    /// Create an empty text description.
    Text() = default;
    /// Create a text description for the given content and target rectangle.
    /// @param text The text content to render.
    /// @param rect The target rectangle.
    /// @param alignment The text alignment inside the rectangle.
    Text(String text, Rectangle rect, const Alignment alignment = Alignment::TopLeft) noexcept :
        _text{std::move(text)}, _rectangle{rect}, _textOptions(alignment) {}

    // defaults
    ~Text() = default;
    Text(const Text &) = default;
    Text(Text &&) = default;
    auto operator=(const Text &) -> Text & = default;
    auto operator=(Text &&) -> Text & = default;

public:
    /// Get the text content.
    [[nodiscard]] auto text() const noexcept -> const String & { return _text; }
    /// Set the text content.
    void setText(String text) noexcept { _text = std::move(text); }
    /// Get the target rectangle.
    [[nodiscard]] auto rectangle() const noexcept -> const Rectangle & { return _rectangle; }
    /// Set the target rectangle.
    void setRectangle(const Rectangle rect) noexcept { _rectangle = rect; }
    /// Get the text options.
    [[nodiscard]] auto textOptions() const noexcept -> const TextOptions & { return _textOptions; }
    /// Set the text options.
    void setTextOptions(const TextOptions &options) noexcept { _textOptions = options; }

public: // wrappers around text options.
    /// @copydoc TextOptions::colorSequence
    [[nodiscard]] auto colorSequence() const noexcept -> const ColorSequence & { return _textOptions.colorSequence(); }
    /// @copydoc TextOptions::setColorSequence
    void setColorSequence(ColorSequence colorSequence) noexcept {
        _textOptions.setColorSequence(std::move(colorSequence));
    }
    /// @copydoc TextOptions::color
    [[nodiscard]] auto color() const noexcept -> Color { return _textOptions.color(); }
    /// @copydoc TextOptions::setColor
    void setColor(const Color color) noexcept { _textOptions.setColor(color); }
    /// @copydoc TextOptions::font
    [[nodiscard]] auto font() const noexcept -> const FontPtr & { return _textOptions.font(); }
    /// @copydoc TextOptions::setFont
    void setFont(const FontPtr &font) noexcept { _textOptions.setFont(font); }
    /// @copydoc TextOptions::animation
    [[nodiscard]] auto animation() const noexcept -> TextAnimation { return _textOptions.animation(); }
    /// @copydoc TextOptions::setAnimation
    void setAnimation(const TextAnimation animation) noexcept { _textOptions.setAnimation(animation); }

public: // wrappers for common paragraph options
    /// @copydoc ParagraphOptions::alignment
    [[nodiscard]] auto alignment() const noexcept -> Alignment { return _textOptions.alignment(); }
    /// @copydoc ParagraphOptions::setAlignment
    void setAlignment(const Alignment alignment) noexcept { _textOptions.setAlignment(alignment); }
    /// @copydoc ParagraphOptions::lineIndent
    [[nodiscard]] auto lineIndent() const noexcept -> int { return _textOptions.lineIndent(); }
    /// @copydoc ParagraphOptions::setLineIndent
    void setLineIndent(const int indent) noexcept { _textOptions.setLineIndent(indent); }
    /// @copydoc ParagraphOptions::firstLineIndent
    [[nodiscard]] auto firstLineIndent() const noexcept -> int { return _textOptions.firstLineIndent(); }
    /// @copydoc ParagraphOptions::setFirstLineIndent
    void setFirstLineIndent(const int indent) noexcept { _textOptions.setFirstLineIndent(indent); }
    /// @copydoc ParagraphOptions::wrappedLineIndent
    [[nodiscard]] auto wrappedLineIndent() const noexcept -> int { return _textOptions.wrappedLineIndent(); }
    /// @copydoc ParagraphOptions::setWrappedLineIndent
    void setWrappedLineIndent(const int indent) noexcept { _textOptions.setWrappedLineIndent(indent); }
    /// @copydoc ParagraphOptions::margins
    [[nodiscard]] auto margins() const noexcept -> const Margins & { return _textOptions.margins(); }
    /// @copydoc ParagraphOptions::setMargins
    void setMargins(const Margins margins) noexcept { _textOptions.setMargins(margins); }
    /// @copydoc ParagraphOptions::backgroundMode
    [[nodiscard]] auto backgroundMode() const noexcept -> ParagraphBackgroundMode {
        return _textOptions.backgroundMode();
    }
    /// @copydoc ParagraphOptions::setBackgroundMode
    void setBackgroundMode(const ParagraphBackgroundMode backgroundMode) noexcept {
        _textOptions.setBackgroundMode(backgroundMode);
    }
    /// @copydoc ParagraphOptions::lineBreakEndMark
    [[nodiscard]] auto lineBreakEndMark() const noexcept -> const String & { return _textOptions.lineBreakEndMark(); }
    /// @copydoc ParagraphOptions::setLineBreakEndMark
    void setLineBreakEndMark(String mark) { _textOptions.setLineBreakEndMark(std::move(mark)); }
    /// @copydoc ParagraphOptions::lineBreakStartMark
    [[nodiscard]] auto lineBreakStartMark() const noexcept -> const String & {
        return _textOptions.lineBreakStartMark();
    }
    /// @copydoc ParagraphOptions::setLineBreakStartMark
    void setLineBreakStartMark(String mark) { _textOptions.setLineBreakStartMark(std::move(mark)); }
    /// @copydoc ParagraphOptions::paragraphSpacing
    [[nodiscard]] auto paragraphSpacing() const noexcept -> ParagraphSpacing { return _textOptions.paragraphSpacing(); }
    /// @copydoc ParagraphOptions::setParagraphSpacing
    void setParagraphSpacing(const ParagraphSpacing spacing) noexcept { _textOptions.setParagraphSpacing(spacing); }
    /// @copydoc ParagraphOptions::wordSeparators
    [[nodiscard]] auto wordSeparators() const noexcept -> const std::u32string & {
        return _textOptions.wordSeparators();
    }
    /// @copydoc ParagraphOptions::setWordSeparators
    void setWordSeparators(std::u32string separators) noexcept {
        _textOptions.setWordSeparators(std::move(separators));
    }
    /// @copydoc ParagraphOptions::wordBreakMark
    [[nodiscard]] auto wordBreakMark() const noexcept -> const Char & { return _textOptions.wordBreakMark(); }
    /// @copydoc ParagraphOptions::setWordBreakMark
    void setWordBreakMark(Char mark) noexcept { _textOptions.setWordBreakMark(mark); }
    /// @copydoc ParagraphOptions::maximumLineWraps
    [[nodiscard]] auto maximumLineWraps() const noexcept -> int { return _textOptions.maximumLineWraps(); }
    /// @copydoc ParagraphOptions::setMaximumLineWraps
    void setMaximumLineWraps(const int lines) noexcept { _textOptions.setMaximumLineWraps(std::max(lines, 0)); }
    /// @copydoc ParagraphOptions::paragraphEllipsisMark
    [[nodiscard]] auto paragraphEllipsisMark() const noexcept -> const String & {
        return _textOptions.paragraphEllipsisMark();
    }
    /// @copydoc ParagraphOptions::setParagraphEllipsisMark
    void setParagraphEllipsisMark(String mark) noexcept { _textOptions.setParagraphEllipsisMark(std::move(mark)); }
    /// @copydoc ParagraphOptions::tabStops
    [[nodiscard]] auto tabStops() const noexcept -> const std::vector<int> & { return _textOptions.tabStops(); }
    /// @copydoc ParagraphOptions::setTabStops
    void setTabStops(std::vector<int> tabStops) noexcept { _textOptions.setTabStops(std::move(tabStops)); }
    /// @copydoc ParagraphOptions::tabOverflowBehavior
    [[nodiscard]] auto tabOverflowBehavior() const noexcept -> TabOverflowBehavior {
        return _textOptions.tabOverflowBehavior();
    }
    /// @copydoc ParagraphOptions::setTabOverflowBehavior
    void setTabOverflowBehavior(const TabOverflowBehavior behavior) noexcept {
        _textOptions.setTabOverflowBehavior(behavior);
    }
    /// @copydoc ParagraphOptions::onError
    [[nodiscard]] auto onError() const noexcept -> ParagraphOnError { return _textOptions.onError(); }
    /// @copydoc ParagraphOptions::setOnError
    void setOnError(const ParagraphOnError onError) noexcept { _textOptions.setOnError(onError); }

private:
    String _text{};
    Rectangle _rectangle{};
    TextOptions _textOptions;
};

}
