// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Char.hpp"

#include <array>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace erbsland::cterm {

class FastCharSet;
/// Shared pointer for FastCharSet.
using FastCharSetPtr = std::shared_ptr<FastCharSet>;

/// Shared immutable character set with optimized lookup for common separator patterns.
///
/// Instances are intended to be created once and reused across many paragraph layouts.
class FastCharSet final {
public:
    /// Fast-track mode for common separator patterns.
    enum class FastTrack : uint8_t {
        Space,    ///< Match only U+0020 SPACE.
        SpaceTab, ///< Match U+0020 SPACE and U+0009 TAB.
    };

public:
    ~FastCharSet() = default;
    FastCharSet(const FastCharSet &) = delete;
    FastCharSet(FastCharSet &&) = delete;
    auto operator=(const FastCharSet &) -> FastCharSet & = delete;
    auto operator=(FastCharSet &&) -> FastCharSet & = delete;

public:
    /// Access the canonicalized characters of this set.
    /// The returned string is duplicate-free and sorted by code point.
    [[nodiscard]] auto characters() const noexcept -> const std::u32string & { return _characters; }
    /// Test whether the given character is part of the set.
    /// @param character The Unicode code point to test.
    /// @return `true` if `character` is contained in this set.
    [[nodiscard]] auto contains(char32_t character) const noexcept -> bool;
    /// Test whether the given terminal character is part of the set.
    /// Only single-code-point characters can match.
    /// @param character The terminal character to test.
    /// @return `true` if `character` is a single-code-point member of this set.
    [[nodiscard]] auto contains(const Char &character) const noexcept -> bool {
        return contains(character.singleCodePoint());
    }

public:
    /// Create a shared immutable character set for the given characters.
    /// Duplicate characters are removed automatically.
    /// If the given characters match a fast-track pattern, a shared instance is returned.
    /// @param characters The characters to include in the set.
    /// @return A shared immutable character set.
    [[nodiscard]] static auto create(std::u32string characters) -> FastCharSetPtr;
    /// Shared set that only contains U+0020 SPACE.
    [[nodiscard]] static auto onlySpace() -> FastCharSetPtr;
    /// Shared set that contains U+0020 SPACE and U+0009 TAB.
    [[nodiscard]] static auto spaceAndTab() -> FastCharSetPtr;

private:
    /// Lookup tables for custom character sets.
    struct Tables final {
        std::array<bool, 128> ascii{};  ///< Fast ASCII lookup table.
        std::vector<char32_t> nonAscii; ///< Sorted non-ASCII lookup table.
    };

private:
    FastCharSet(FastTrack fastTrack, std::u32string characters) noexcept;
    FastCharSet(std::u32string characters, Tables tables) noexcept;

private:
    [[nodiscard]] static auto canonicalize(std::u32string characters) -> std::u32string;
    [[nodiscard]] static auto createCustomTables(const std::u32string &characters) -> Tables;

private:
    std::variant<FastTrack, Tables> _lookup;
    std::u32string _characters;
};

}
