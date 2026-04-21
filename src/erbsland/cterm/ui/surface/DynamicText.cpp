// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "DynamicText.hpp"

#include <algorithm>
#include <utility>

namespace erbsland::cterm::ui::surface {

DynamicText::DynamicText(String text, const Alignment alignment, ProtectedTag) :
    Surface{LayoutMetrics{
        Size{1, 1},
        Size::maximum(),
        Size{1, 1},
        SizePolicy{DimensionPolicy{DimensionPolicy::Preferred}, DimensionPolicy{DimensionPolicy::Preferred}}}},
    _text{std::move(text)},
    _textOptions{alignment} {
    editLayoutMetrics().setFixedHeight(1);
    updatePreferredSize();
}

auto DynamicText::create(String text, Alignment alignment) -> DynamicTextPtr {
    return std::make_shared<DynamicText>(std::move(text), alignment, ProtectedTag{});
}

auto DynamicText::create(const std::string_view text, const Alignment alignment) -> DynamicTextPtr {
    return create(String{text, EncodingErrors::Replace}, alignment);
}

void DynamicText::setText(String text) {
    _text = std::move(text);
    updatePreferredSize();
    flags().setPaintOutdated();
}

void DynamicText::setText(const std::string_view text) {
    setText(String{text, EncodingErrors::Replace});
}

void DynamicText::setUpdateMode(const UpdateMode updateMode) noexcept {
    _updateMode = updateMode;
    _lastWidth = std::nullopt;
    flags().setPaintOutdated();
}

void DynamicText::setUpdateFn(UpdateFn updateFn) {
    _updateFn = std::move(updateFn);
    _lastWidth = std::nullopt;
    flags().setPaintOutdated();
}

void DynamicText::clearUpdateFn() noexcept {
    _updateFn = {};
    _lastWidth = std::nullopt;
    flags().setPaintOutdated();
}

void DynamicText::updateForWidth(const Coordinate availableWidth) noexcept {
    if (_updateMode == UpdateMode::Static || !_updateFn) {
        return;
    }
    if (_updateMode == UpdateMode::OnResize && _lastWidth.has_value() && *_lastWidth == availableWidth) {
        return;
    }
    _updateFn(_text, availableWidth);
    _lastWidth = availableWidth;
    updatePreferredSize();
}

auto DynamicText::onMeasure([[maybe_unused]] MeasureScope &scope, const LayoutProposal &proposal) noexcept
    -> LayoutMetrics {
    if (proposal.width().hasBound()) {
        updateForWidth(proposal.width().value());
    }
    return layoutMetrics();
}

void DynamicText::onLayout(LayoutScope &scope) noexcept {
    updateForWidth(std::max(scope.size().width(), 0));
}

void DynamicText::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    buffer.drawText(_text, context.surfaceRect(), _textOptions);
}

void DynamicText::updatePreferredSize() {
    const auto preferredSize = Size{std::max(_text.displayWidth(), 1), 1};
    if (layoutMetrics().preferred() != preferredSize) {
        editLayoutMetrics().setPreferredSize(preferredSize);
    }
}

}
