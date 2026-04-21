// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "WorldMapView.hpp"

#include <algorithm>
#include <cmath>
#include <format>

namespace demo::ui_world_view {

WorldMapView::WorldMapView(WorldMapDataPtr mapData, ProtectedTag) noexcept :
    Surface{ui::LayoutMetrics{}}, _mapData{std::move(mapData)} {
    updateLayoutSize();
}

auto WorldMapView::create(WorldMapDataPtr mapData) noexcept -> WorldMapViewPtr {
    return std::make_shared<WorldMapView>(std::move(mapData), ProtectedTag{});
}

auto WorldMapView::zoomIndex() const noexcept -> std::size_t {
    return _zoomIndex;
}

auto WorldMapView::zoomLevelCount() const noexcept -> std::size_t {
    return _mapData != nullptr ? _mapData->zoomLevelCount() : 0;
}

auto WorldMapView::mapSize() const noexcept -> Size {
    if (_mapData == nullptr || _mapData->zoomLevelCount() == 0) {
        return {};
    }
    return _mapData->zoomLevel(_zoomIndex).size;
}

auto WorldMapView::contentSize() const noexcept -> Size {
    const auto size = mapSize();
    return Size{size.width() * 2, size.height()};
}

auto WorldMapView::visibleSourceRect(const Rectangle contentRect) const noexcept -> Rectangle {
    const auto size = mapSize();
    const auto x1 = std::clamp(contentRect.x1() / 2, Coordinate{0}, size.width());
    const auto x2 = std::clamp((contentRect.x2() + 1) / 2, Coordinate{0}, size.width());
    const auto y1 = std::clamp(contentRect.y1(), Coordinate{0}, size.height());
    const auto y2 = std::clamp(contentRect.y2(), Coordinate{0}, size.height());
    return Rectangle{x1, y1, std::max(x2 - x1, Coordinate{0}), std::max(y2 - y1, Coordinate{0})};
}

auto WorldMapView::labelsShown() const noexcept -> bool {
    return _labelsShown;
}

void WorldMapView::setLabelsShown(const bool labelsShown) noexcept {
    if (_labelsShown == labelsShown) {
        return;
    }
    _labelsShown = labelsShown;
    flags().setPaintOutdated();
}

auto WorldMapView::gridShown() const noexcept -> bool {
    return _gridShown;
}

void WorldMapView::setGridShown(const bool gridShown) noexcept {
    if (_gridShown == gridShown) {
        return;
    }
    _gridShown = gridShown;
    flags().setPaintOutdated();
}

auto WorldMapView::zoomIn() noexcept -> bool {
    return setZoomIndex(_zoomIndex + 1);
}

auto WorldMapView::zoomOut() noexcept -> bool {
    if (_zoomIndex == 0) {
        return false;
    }
    return setZoomIndex(_zoomIndex - 1);
}

auto WorldMapView::gridStepForZoomIndex(const std::size_t zoomIndex) noexcept -> Coordinate {
    return zoomIndex >= 2 ? 15 : 30;
}

auto WorldMapView::projectLongitude(const Coordinate longitude, const Coordinate width) noexcept -> Coordinate {
    if (width <= 0) {
        return 0;
    }
    const auto x =
        static_cast<Coordinate>(std::lround(((static_cast<double>(longitude) + 180.0) / 360.0) * (width - 1)));
    return std::clamp(x, 0, width - 1);
}

auto WorldMapView::projectLatitude(const Coordinate latitude, const Coordinate height) noexcept -> Coordinate {
    if (height <= 0) {
        return 0;
    }
    const auto y =
        static_cast<Coordinate>(std::lround(((90.0 - static_cast<double>(latitude)) / 180.0) * (height - 1)));
    return std::clamp(y, 0, height - 1);
}

auto WorldMapView::isOpaque() const noexcept -> bool {
    return true;
}

void WorldMapView::onLayout([[maybe_unused]] ui::LayoutScope &scope) noexcept {
}

void WorldMapView::onPaint(WritableBuffer &buffer, const ui::PaintContext &context) noexcept {
    const auto targetRect = context.visibleRect();
    if (targetRect.size().isZero()) {
        return;
    }
    const auto visibleRect = visibleSourceRect(targetRect);
    drawTerrain(buffer, targetRect, visibleRect);
    if (!visibleRect.size().isZero()) {
        if (_gridShown) {
            drawGrid(buffer, targetRect, visibleRect);
        }
        if (_labelsShown) {
            drawLabels(buffer, targetRect, visibleRect);
        }
        if (_gridShown) {
            drawCoordinateLabels(buffer, targetRect, visibleRect);
        }
    }
}

auto WorldMapView::setZoomIndex(const std::size_t zoomIndex) noexcept -> bool {
    if (zoomIndex >= zoomLevelCount() || zoomIndex == _zoomIndex) {
        return false;
    }
    _zoomIndex = zoomIndex;
    updateLayoutSize();
    flags().setLayoutOutdated();
    flags().setPaintOutdated();
    return true;
}

void WorldMapView::updateLayoutSize() noexcept {
    editLayoutMetrics().setFixedSize(contentSize());
}

void WorldMapView::drawTerrain(
    WritableBuffer &buffer, const Rectangle targetRect, const Rectangle visibleRect) const noexcept {
    static const auto seaCell = Char{U' ', Color{fg::BrightBlue, bg::Blue}};
    static const auto landCell = Char{U' ', Color{fg::BrightGreen, bg::Green}};

    buffer.fill(targetRect, seaCell);
    if (visibleRect.size().isZero()) {
        return;
    }
    const auto &zoomLevel = _mapData->zoomLevel(_zoomIndex);
    visibleRect.size().forEach([&](const Position delta) -> void {
        const auto sourcePos = visibleRect.topLeft() + delta;
        if (!zoomLevel.landMask.pixel(sourcePos)) {
            return;
        }
        const auto x = sourcePos.x() * 2;
        const auto y = sourcePos.y();
        if (targetRect.contains(Position{x, y})) {
            buffer.set(Position{x, y}, landCell);
        }
        if (targetRect.contains(Position{x + 1, y})) {
            buffer.set(Position{x + 1, y}, landCell);
        }
    });
}

void WorldMapView::drawGrid(
    WritableBuffer &buffer, const Rectangle targetRect, const Rectangle visibleRect) const noexcept {
    const auto step = gridStepForZoomIndex(_zoomIndex);
    const auto mapSize = this->mapSize();
    const auto gridStyle = CharCombinationStyle::commonBoxFrame();
    const auto verticalLine = Char{U'│', Color{fg::Inherited, bg::Inherited}};
    const auto horizontalLine = Char{U'─', Color{fg::Inherited, bg::Inherited}};

    for (auto longitude = Coordinate{-150}; longitude <= 150; longitude += step) {
        const auto sourceX = projectLongitude(longitude, mapSize.width());
        if (sourceX < visibleRect.x1() || sourceX >= visibleRect.x2()) {
            continue;
        }
        const auto renderedX = sourceX * 2;
        for (auto y = targetRect.y1(); y < targetRect.y2(); ++y) {
            const auto pos = Position{renderedX, y};
            buffer.set(pos, gridStyle->combine(buffer.get(pos), verticalLine));
        }
    }
    for (auto latitude = Coordinate{-75}; latitude <= 75; ++latitude) {
        if (latitude % step != 0) {
            continue;
        }
        const auto sourceY = projectLatitude(latitude, mapSize.height());
        if (sourceY < visibleRect.y1() || sourceY >= visibleRect.y2()) {
            continue;
        }
        const auto renderedY = sourceY;
        for (auto x = targetRect.x1(); x < targetRect.x2(); ++x) {
            const auto pos = Position{x, renderedY};
            buffer.set(pos, gridStyle->combine(buffer.get(pos), horizontalLine));
        }
    }
}

void WorldMapView::drawCoordinateLabels(
    WritableBuffer &buffer, const Rectangle targetRect, const Rectangle visibleRect) const {
    const auto step = gridStepForZoomIndex(_zoomIndex);
    const auto mapSize = this->mapSize();

    for (auto latitude = Coordinate{-75}; latitude <= 75; ++latitude) {
        if (latitude % step != 0) {
            continue;
        }
        const auto sourceY = projectLatitude(latitude, mapSize.height());
        if (sourceY < visibleRect.y1() || sourceY >= visibleRect.y2()) {
            continue;
        }
        const auto renderedY = sourceY;
        if (renderedY == targetRect.y1()) {
            continue;
        }
        buffer.drawText(Position{targetRect.x1(), renderedY}, formatLatitude(latitude));
    }
    for (auto longitude = Coordinate{-150}; longitude <= 150; longitude += step) {
        const auto sourceX = projectLongitude(longitude, mapSize.width());
        if (sourceX < visibleRect.x1() || sourceX >= visibleRect.x2()) {
            continue;
        }
        const auto renderedX = sourceX * 2;
        const auto text = formatLongitude(longitude);
        const auto startX = renderedX - static_cast<Coordinate>(text.displayWidth()) / 2;
        buffer.drawText(Position{startX, targetRect.y1()}, text);
    }
}

void WorldMapView::drawLabels(WritableBuffer &buffer, const Rectangle targetRect, const Rectangle visibleRect) const {
    const auto &zoomLevel = _mapData->zoomLevel(_zoomIndex);
    for (const auto &label : zoomLevel.labels) {
        if (label.anchor.y() < visibleRect.y1() || label.anchor.y() >= visibleRect.y2()) {
            continue;
        }
        const auto centerX = label.anchor.x() * 2;
        const auto startX = centerX - label.displayWidth / 2;
        const auto endX = startX + label.displayWidth;
        const auto y = label.anchor.y();
        if (y < targetRect.y1() || y >= targetRect.y2()) {
            continue;
        }
        if (endX <= targetRect.x1() || startX >= targetRect.x2()) {
            continue;
        }
        if (_gridShown && intersectsPinnedCoordinateArea(targetRect, startX, endX, y)) {
            continue;
        }
        buffer.drawText(Position{startX, y}, label.text);
    }
}

auto WorldMapView::formatLongitude(const Coordinate longitude) -> String {
    if (longitude == 0) {
        return String{"0", Color{fg::BrightWhite, bg::Black}};
    }
    return String{
        std::format("{}{}", std::abs(longitude), longitude < 0 ? "W" : "E"), Color{fg::BrightWhite, bg::Black}};
}

auto WorldMapView::formatLatitude(const Coordinate latitude) -> String {
    if (latitude == 0) {
        return String{"0", Color{fg::BrightWhite, bg::Black}};
    }
    return String{std::format("{}{}", std::abs(latitude), latitude < 0 ? "S" : "N"), Color{fg::BrightWhite, bg::Black}};
}

auto WorldMapView::intersectsPinnedCoordinateArea(
    const Rectangle targetRect, const Coordinate startX, const Coordinate endX, const Coordinate y) noexcept -> bool {
    if (y < targetRect.y1() + cPinnedTopStripHeight) {
        return true;
    }
    return startX < targetRect.x1() + cPinnedLeftStripWidth && endX > targetRect.x1();
}

}
