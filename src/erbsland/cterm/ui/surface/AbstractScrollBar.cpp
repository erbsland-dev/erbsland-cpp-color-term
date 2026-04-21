// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "AbstractScrollBar.hpp"

#include "../../theme/ThemePainter.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace erbsland::cterm::ui::surface {

AbstractScrollBar::AbstractScrollBar(
    const Orientation orientation, const LayoutMetrics &layoutSize, ProtectedTag) noexcept :
    Surface{layoutSize}, _orientation{orientation} {
}

auto AbstractScrollBar::scrollRegion() const noexcept -> IndexRange {
    return _scrollRegion;
}

void AbstractScrollBar::setScrollRegion(const IndexRange scrollRegion) noexcept {
    if (_scrollRegion == scrollRegion) {
        return;
    }
    _scrollRegion = scrollRegion;
    flags().setPaintOutdated();
}

auto AbstractScrollBar::visibleRegion() const noexcept -> IndexRange {
    return _visibleRegion;
}

void AbstractScrollBar::setVisibleRegion(const IndexRange visibleRegion) noexcept {
    if (_visibleRegion == visibleRegion) {
        return;
    }
    _visibleRegion = visibleRegion;
    flags().setPaintOutdated();
}

auto AbstractScrollBar::isOpaque() const noexcept -> bool {
    return true;
}

void AbstractScrollBar::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    paintThemed(buffer, context);
}

auto AbstractScrollBar::orientation() const noexcept -> Orientation {
    return _orientation;
}

void AbstractScrollBar::paintThemed(WritableBuffer &buffer, const PaintContext &context) noexcept {
    const auto targetRect = context.surfaceRect();
    if (targetRect.size().isZero()) {
        return;
    }
    const auto trackTheme = context.theme().forPart(theme::Part::Track);
    theme::ThemePainter{buffer, trackTheme}.fill(targetRect);

    const auto contentRect = trackTheme.contentRect(targetRect) & targetRect;
    const auto handleSpan = projectHandle(mainAxisLength(contentRect));
    if (contentRect.size().isZero() || handleSpan.empty()) {
        return;
    }
    theme::ThemePainter{buffer, context.theme().forPart(theme::Part::Thumb)}.fill(
        handleRectForSpan(contentRect, handleSpan));
}

auto AbstractScrollBar::handleRectForSpan(const Rectangle contentRect, const HandleSpan &handleSpan) const noexcept
    -> Rectangle {
    if (_orientation == Orientation::Horizontal) {
        return Rectangle{
            contentRect.x1() + handleSpan.start, contentRect.y1(), handleSpan.length, contentRect.height()};
    }
    return Rectangle{contentRect.x1(), contentRect.y1() + handleSpan.start, contentRect.width(), handleSpan.length};
}

auto AbstractScrollBar::toCoordinate(const std::size_t value) noexcept -> Coordinate {
    const auto maximum = static_cast<std::size_t>(std::numeric_limits<Coordinate>::max());
    return static_cast<Coordinate>(std::min(value, maximum));
}

auto AbstractScrollBar::normalizedScrollRegion() const noexcept -> IndexRange {
    if (_scrollRegion.empty() || _scrollRegion.length() == IndexRange::npos) {
        return {};
    }
    return _scrollRegion;
}

auto AbstractScrollBar::normalizedVisibleRegion() const noexcept -> IndexRange {
    const auto scrollRegion = normalizedScrollRegion();
    if (scrollRegion.empty() || _visibleRegion.empty()) {
        return {};
    }
    const auto scrollStart = scrollRegion.startIndex();
    const auto scrollEnd = scrollRegion.endIndex();
    if (_visibleRegion.startIndex() >= scrollEnd) {
        return {};
    }
    const auto visibleStart = std::max(_visibleRegion.startIndex(), scrollStart);
    const auto visibleEnd =
        _visibleRegion.length() == IndexRange::npos ? scrollEnd : std::min(_visibleRegion.endIndex(), scrollEnd);
    if (visibleEnd <= visibleStart) {
        return {};
    }
    return IndexRange{visibleStart, visibleEnd - visibleStart};
}

auto AbstractScrollBar::mainAxisLength(const Rectangle &rect) const noexcept -> Coordinate {
    if (_orientation == Orientation::Horizontal) {
        return std::max(rect.width(), 0);
    }
    return std::max(rect.height(), 0);
}

auto AbstractScrollBar::projectHandle(const Coordinate effectiveTrackLength) const noexcept -> HandleSpan {
    const auto scrollRegion = normalizedScrollRegion();
    const auto visibleRegion = normalizedVisibleRegion();
    if (effectiveTrackLength <= 0 || scrollRegion.empty() || visibleRegion.empty()) {
        return {};
    }
    const auto scrollLength = scrollRegion.length();
    const auto visibleLength = visibleRegion.length();
    if (visibleLength >= scrollLength) {
        return HandleSpan{0, effectiveTrackLength};
    }
    const auto relativeStart = visibleRegion.startIndex() - scrollRegion.startIndex();
    const auto relativeEnd = visibleRegion.endIndex() - scrollRegion.startIndex();
    const auto trackLengthAsDouble = static_cast<double>(effectiveTrackLength);
    const auto scrollLengthAsDouble = static_cast<double>(scrollLength);
    auto span = HandleSpan{
        .start = static_cast<Coordinate>(
            std::floor((static_cast<double>(relativeStart) * trackLengthAsDouble) / scrollLengthAsDouble)),
        .length = 0,
    };
    auto end = static_cast<Coordinate>(
        std::ceil((static_cast<double>(relativeEnd) * trackLengthAsDouble) / scrollLengthAsDouble));
    span.start = std::clamp(span.start, 0, effectiveTrackLength);
    end = std::clamp(end, 0, effectiveTrackLength);
    if (end <= span.start) {
        end = std::min(span.start + 1, effectiveTrackLength);
    }
    span.length = std::max(end - span.start, 0);
    const auto minimumLength = minimumHandleLength(effectiveTrackLength);
    if (span.length >= minimumLength) {
        return span;
    }
    return expandedHandleSpan(span, effectiveTrackLength, minimumLength);
}

auto AbstractScrollBar::minimumHandleLength(const Coordinate trackLength) noexcept -> Coordinate {
    return std::clamp(trackLength, 0, 3);
}

auto AbstractScrollBar::expandedHandleSpan(
    HandleSpan span, const Coordinate trackLength, const Coordinate desiredLength) noexcept -> HandleSpan {
    if (desiredLength <= span.length) {
        return span;
    }
    auto start = span.start - (desiredLength - span.length) / 2;
    auto end = start + desiredLength;
    if (start < 0) {
        end = std::min(end - start, trackLength);
        start = 0;
    }
    if (end > trackLength) {
        start = std::max(0, start - (end - trackLength));
        end = trackLength;
    }
    span.start = std::clamp(start, 0, trackLength);
    span.length = std::max(end - span.start, 0);
    return span;
}

}
