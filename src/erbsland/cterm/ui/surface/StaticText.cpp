// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "StaticText.hpp"

#include "../../theme/LayoutHelper.hpp"

#include <utility>

namespace erbsland::cterm::ui::surface {

StaticText::StaticText(const String &text, const Alignment alignment, ProtectedTag protectedTag) :
    StaticText{theme::Element::StaticText, text, alignment, protectedTag} {
}

StaticText::StaticText(const theme::Element themeElement, const String &text, const Alignment alignment, ProtectedTag) :
    Surface{LayoutMetrics{
        Size{0, 1},
        Size::maximum(),
        Size{0, 1},
        SizePolicy{DimensionPolicy{DimensionPolicy::Preferred}, DimensionPolicy{DimensionPolicy::Preferred}}}},
    _text{text.normalized(U"\n\t")},
    _alignment{alignment},
    _themeElement{themeElement} {
    editLayoutMetrics().setFixedHeight(1);
    updatePreferredSize();
}

auto StaticText::create(const String &text, Alignment alignment) -> StaticTextPtr {
    auto result = std::make_shared<StaticText>(text, alignment, ProtectedTag{});
    result->initializeUi();
    return result;
}

auto StaticText::create(const std::string_view text, const Alignment alignment) -> StaticTextPtr {
    return create(String{text, EncodingErrors::Replace}, alignment);
}

void StaticText::setText(String text) {
    if (text.containsControlCharacters()) {
        text.normalize(U"\n\t"); // make sure there is only one line.
    }
    const auto oldDisplayWidth = _text.displayWidth();
    _text = std::move(text);
    if (_text.displayWidth() != oldDisplayWidth) {
        updatePreferredSize();
    }
    flags().setPaintOutdated();
}

void StaticText::setText(const std::string_view text) {
    setText(String{text, EncodingErrors::Replace});
}

void StaticText::setText(const std::string_view text, const CharStyle style) {
    setText(String{text, style, EncodingErrors::Replace});
}

void StaticText::setAlignment(const Alignment alignment) noexcept {
    _alignment = alignment;
    flags().setPaintOutdated();
}

auto StaticText::onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept -> LayoutMetrics {
    auto result = layoutMetrics();
    const auto textTheme = scope.theme().forPart(theme::Part::Text);
    auto preferredSize = Size{_text.displayWidth(), 1};
    preferredSize.add(textTheme.horizontalPadding().extent(), Orientation::Horizontal);
    if (proposal.width().hasBound()) {
        preferredSize.limitTo(Size{proposal.width().value(), 1}, Orientation::Horizontal);
    }
    result.setPreferred(preferredSize);
    result.setMargins(textTheme.horizontalMargins());
    return result;
}

void StaticText::initializeUi() {
    Surface::initializeUi();
    themeAttributes().setElement(_themeElement);
}

void StaticText::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    const auto text = theme::layout::stylePaddingCropAndMargins(
        _text, context.surfaceRect().width(), context.theme(), theme::Part::Text, theme::Part::Ellipsis, _alignment);
    if (text.text().empty()) {
        return;
    }
    buffer.drawText(text.text(), context.surfaceRect(), _alignment);
}

void StaticText::updatePreferredSize() {
    const auto preferredSize = Size{_text.displayWidth(), 1};
    if (layoutMetrics().preferred() != preferredSize) {
        editLayoutMetrics().setPreferredSize(preferredSize);
    }
}

}
