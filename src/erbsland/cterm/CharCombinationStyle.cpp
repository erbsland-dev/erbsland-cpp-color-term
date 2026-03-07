// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "CharCombinationStyle.hpp"


#include "impl/U8Buffer.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>


namespace erbsland::cterm {


auto CharCombinationStyle::combine([[maybe_unused]] const Char &current, const Char &overlay) const noexcept -> Char {
    return overlay;
}


auto CharCombinationStyle::combine(
    [[maybe_unused]] const std::array<Char *, 9> &current, const Char &overlay) const noexcept -> Char {
    return overlay;
}


auto CharCombinationStyle::overwrite() noexcept -> const CharCombinationStylePtr & {
    static const auto style = std::make_shared<CharCombinationStyle>();
    return style;
}


auto CharCombinationStyle::colorOverlay() noexcept -> const CharCombinationStylePtr & {
    static const CharCombinationStylePtr style = std::make_shared<SimpleCharCombinationStyle>();
    return style;
}


SimpleCharCombinationStyle::SimpleCharCombinationStyle(Map map) noexcept : _map{std::move(map)} {
}


auto SimpleCharCombinationStyle::combine(const Char &current, const Char &overlay) const noexcept -> Char {
    auto result = Char{};
    const auto key = current.charStr() + overlay.charStr();
    if (auto it = map().find(key); it != map().end()) {
        result = Char{it->second};
    } else {
        result = Char{overlay.charStr()};
    }
    return result.withColor(current.color().overlayWith(overlay.color()));
}


auto SimpleCharCombinationStyle::map() const noexcept -> const Map & {
    return _map;
}


void SimpleCharCombinationStyle::setMap(Map map) noexcept {
    _map = std::move(map);
}


void SimpleCharCombinationStyle::add(
    const std::string &current, const std::string &overlay, const std::string &combined) noexcept {
    _map.emplace(current + overlay, combined);
}


MatrixCombinationStyle::MatrixCombinationStyle(std::u32string characters, const std::string_view resultMatrix) :
    _characters{std::move(characters)}, _resultMatrix{resultMatrix} {
    if (_characters.size() > std::numeric_limits<uint8_t>::max()) {
        throw std::invalid_argument{"MatrixCombinationStyle supports at most 255 characters."};
    }
    const auto expectedMatrixSize = _characters.size() * _characters.size();
    if (_resultMatrix.size() != expectedMatrixSize) {
        throw std::invalid_argument{"MatrixCombinationStyle result matrix size does not match the character count."};
    }
    if (_characters.empty()) {
        return;
    }
    const auto [minimumCodePoint, maximumCodePoint] = std::minmax_element(_characters.begin(), _characters.end());
    _lookupBase = *minimumCodePoint;
    const auto lookupSize = static_cast<std::size_t>(*maximumCodePoint - *minimumCodePoint + 1);
    _characterIndexByCodePoint.assign(lookupSize, cUnsupportedIndex);
    for (std::size_t index = 0; index < _characters.size(); ++index) {
        const auto offset = static_cast<std::size_t>(_characters[index] - _lookupBase);
        _characterIndexByCodePoint[offset] = static_cast<uint8_t>(index);
    }
}


auto MatrixCombinationStyle::combine(const Char &current, const Char &overlay) const noexcept -> Char {
    auto result = Char{overlay.charStr()};
    const auto currentIndex = lookupIndex(decodeCodePoint(current.charStr()));
    const auto overlayIndex = lookupIndex(decodeCodePoint(overlay.charStr()));
    if (currentIndex != cUnsupportedIndex && overlayIndex != cUnsupportedIndex) {
        const auto matrixSize = _characters.size();
        const auto matrixIndex =
            static_cast<std::size_t>(currentIndex) * matrixSize + static_cast<std::size_t>(overlayIndex);
        if (matrixIndex < _resultMatrix.size()) {
            const auto resultIndex = static_cast<uint8_t>(_resultMatrix[matrixIndex]);
            if (resultIndex < _characters.size()) {
                result = Char{encodeUtf8(_characters[resultIndex])};
            }
        }
    }
    return result.withColor(current.color().overlayWith(overlay.color()));
}


auto MatrixCombinationStyle::decodeCodePoint(const std::string_view text) noexcept -> char32_t {
    if (text.empty()) {
        return 0;
    }
    try {
        return impl::U8Buffer{text}.decodeOneChar();
    } catch ([[maybe_unused]] const std::invalid_argument &) {
        return 0;
    }
}


auto MatrixCombinationStyle::encodeUtf8(const char32_t codePoint) noexcept -> std::string {
    auto result = std::string{};
    impl::U8Buffer<const char>::encodeChar(result, codePoint);
    return result;
}


auto MatrixCombinationStyle::lookupIndex(const char32_t codePoint) const noexcept -> uint8_t {
    if (_characterIndexByCodePoint.empty() || codePoint < _lookupBase) {
        return cUnsupportedIndex;
    }
    const auto offset = static_cast<std::size_t>(codePoint - _lookupBase);
    if (offset >= _characterIndexByCodePoint.size()) {
        return cUnsupportedIndex;
    }
    return _characterIndexByCodePoint[offset];
}


}
