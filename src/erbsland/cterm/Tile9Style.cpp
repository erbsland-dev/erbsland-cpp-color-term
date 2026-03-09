// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Tile9Style.hpp"


#include <algorithm>
#include <stdexcept>


namespace erbsland::cterm {

Tile9Style::Tile9Style(const std::array<Char, 9> tiles) noexcept : Tile9Style(toParsedTiles(tiles)) {
}

Tile9Style::Tile9Style(const std::array<Char, 16> tiles) noexcept : Tile9Style(toParsedTiles(tiles)) {
}

Tile9Style::Tile9Style(const std::string_view tiles) : Tile9Style(parseTiles(String{tiles})) {
}

Tile9Style::Tile9Style(const std::u32string_view tiles) : Tile9Style(parseTiles(String{tiles})) {
}


auto Tile9Style::block(const Rectangle rect, const Position pos) const noexcept -> Char {
    if (!rect.contains(pos)) {
        return Char{};
    }
    if (rect.width() == 1 && rect.height() == 1) {
        return _hasExtendedTiles ? _tiles[15] : _tiles[0];
    }
    if (rect.height() == 1) {
        if (_hasExtendedTiles) {
            if (pos.x() == rect.x1()) {
                return _tiles[9];
            }
            if (pos.x() == rect.x2() - 1) {
                return _tiles[11];
            }
            return _tiles[10];
        }
        if (pos.x() == rect.x1()) {
            return _tiles[0];
        }
        if (pos.x() == rect.x2() - 1) {
            return _tiles[2];
        }
        return _tiles[1];
    }
    if (rect.width() == 1) {
        if (_hasExtendedTiles) {
            if (pos.y() == rect.y1()) {
                return _tiles[12];
            }
            if (pos.y() == rect.y2() - 1) {
                return _tiles[14];
            }
            return _tiles[13];
        }
        if (pos.y() == rect.y1()) {
            return _tiles[0];
        }
        if (pos.y() == rect.y2() - 1) {
            return _tiles[6];
        }
        return _tiles[3];
    }
    const auto row = pos.y() == rect.y1() ? 0 : pos.y() == rect.y2() - 1 ? 2 : 1;
    const auto column = pos.x() == rect.x1() ? 0 : pos.x() == rect.x2() - 1 ? 2 : 1;
    return _tiles[static_cast<std::size_t>(row * 3 + column)];
}

auto Tile9Style::create(const std::string_view tiles) -> Tile9StylePtr {
    return std::make_shared<Tile9Style>(tiles);
}

auto Tile9Style::create(const std::u32string_view tiles) -> Tile9StylePtr {
    return std::make_shared<Tile9Style>(tiles);
}

auto Tile9Style::outerHalfBlockFrame() -> Tile9StylePtr {
    static auto style = create("▛▀▜▌ ▐▙▄▟███████");
    return style;
}

auto Tile9Style::innerHalfBlockFrame() -> Tile9StylePtr {
    static auto style = create("▗▄▖▐ ▌▝▀▘███████");
    return style;
}

auto Tile9Style::forStyle(const FrameStyle frameStyle) -> Tile9StylePtr {
    switch (frameStyle) {
    case FrameStyle::OuterHalfBlock:
        return outerHalfBlockFrame();
    case FrameStyle::InnerHalfBlock:
        return innerHalfBlockFrame();
    default:
        return {};
    }
}

Tile9Style::Tile9Style(const ParsedTiles &parsed) noexcept :
    _tiles{parsed.tiles}, _hasExtendedTiles{parsed.hasExtendedTiles} {
}

auto Tile9Style::parseTiles(const String &tiles) -> ParsedTiles {
    if (tiles.size() != 9 && tiles.size() != 16) {
        throw std::invalid_argument{"Tile9Style requires exactly 9 or 16 terminal characters."};
    }
    auto result = ParsedTiles{};
    result.hasExtendedTiles = tiles.size() == 16;
    for (std::size_t index = 0; index < tiles.size(); ++index) {
        result.tiles[index] = tiles[index];
    }
    return result;
}

auto Tile9Style::toParsedTiles(const std::array<Char, 9> &tiles) noexcept -> ParsedTiles {
    auto result = ParsedTiles{};
    std::copy_n(tiles.begin(), tiles.size(), result.tiles.begin());
    return result;
}

auto Tile9Style::toParsedTiles(const std::array<Char, 16> &tiles) noexcept -> ParsedTiles {
    return ParsedTiles{.tiles = tiles, .hasExtendedTiles = true};
}

}
