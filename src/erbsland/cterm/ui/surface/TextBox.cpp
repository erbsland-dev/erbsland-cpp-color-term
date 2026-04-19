// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "TextBox.hpp"

#include <utility>

namespace erbsland::cterm::ui::surface {

TextBox::TextBox(String text, const Alignment alignment, ProtectedTag) :
    Surface{Geometry{
        Size{1, 1},
        Size::maximum(),
        Size{1, 1},
        SizePolicy{DimensionPolicy{DimensionPolicy::Type::Grow}, DimensionPolicy{DimensionPolicy::Type::Preferred}}}},
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
    setPaintOutdated();
}

auto TextBox::textOptions() const noexcept -> const TextOptions & {
    return _textOptions;
}

void TextBox::setTextOptions(const TextOptions &textOptions) {
    _textOptions = textOptions;
    updatePreferredSize();
    setPaintOutdated();
}

void TextBox::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    buffer.drawText(_text, context.targetRect(), _textOptions);
    Surface::onPaint(buffer, context);
}

void TextBox::updatePreferredSize() {
    auto preferredWidth = Coordinate{1};
    auto preferredHeight = Coordinate{1};
    auto currentLineWidth = Coordinate{0};
    for (auto index = std::size_t{0}; index < _text.size(); ++index) {
        const auto &character = _text[index];
        if (character == U'\n') {
            preferredWidth = std::max(preferredWidth, currentLineWidth);
            currentLineWidth = 0;
            if (index + 1 < _text.size()) {
                preferredHeight += 1;
            }
            continue;
        }
        currentLineWidth += character.displayWidth();
    }
    preferredWidth = std::max(preferredWidth, currentLineWidth);
    const auto preferredSize = Size{preferredWidth, preferredHeight};
    if (_geometry.preferred() != preferredSize) {
        _geometry.setPreferred(preferredSize);
        setLayoutOutdated();
    }
}

}
