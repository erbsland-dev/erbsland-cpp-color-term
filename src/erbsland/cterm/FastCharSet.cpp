// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "FastCharSet.hpp"


#include <algorithm>
#include <cstddef>
#include <ranges>


namespace erbsland::cterm {


FastCharSet::FastCharSet(const FastTrack fastTrack, std::u32string characters) noexcept :
    _lookup{fastTrack}, _characters{std::move(characters)} {
}


FastCharSet::FastCharSet(std::u32string characters, Tables tables) noexcept :
    _lookup{std::move(tables)}, _characters{std::move(characters)} {
}


auto FastCharSet::contains(const char32_t character) const noexcept -> bool {
    if (const auto *fastTrack = std::get_if<FastTrack>(&_lookup)) {
        switch (*fastTrack) {
        case FastTrack::Space:
            return character == U' ';
        case FastTrack::SpaceTab:
            return character == U' ' || character == U'\t';
        default:
            return false;
        }
    }
    const auto &tables = std::get<Tables>(_lookup);
    if (character < static_cast<char32_t>(tables.ascii.size())) {
        return tables.ascii[static_cast<std::size_t>(character)];
    }
    return std::ranges::binary_search(tables.nonAscii, character);
}


auto FastCharSet::create(std::u32string characters) -> FastCharSetPtr {
    characters = canonicalize(std::move(characters));
    if (characters == U" ") {
        return onlySpace();
    }
    if (characters == U"\t ") {
        return spaceAndTab();
    }
    auto tables = createCustomTables(characters);
    return FastCharSetPtr{new FastCharSet(std::move(characters), std::move(tables))};
}


auto FastCharSet::onlySpace() -> FastCharSetPtr {
    static const auto cOnlySpace = FastCharSetPtr{new FastCharSet(FastTrack::Space, U" ")};
    return cOnlySpace;
}


auto FastCharSet::spaceAndTab() -> FastCharSetPtr {
    static const auto cSpaceAndTab = FastCharSetPtr{new FastCharSet(FastTrack::SpaceTab, U"\t ")};
    return cSpaceAndTab;
}


auto FastCharSet::canonicalize(std::u32string characters) -> std::u32string {
    std::ranges::sort(characters);
    characters.erase(std::ranges::unique(characters).begin(), characters.end());
    return characters;
}


auto FastCharSet::createCustomTables(const std::u32string &characters) -> Tables {
    auto tables = Tables{};
    for (const auto character : characters) {
        if (character < static_cast<char32_t>(tables.ascii.size())) {
            tables.ascii[static_cast<std::size_t>(character)] = true;
        } else {
            tables.nonAscii.push_back(character);
        }
    }
    return tables;
}


}
