// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TextBox.hpp"

#include "../../theme/LayoutHelper.hpp"
#include "../../theme/ThemePainter.hpp"

#include <algorithm>
#include <utility>

namespace erbsland::cterm::ui::surface {

TextBox::TextBox(String text, const Alignment alignment, ProtectedTag) :
    Surface{LayoutMetrics{Size{1, 1}, Size::maximum(), Size{1, 1}, SizePolicy{DimensionPolicy::Preferred}}},
    _text(std::move(text)),
    _alignment(alignment) {
    updatePreferredSize();
}

auto TextBox::create(String text, Alignment alignment) -> TextBoxPtr {
    auto result = std::make_shared<TextBox>(std::move(text), alignment, ProtectedTag{});
    result->initializeUi();
    return result;
}

auto TextBox::create(const std::string_view text, const Alignment alignment) -> TextBoxPtr {
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

auto TextBox::alignment() const noexcept -> Alignment {
    return _alignment;
}

void TextBox::setAlignment(const Alignment alignment) {
    _alignment = alignment;
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
    const auto textTheme = scope.theme().forPart(theme::Part::Text);
    const auto textPadding = textTheme.padding();
    auto textSize = preferredTextSize();
    if (proposal.width().hasBound()) {
        const auto proposedWidth = std::max(proposal.width().value(), Coordinate{1});
        const auto availableTextWidth = std::max(proposedWidth - textPadding.horizontalExtent(), Coordinate{1});
        if (proposal.width().isExact() || !_preferredLineWidth.has_value()) {
            textSize = textSizeForWidth(availableTextWidth);
        } else {
            textSize = textSizeForWidth(std::min(textSize.width(), availableTextWidth));
        }
    }

    auto result = layoutMetrics();
    result.setPreferred(textSize + textPadding.extent());
    result.setMargins(textTheme.margins());
    return result;
}

void TextBox::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    const auto textTheme = context.theme().forPart(theme::Part::Text);
    const auto contentRect = context.surfaceRect().insetBy(textTheme.padding());
    theme::ThemePainter{buffer, context.theme()}.fillBackground(context.surfaceRect());
    buffer.drawText(_text, contentRect, _alignment, textTheme.style());
}

void TextBox::initializeUi() {
    Surface::initializeUi();
    themeAttributes().setElement(theme::Element::TextBox);
}

void TextBox::updatePreferredSize() {
    const auto preferredSize = preferredTextSize();
    if (layoutMetrics().preferred() != preferredSize) {
        editLayoutMetrics().setPreferredSize(preferredSize);
    }
}

auto TextBox::preferredTextSize() const noexcept -> Size {
    const auto naturalSize = _text.naturalTextSize();
    if (!_preferredLineWidth.has_value() || naturalSize.width() <= *_preferredLineWidth) {
        return naturalSize;
    }
    return textSizeForWidth(*_preferredLineWidth);
}

auto TextBox::textSizeForWidth(const Coordinate width) const noexcept -> Size {
    const auto textWidth = std::max(width, Coordinate{1});
    return Size{textWidth, WritableBuffer::textHeightForWidth(_text, textWidth, TextOptions{_alignment})};
}

}
