// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "WorldMapData.hpp"

#include <charconv>
#include <format>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace demo::ui_world_view {

namespace {

constexpr auto cFormatPrefix = std::string_view{"format ui-world-view-map "};
constexpr auto cSupportedVersion = std::size_t{1};

auto fail(const std::size_t lineNumber, const std::string_view message) -> void {
    throw std::runtime_error{std::format("World map data parse error in line {}: {}", lineNumber, message)};
}

}

auto WorldMapData::loadFromFile(const std::filesystem::path &path) -> WorldMapData {
    auto file = std::ifstream{path, std::ios::binary};
    if (!file) {
        throw std::runtime_error{std::format("Failed to open world map data file: {}", path.string())};
    }
    auto buffer = std::ostringstream{};
    buffer << file.rdbuf();
    return parse(buffer.str());
}

auto WorldMapData::parse(const std::string_view text) -> WorldMapData {
    auto result = WorldMapData{};
    auto stream = std::istringstream{std::string{text}};
    auto line = std::string{};
    auto lineNumber = std::size_t{0};
    auto versionSeen = false;
    auto currentZoomIndex = std::size_t{0};
    auto currentZoom = ZoomLevel{};
    auto inZoom = false;
    auto parsedRows = Coordinate{0};
    auto expectedLabels = std::size_t{0};
    while (std::getline(stream, line)) {
        ++lineNumber;
        auto textLine = trimmed(line);
        if (textLine.empty() || textLine.starts_with('#')) {
            continue;
        }
        if (!versionSeen) {
            if (!textLine.starts_with(cFormatPrefix)) {
                fail(lineNumber, "missing file format header.");
            }
            const auto version = parseNumber(textLine.substr(cFormatPrefix.size()), lineNumber, "version");
            if (version != cSupportedVersion) {
                fail(lineNumber, "unsupported file format version.");
            }
            versionSeen = true;
            continue;
        }
        if (textLine.starts_with("zoom ")) {
            if (inZoom) {
                fail(lineNumber, "nested zoom section.");
            }
            auto zoomStream = std::istringstream{std::string{textLine.substr(5)}};
            auto zoomIndex = std::size_t{};
            auto width = std::size_t{};
            auto height = std::size_t{};
            if (!(zoomStream >> zoomIndex >> width >> height)) {
                fail(lineNumber, "invalid zoom header.");
            }
            if (zoomIndex != currentZoomIndex) {
                fail(lineNumber, "zoom sections must be ordered consecutively.");
            }
            currentZoom = ZoomLevel{
                .size = Size{static_cast<Coordinate>(width), static_cast<Coordinate>(height)},
                .landMask = Bitmap{Size{static_cast<Coordinate>(width), static_cast<Coordinate>(height)}},
                .labels = {}};
            parsedRows = 0;
            expectedLabels = 0;
            inZoom = true;
            ++currentZoomIndex;
            continue;
        }
        if (!inZoom) {
            fail(lineNumber, "content outside a zoom section.");
        }
        if (textLine.starts_with("row ")) {
            if (parsedRows >= currentZoom.size.height()) {
                fail(lineNumber, "too many rows in zoom section.");
            }
            const auto row = parseRow(textLine.substr(4), currentZoom.size.width(), lineNumber);
            for (auto x = Coordinate{0}; x < currentZoom.size.width(); ++x) {
                currentZoom.landMask.setPixel(Position{x, parsedRows}, row[static_cast<std::size_t>(x)]);
            }
            ++parsedRows;
            continue;
        }
        if (textLine.starts_with("labels ")) {
            expectedLabels = parseNumber(textLine.substr(7), lineNumber, "label count");
            currentZoom.labels.reserve(expectedLabels);
            continue;
        }
        if (textLine.starts_with("label ")) {
            auto labelStream = std::istringstream{std::string{textLine.substr(6)}};
            auto x = std::size_t{};
            auto y = std::size_t{};
            if (!(labelStream >> x >> y)) {
                fail(lineNumber, "invalid label header.");
            }
            auto labelText = std::string{};
            std::getline(labelStream, labelText);
            const auto trimmedLabel = trimmed(labelText);
            if (trimmedLabel.empty()) {
                fail(lineNumber, "label text must not be empty.");
            }
            auto textValue = styledLabelText(trimmedLabel);
            currentZoom.labels.push_back(
                Label{
                    .anchor = Position{static_cast<Coordinate>(x), static_cast<Coordinate>(y)},
                    .text = textValue,
                    .displayWidth = static_cast<Coordinate>(textValue.displayWidth())});
            continue;
        }
        if (textLine == "endzoom") {
            if (parsedRows != currentZoom.size.height()) {
                fail(lineNumber, "zoom section does not contain the expected number of rows.");
            }
            if (expectedLabels != currentZoom.labels.size()) {
                fail(lineNumber, "label count does not match the declared number of labels.");
            }
            result._zoomLevels.push_back(std::move(currentZoom));
            inZoom = false;
            continue;
        }
        fail(lineNumber, "unknown statement.");
    }
    if (!versionSeen) {
        throw std::runtime_error{"World map data parse error: missing file format header."};
    }
    if (inZoom) {
        throw std::runtime_error{"World map data parse error: unterminated zoom section."};
    }
    if (result._zoomLevels.empty()) {
        throw std::runtime_error{"World map data parse error: no zoom levels found."};
    }
    return result;
}

