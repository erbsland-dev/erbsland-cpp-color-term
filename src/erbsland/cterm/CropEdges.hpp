// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Direction.hpp"
#include "Rectangle.hpp"

#include <bitset>
#include <cstdint>


namespace erbsland::cterm {

/// Flags for crop edges and corners.
class CropEdges {
public:
    /// Bitset type storing the eight crop-edge directions.
    using Flags = std::bitset<8>;

public:
    /// No crop edges.
    CropEdges() = default;

    // defaults
    ~CropEdges() = default;
    CropEdges(const CropEdges &) = default;
    CropEdges(CropEdges &&) noexcept = default;
    auto operator=(const CropEdges &) -> CropEdges & = default;
    auto operator=(CropEdges &&) -> CropEdges & = default;

public:
    /// Compare two crop-edges objects.
    auto operator==(const CropEdges &) const noexcept -> bool = default;
    /// Compare two crop-edges objects.
    auto operator!=(const CropEdges &) const noexcept -> bool = default;

public:
    /// Get the raw flags.
    [[nodiscard]] auto flags() const noexcept -> Flags { return _flags; }
    /// Test if a crop edge is set.
    [[nodiscard]] auto isSet(const Direction direction) const noexcept -> bool {
        if (direction == Direction::None) {
            return false;
        }
        return _flags.test(indexFromDirection(direction));
    }
    /// Set a crop edge.
    void set(const Direction direction, bool value = true) noexcept {
        if (direction == Direction::None) {
            return;
        }
        _flags.set(indexFromDirection(direction), value);
    }
    /// Clear a crop edge.
    void clear(const Direction direction) noexcept {
        if (direction == Direction::None) {
            return;
        }
        _flags.reset(indexFromDirection(direction));
    }
    /// Reset all crop edges.
    void reset() noexcept { _flags.reset(); }

private:
    [[nodiscard]] auto indexFromDirection(const Direction direction) const noexcept -> std::size_t {
        return std::min(static_cast<std::size_t>(direction) - 1, _flags.size());
    }

public: // tools
    /// Test if a frame position in a view rectangle matches a given crop direction.
    /// This function automatically handles the corners of the view rectangle correctly.
    /// Corner directions, like `Direction::NorthEast` are only returned if `Direction::North` and
    /// `Direction::East` are set. Otherwise, the corner direction matches the main direction.
    [[nodiscard]] auto edgeForView(const Position pos, const Rectangle viewRect) const noexcept -> Direction {
        const auto frameDirection = viewRect.frameDirection(pos);
        if (frameDirection == Direction::None) {
            return Direction::None;
        }
        const bool north = frameDirection.contains(Direction::North) && isSet(Direction::North);
        const bool east = frameDirection.contains(Direction::East) && isSet(Direction::East);
        const bool south = frameDirection.contains(Direction::South) && isSet(Direction::South);
        const bool west = frameDirection.contains(Direction::West) && isSet(Direction::West);

        if (north && east) {
            return Direction::NorthEast;
        }
        if (south && east) {
            return Direction::SouthEast;
        }
        if (south && west) {
            return Direction::SouthWest;
        }
        if (north && west) {
            return Direction::NorthWest;
        }
        if (north) {
            return Direction::North;
        }
        if (east) {
            return Direction::East;
        }
        if (south) {
            return Direction::South;
        }
        if (west) {
            return Direction::West;
        }
        return Direction::None;
    }

    /// Create a crop edge instance for the given view/content situation.
    /// @param viewRect The view rectangle.
    /// @param contentRect The content rectangle.
    [[nodiscard]] static auto fromView(const Rectangle viewRect, const Rectangle contentRect) -> CropEdges {
        CropEdges result;
        const bool north = viewRect.y1() > contentRect.y1();
        const bool east = viewRect.x2() < contentRect.x2();
        const bool south = viewRect.y2() < contentRect.y2();
        const bool west = viewRect.x1() > contentRect.x1();
        const bool northEast = north && east;
        const bool northWest = north && west;
        const bool southEast = south && east;
        const bool southWest = south && west;
        result.set(Direction::North, north);
        result.set(Direction::East, east);
        result.set(Direction::South, south);
        result.set(Direction::West, west);
        result.set(Direction::NorthEast, northEast);
        result.set(Direction::NorthWest, northWest);
        result.set(Direction::SouthEast, southEast);
        result.set(Direction::SouthWest, southWest);
        return result;
    }

private:
    /// The flags for crop edges and corners.
    /// Bits are clockwise, starting with north.
    /// 0:N, 1:NE, 2:E, 3:SE, 4:S, 5:SW, 6:W, 7:NW
    Flags _flags{0};
};

}
