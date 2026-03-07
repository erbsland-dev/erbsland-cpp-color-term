// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Char16Style.hpp"


#include "impl/U8Buffer.hpp"
#include "impl/UnicodeWidth.hpp"

#include <stdexcept>


namespace erbsland::cterm {

Char16Style::Char16Style(const std::string_view tiles) : _tiles(splitTiles(tiles)) {
}


auto Char16Style::block(const uint32_t bitMask) const noexcept -> Char {
    if (bitMask > 15) {
        return Char{};
    }
    return _tiles[static_cast<std::size_t>(bitMask)];
}

auto Char16Style::lightFrame() -> Char16StylePtr {
    static auto style = styleFromTiles("∙╶╷┌╴─┐┬╵└│├┘┴┤┼");
    return style;
}

auto Char16Style::lightDoubleDashFrame() -> Char16StylePtr {
    static auto style = styleFromTiles("∙╶╷┌╴╌┐┬╵└╎├┘┴┤┼");
    return style;
}


auto Char16Style::lightTripleDashFrame() -> Char16StylePtr {
    static auto style = styleFromTiles("∙╶╷┌╴┄┐┬╵└┆├┘┴┤┼");
    return style;
}


auto Char16Style::lightQuadrupleDashFrame() -> Char16StylePtr {
    static auto style = styleFromTiles("∙╶╷┌╴┈┐┬╵└┊├┘┴┤┼");
    return style;
}


auto Char16Style::lightRoundedFrame() -> Char16StylePtr {
    static auto style = styleFromTiles("∙╶╷╭╴─╮┬╵╰│├╯┴┤┼");
    return style;
}

auto Char16Style::heavyFrame() -> Char16StylePtr {
    static auto style = styleFromTiles("▪╺╻┏╸━┓┳╹┗┃┣┛┻┫╋");
    return style;
}

auto Char16Style::heavyDoubleDashFrame() -> Char16StylePtr {
    static auto style = styleFromTiles("▪╺╻┏╸╍┓┳╹┗╏┣┛┻┫╋");
    return style;
}


auto Char16Style::heavyTripleDashFrame() -> Char16StylePtr {
    static auto style = styleFromTiles("▪╺╻┏╸┅┓┳╹┗┇┣┛┻┫╋");
    return style;
}


auto Char16Style::heavyQuadrupleDashFrame() -> Char16StylePtr {
    static auto style = styleFromTiles("▪╺╻┏╸┉┓┳╹┗┋┣┛┻┫╋");
    return style;
}


auto Char16Style::doubleFrame() -> Char16StylePtr {
    static auto style = styleFromTiles("▫╒╖╔╕═╗╦╜╚║╠╝╩╣╬");
    return style;
}


auto Char16Style::fullBlockFrame() -> Char16StylePtr {
    static auto style = styleFromTiles(" ███████████████");
    return style;
}


auto Char16Style::fullBlockWithChamferFrame() -> Char16StylePtr {
    static auto style = styleFromTiles(" ██◢██◣██◥██◤███");
    return style;
}


auto Char16Style::outerHalfBlockFrame() -> Char16StylePtr {
    static auto style = styleFromTiles(" ▌▀▛▐█▜█▄▙██▟███");
    return style;
}


auto Char16Style::innerHalfBlockFrame() -> Char16StylePtr {
    static auto style = styleFromTiles(" ▐▄▗▌█▖█▀▝██▘███");
    return style;
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
        return outerHalfBlockFrame();
    case FrameStyle::InnerHalfBlock:
        return innerHalfBlockFrame();
    default:
        return lightFrame();
    }
}

auto Char16Style::splitTiles(const std::string_view tiles) -> std::array<Char, 16> {
    auto result = std::array<Char, 16>{};
    auto index = std::size_t{0};
    impl::U8Buffer{tiles}.decodeAll([&](const char32_t codePoint) -> void {
        std::string characterText;
        impl::U8Buffer<const char>::encodeChar(characterText, codePoint);
        if (impl::consoleCharacterWidth(codePoint) == 0 && codePoint >= 0x20 && index > 0) {
            const auto previousIndex = index - 1;
            result[previousIndex] =
                Char{result[previousIndex].charStr() + characterText, result[previousIndex].color()};
            return;
        }
        if (index >= result.size()) {
            throw std::invalid_argument{"Char16Style requires exactly 16 terminal characters."};
        }
        result[index++] = Char{characterText};
    });
    if (index != result.size()) {
        throw std::invalid_argument{"Char16Style requires exactly 16 terminal characters."};
    }
    return result;
}


auto Char16Style::styleFromTiles(const std::string_view tiles) -> Char16StylePtr {
    return std::make_shared<Char16Style>(tiles);
}

}
