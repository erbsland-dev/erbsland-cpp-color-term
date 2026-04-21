// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TextBox.hpp"

#include <utility>

namespace erbsland::cterm::ui::surface {

TextBox::TextBox(String text, const Alignment alignment, ProtectedTag) :
    Surface{
        theme::Element::TextBox,
        LayoutMetrics{Size{1, 1}, Size::maximum(), Size{1, 1}, SizePolicy{DimensionPolicy::Preferred}}},
    _text(std::move(text)),
    _textOptions(alignment) {
    updatePreferredSize();
}

auto TextBox::create(String text, Alignment alignment) noexcept -> TextBoxPtr {
    return std::make_shared<TextBox>(std::move(text), alignment, ProtectedTag{});
}

auto TextBox::create(std::string_view text, Alignment alignment) noexcept -> TextBoxPtr {
    return create(String{text, EncodingErrors::Replace}, alignment);
}

auto TextBox::text() const noexcept -> const String & {
    return _text;
}

void TextBox::setText(String text) {
    _text = std::move(text);
    updatePreferredSize();
    flags().setPaintOutdated();
}

auto TextBox::textOptions() const noexcept -> const TextOptions & {
    return _textOptions;
}

void TextBox::setTextOptions(const TextOptions &textOptions) {
    _textOptions = textOptions;
    updatePreferredSize();
    flags().setPaintOutdated();
}

auto TextBox::preferredLineWidth() const noexcept -> std::optional<Coordinate> {
    return _preferredLineWidth;
}

void TextBox::setPreferredLineWidth(std::optional<Coordinate> width) {
    if (width.has_value()) {
        width = std::max(*width, Coordinate{1});
    }
    if (_preferredLineWidth == width) {
        return;
    }
    _preferredLineWidth = width;
    updatePreferredSize();
    flags().setPaintOutdated();
}

auto TextBox::onMeasure([[maybe_unused]] MeasureScope &scope, const LayoutProposal &proposal) noexcept
    -> LayoutMetrics {
    auto result = layoutMetrics();
    result.setPreferred(measuredTextSize(proposal, scope.themeContext()));
    return result;
}

void TextBox::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    const auto textTheme = context.theme().forPart(theme::Part::Text);
    buffer.fill(Char::space().withStyleReplaced(textTheme.style()));
    buffer.drawText(_text, textTheme.contentRect(context.surfaceRect()), _textOptions);
}

void TextBox::updatePreferredSize() {
    const auto preferredSize = preferredContentSize();
    if (layoutMetrics().preferred() != preferredSize) {
        editLayoutMetrics().setPreferredSize(preferredSize);
    }
}

auto TextBox::measuredTextSize(const LayoutProposal &proposal, const ThemeContext &themeContext) const noexcept
    -> Size {
    const auto textMargins = themeContext.theme().forPart(theme::Part::Text).margins();
    const auto marginExtent = textMargins.extent();
    if (proposal.width().isUnspecified()) {
        return preferredContentSize() + marginExtent;
    }
    const auto proposedWidth = std::max(proposal.width().value(), Coordinate{1});
    const auto contentWidthLimit = std::max(proposedWidth - marginExtent.width(), Coordinate{1});
    auto contentWidth = contentWidthLimit;
    if (proposal.width().isAtMost() && _preferredLineWidth.has_value()) {
        contentWidth = std::min(preferredContentSize().width(), contentWidthLimit);
    }
    return contentSizeForWidth(contentWidth) + marginExtent;
}

auto TextBox::preferredContentSize() const noexcept -> Size {
    const auto naturalSize = _text.naturalTextSize();
    if (!_preferredLineWidth.has_value()) {
        return naturalSize;
    }
    return contentSizeForWidth(std::min(naturalSize.width(), *_preferredLineWidth));
}

auto TextBox::contentSizeForWidth(const Coordinate width) const noexcept -> Size {
    const auto contentWidth = std::max(width, Coordinate{1});
    return Size{contentWidth, WritableBuffer::textHeightForWidth(_text, contentWidth, _textOptions)};
}

}
