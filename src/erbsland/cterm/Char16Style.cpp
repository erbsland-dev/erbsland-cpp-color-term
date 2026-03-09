// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Char16Style.hpp"


#include <stdexcept>


namespace erbsland::cterm {

Char16Style::Char16Style(const std::string_view tiles) : _tiles(toTiles(String{tiles})) {
}

Char16Style::Char16Style(const std::u32string_view tiles) : _tiles(toTiles(String{tiles})) {
}


auto Char16Style::block(const uint32_t bitMask) const noexcept -> Char {
    if (bitMask > 15) {
        return Char{};
    }
    return _tiles[static_cast<std::size_t>(bitMask)];
}

auto Char16Style::lightFrame() -> Char16StylePtr {
    static auto style = create("∙╶╷┌╴─┐┬╵└│├┘┴┤┼");
    return style;
}

auto Char16Style::lightDoubleDashFrame() -> Char16StylePtr {
    static auto style = create("∙╶╷┌╴╌┐┬╵└╎├┘┴┤┼");
    return style;
}


auto Char16Style::lightTripleDashFrame() -> Char16StylePtr {
    static auto style = create("∙╶╷┌╴┄┐┬╵└┆├┘┴┤┼");
    return style;
}


auto Char16Style::lightQuadrupleDashFrame() -> Char16StylePtr {
    static auto style = create("∙╶╷┌╴┈┐┬╵└┊├┘┴┤┼");
    return style;
}


auto Char16Style::lightRoundedFrame() -> Char16StylePtr {
    static auto style = create("∙╶╷╭╴─╮┬╵╰│├╯┴┤┼");
    return style;
}

auto Char16Style::heavyFrame() -> Char16StylePtr {
    static auto style = create("▪╺╻┏╸━┓┳╹┗┃┣┛┻┫╋");
    return style;
}

auto Char16Style::heavyDoubleDashFrame() -> Char16StylePtr {
    static auto style = create("▪╺╻┏╸╍┓┳╹┗╏┣┛┻┫╋");
    return style;
}


auto Char16Style::heavyTripleDashFrame() -> Char16StylePtr {
    static auto style = create("▪╺╻┏╸┅┓┳╹┗┇┣┛┻┫╋");
    return style;
}


auto Char16Style::heavyQuadrupleDashFrame() -> Char16StylePtr {
    static auto style = create("▪╺╻┏╸┉┓┳╹┗┋┣┛┻┫╋");
    return style;
}


auto Char16Style::doubleFrame() -> Char16StylePtr {
    static auto style = create("▫╒╖╔╕═╗╦╜╚║╠╝╩╣╬");
    return style;
}


auto Char16Style::fullBlockFrame() -> Char16StylePtr {
    static auto style = create(" ███████████████");
    return style;
}


auto Char16Style::fullBlockWithChamferFrame() -> Char16StylePtr {
    static auto style = create(" ██◢██◣██◥██◤███");
    return style;
}

auto Char16Style::create(const std::string_view tiles) -> Char16StylePtr {
    return std::make_shared<Char16Style>(tiles);
}


auto Char16Style::create(const std::u32string_view tiles) -> Char16StylePtr {
    return std::make_shared<Char16Style>(tiles);
}

auto Char16Style::forStyle(const FrameStyle frameStyle) -> Char16StylePtr {
    switch (frameStyle) {
    case FrameStyle::Light:
        return lightFrame();
    case FrameStyle::LightDoubleDash:
        return lightDoubleDashFrame();
    case FrameStyle::LightTripleDash:
        return lightTripleDashFrame();
    case FrameStyle::LightQuadrupleDash:
        return lightQuadrupleDashFrame();
    case FrameStyle::LightWithRoundedCorners:
        return lightRoundedFrame();
    case FrameStyle::Heavy:
        return heavyFrame();
    case FrameStyle::HeavyDoubleDash:
        return heavyDoubleDashFrame();
    case FrameStyle::HeavyTripleDash:
        return heavyTripleDashFrame();
    case FrameStyle::HeavyQuadrupleDash:
        return heavyQuadrupleDashFrame();
    case FrameStyle::Double:
        return doubleFrame();
    case FrameStyle::FullBlock:
        return fullBlockFrame();
    case FrameStyle::FullBlockWithChamfer:
        return fullBlockWithChamferFrame();
    case FrameStyle::OuterHalfBlock:
    case FrameStyle::InnerHalfBlock:
        return {};
    default:
        return lightFrame();
    }
}

auto Char16Style::toTiles(const String &tiles) -> std::array<Char, 16> {
    if (tiles.size() != 16) {
        throw std::invalid_argument{"Char16Style requires exactly 16 terminal characters."};
    }
    auto result = std::array<Char, 16>{};
    for (std::size_t index = 0; index < result.size(); ++index) {
        result[index] = tiles[index];
    }
    return result;
}

}