auto WorldMapData::zoomLevels() const noexcept -> const std::vector<ZoomLevel> & {
    return _zoomLevels;
}

auto WorldMapData::zoomLevel(const std::size_t zoomIndex) const -> const ZoomLevel & {
    return _zoomLevels.at(zoomIndex);
}

auto WorldMapData::zoomLevelCount() const noexcept -> std::size_t {
    return _zoomLevels.size();
}

auto WorldMapData::parseNumber(const std::string_view text, const std::size_t lineNumber, const std::string_view name)
    -> std::size_t {
    auto value = std::size_t{};
    const auto *begin = text.data();
    const auto *end = begin + text.size();
    if (const auto [ptr, error] = std::from_chars(begin, end, value); error != std::errc{} || ptr != end) {
        fail(lineNumber, std::format("invalid {}.", name));
    }
    return value;
}

auto WorldMapData::parseRow(const std::string_view rowText, const Coordinate width, const std::size_t lineNumber)
    -> std::vector<bool> {
    auto result = std::vector<bool>(static_cast<std::size_t>(width), false);
    auto stream = std::istringstream{std::string{rowText}};
    auto token = std::string{};
    auto x = Coordinate{0};
    auto land = false;
    while (stream >> token) {
        if (token.size() < 2) {
            fail(lineNumber, "invalid row span token.");
        }
        const auto type = token.back();
        if (type != 'S' && type != 'L') {
            fail(lineNumber, "row span token must end in S or L.");
        }
        const auto spanLength = parseNumber(std::string_view{token}.substr(0, token.size() - 1), lineNumber, "span");
        if ((land && type != 'L') || (!land && type != 'S')) {
            fail(lineNumber, "row spans must alternate between sea and land.");
        }
        for (auto index = std::size_t{0}; index < spanLength; ++index) {
            if (x >= width) {
                fail(lineNumber, "row is wider than the declared zoom width.");
            }
            result[static_cast<std::size_t>(x++)] = land;
        }
        land = !land;
    }
    if (x != width) {
        fail(lineNumber, "row width does not match the declared zoom width.");
    }
    return result;
}

auto WorldMapData::trimmed(const std::string_view text) noexcept -> std::string_view {
    const auto first = text.find_first_not_of(" \t\r\n");
    if (first == std::string_view::npos) {
        return {};
    }
    const auto last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

auto WorldMapData::styledLabelText(const std::string_view text) -> String {
    return String{text, CharStyle{Color{fg::BrightWhite, bg::Inherited}}};
}

}
