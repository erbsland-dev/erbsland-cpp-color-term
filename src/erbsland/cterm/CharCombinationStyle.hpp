// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Char.hpp"

#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <string_view>
#include <vector>


namespace erbsland::cterm {


class CharCombinationStyle;
/// Shared pointer for CharCombinationStyle
using CharCombinationStylePtr = std::shared_ptr<CharCombinationStyle>;


/// A style how two characters are visually combined to a new one.
class CharCombinationStyle {
public:
    virtual ~CharCombinationStyle() = default;

public:
    /// Does this style take the surrounding character into account?
    /// If this method returns false, `combine` with current as a single character is called.
    /// If this method returns true, `combine` with current and overlay is called.
    [[nodiscard]] virtual auto isSurroundingAware() const noexcept -> bool { return false; }

    /// Combines the current char with a new that is placed on top of the current one.
    /// The default implementation just returns the overlay character.
    /// @param current The current (lower) character.
    /// @param overlay The new (upper) character that overlays the current one.
    /// @return The combined character.
    [[nodiscard]] virtual auto combine(const Char &current, const Char &overlay) const noexcept -> Char;

    /// Combines the current character situation with a new overlay character that is placed on top of the current one.
    /// The default implementation just returns the overlay character.
    /// The matrix starts at (-1, -1) and ends at (1, 1) (left to right, top to bottom).
    /// @param current A 3x3 matrix with nullptr for locations outside the buffer.
    /// @param overlay The new (upper) character that overlays the current one.
    /// @return The combined character.
    [[nodiscard]] virtual auto combine(const std::array<Char *, 9> &current, const Char &overlay) const noexcept
        -> Char;

public: // predefined styles
    /// Overwrite the character and color.
    [[nodiscard]] static auto overwrite() noexcept -> const CharCombinationStylePtr &;
    /// Overwrite the character but overlay the color.
    [[nodiscard]] static auto colorOverlay() noexcept -> const CharCombinationStylePtr &;
    /// Combine light, double, and heavy frames.
    /// In that order: double overwrites light, and heavy overwrites double and light.
    /// Colors are overlay.
    [[nodiscard]] static auto commonBoxFrame() noexcept -> const CharCombinationStylePtr &;
};


/// A class to use a simple map to combine styles.
/// It uses a map of [current, overlay] -> combined and uses `Color::overlayWith` to combine the colors.
/// If a character is missing in the map, the overlay overwrites the current one.
class SimpleCharCombinationStyle : public CharCombinationStyle {
public:
    /// The map, where the key is `<current>+<overlay>`, and the value is the combined character.
    using Map = std::map<std::string, std::string>;

public:
    /// Create an empty combination style.
    SimpleCharCombinationStyle() = default;
    /// Create a new instance from the given map.
    explicit SimpleCharCombinationStyle(Map map) noexcept;

public: // implement CharCombinationStyle
    [[nodiscard]] auto combine(const Char &current, const Char &overlay) const noexcept -> Char override;

public:
    /// Access the current map.
    [[nodiscard]] auto map() const noexcept -> const Map &;
    /// Replace the map.
    void setMap(Map map) noexcept;
    /// Add a new entry to the map.
    void add(const std::string &current, const std::string &overlay, const std::string &combined) noexcept;

private:
    Map _map;
};


/// A class that combines characters through an indexed result matrix.
/// It uses supported Unicode code points and a compact byte matrix of result indexes.
/// Unknown characters fall back to the overlay character.
class MatrixCombinationStyle : public CharCombinationStyle {
public:
    /// Create a new matrix-based combination style.
    /// @param characters The supported Unicode characters in matrix index order.
    /// @param resultMatrix The result matrix in row-major order using result indexes.
    /// The matrix size must be `characters.size() * characters.size()`.
    /// @throws std::invalid_argument If the matrix size is invalid or the character count exceeds 255.
    MatrixCombinationStyle(std::u32string characters, std::string_view resultMatrix);

public: // implement CharCombinationStyle
    [[nodiscard]] auto combine(const Char &current, const Char &overlay) const noexcept -> Char override;

private:
    /// Resolve one code point to its matrix index.
    /// @param codePoint The Unicode code point to resolve.
    /// @return The matrix index, or 255 if the code point is unsupported.
    [[nodiscard]] auto lookupIndex(char32_t codePoint) const noexcept -> uint8_t;

private:
    static constexpr auto cUnsupportedIndex = uint8_t{0xFFU};

private:
    std::u32string _characters;
    std::string _resultMatrix;
    char32_t _lookupBase{0};
    std::vector<uint8_t> _characterIndexByCodePoint;
};


}
