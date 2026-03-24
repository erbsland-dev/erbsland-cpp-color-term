// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "Orientation.hpp"
#include "WritableBuffer.hpp"

#include <string_view>


namespace erbsland::cterm {

/// A buffer that allows fast remapping/shifting/inserting/deleting of rows and columns.
///
/// This buffer is useful if you have a large buffer that requires frequent row- or column-based manipulations.
/// It is designed for fast insert/delete/move and shift operations.
///
/// This buffer also knows two orientations: vertical and horizontal.
///
/// - In the vertical orientation, the buffer can efficiently grow vertically, keeping the existing data intact.
/// - In the horizontal orientation, the buffer can efficiently grow horizontally, keeping the existing data intact.
/// - When growing the buffer, memory reallocation may be necessary. Use `reserve` to reserve
///   enough memory to avoid frequent reallocations.
/// - Use `resize(size, true, fillChar)` when you need to preserve the visible order while resizing.
///
class RemappedBuffer : public WritableBuffer {
public:
    /// Largest buffer size accepted for this buffer.
    constexpr static auto cMaximumSize = Size{10'000, 10'000};
    /// Smallest valid buffer size.
    constexpr static auto cMinimumSize = Size{1, 1};

    /// A vector mapping one coordinate to another.
    using CoordinateMap = std::vector<Coordinate>;

    using WritableBuffer::drawBitmap;
    using WritableBuffer::drawText;
    using WritableBuffer::fill;
    using WritableBuffer::get;
    using WritableBuffer::resize;
    using WritableBuffer::set;

public:
    /// Creates a 1x1 vertical buffer filled with a space.
    /// Usually only used as a placeholder until resized.
    RemappedBuffer();

    /// Construct a buffer with the given size and fill it with an initial block.
    /// @param size The dimensions of the buffer. Size must be at least 1x1.
    /// @param orientation The orientation of the buffer. Cannot be changed after creation.
    /// @param fillChar The optional fill character for the buffer.
    /// @throws std::invalid_argument if size is invalid
    explicit RemappedBuffer(Size size, Orientation orientation = Orientation::Vertical, Char fillChar = Char::space());

    // defaults
    ~RemappedBuffer() override = default;
    RemappedBuffer(const RemappedBuffer &) = default;
    RemappedBuffer(RemappedBuffer &&) = default;
    auto operator=(const RemappedBuffer &) -> RemappedBuffer & = default;
    auto operator=(RemappedBuffer &&) -> RemappedBuffer & = default;

public: // implement ReadableBuffer
    /// Get the current size of the buffer.
    /// @return The configured width and height.
    [[nodiscard]] auto size() const noexcept -> Size override;
    /// Get the rectangle covering the whole buffer.
    /// @return A rectangle with origin `(0,0)` and the current size.
    [[nodiscard]] auto rect() const noexcept -> Rectangle override;
    /// Read the block stored at the given logical position.
    /// @param pos The logical coordinates inside the buffer.
    /// @return A reference to the stored block, or a shared space block for invalid positions.
    [[nodiscard]] auto get(Position pos) const noexcept -> const Char & override;
    /// Create an independent writable copy of this buffer.
    /// @return A shared pointer to the cloned buffer.
    [[nodiscard]] auto clone() const -> WritableBufferPtr override;

public: // implement WritableBuffer
    /// Resize this buffer.
    ///
    /// This is the fast resize path. The internal mapping is rebuilt and the visible content order is undefined after
    /// the operation. Use `resize(size, true, fillChar)` if you need to preserve the visible order.
    /// @param newSize The new size of the buffer.
    /// @throws std::invalid_argument if `newSize` is invalid.
    void resize(Size newSize) override;
    /// Write a block at the given logical position.
    /// This mirrors the wide-character handling from `Buffer`: zero-width blocks are ignored, width-2 blocks occupy
    /// the next logical cell as an empty continuation cell, and width-2 blocks at the right edge are ignored.
    /// @param pos The logical coordinates within the buffer.
    /// @param block The block to write.
    void set(Position pos, const Char &block) noexcept override;

public:
    /// Resize this buffer and optionally keep the visible content order.
    /// @param size The new size.
    /// @param reorder If `true`, keep the visible order and fill new cells with `fillChar`. If `false`, resize using
    ///   the fast path and leave the visible order undefined.
    /// @param fillChar The fill character for newly created cells in reordered mode. In fast mode it is only used to
    ///   initialize newly appended storage cells.
    /// @throws std::invalid_argument if `size` is invalid.
    void resize(Size size, bool reorder, Char fillChar = Char::space());

public: // manage memory
    /// Reserve memory for the given buffer size.
    /// @param size The size whose capacity should be reserved.
    void reserve(Size size) noexcept;

public: // manipulate the buffer
    /// Shift the buffer in the given direction, fill new cells with a given character.
    /// @param direction The direction of the shift.
    /// @param fillChar The character to fill new cells with.
    /// @param count The number of cells to shift.
    /// @throws std::invalid_argument if `count` is negative or exceeds buffer size.
    void shift(Direction direction, Char fillChar, int count = 1);
    /// @overload
    void shift(const Direction direction, const int count = 1) { shift(direction, Char::space(), count); }
    /// Rotate the buffer in the given direction.
    /// Cells are shifted in a circular manner, wrapping around to the other end of the buffer.
    /// @param direction The direction in which to rotate.
    /// @param count The number of cells to rotate.
    /// @throws std::invalid_argument if `count` is negative or exceeds buffer size.
    void rotate(Direction direction, int count = 1);
    /// Erase rows in the buffer.
    /// This will erase `count` rows, starting from `startRow`, and insert empty ones at the end.
    /// If you like to actually shrink the buffer, use `resize` *after* this call.
    /// @param startRow The first row to delete.
    /// @param fillChar The character to fill new cells with.
    /// @param count The number of rows to delete.
    /// @throws std::invalid_argument if `startRow` is out of bounds or `count` is negative or exceeds buffer size.
    void eraseRows(Coordinate startRow, Char fillChar, int count = 1);
    /// @overload
    void eraseRows(const Coordinate startRow, const int count = 1) { eraseRows(startRow, Char::space(), count); }
    /// Erase columns in the buffer.
    /// This will erase `count` columns, starting from `startColumn`, and insert empty ones at the end.
    /// If you like to actually shrink the buffer, use `resize` *after* this call.
    /// @param startColumn The first column to delete.
    /// @param fillChar The character to fill new cells with.
    /// @param count The number of columns to delete.
    /// @throws std::invalid_argument if `startColumn` is out of bounds or `count` is negative or exceeds buffer size.
    void eraseColumns(Coordinate startColumn, Char fillChar, int count = 1);
    /// @overload
    void eraseColumns(const Coordinate startColumn, const int count = 1) {
        eraseColumns(startColumn, Char::space(), count);
    }
    /// Insert rows in the buffer.
    /// The rows at the bottom of the buffer will be erased to make room for the new rows.
    /// If you like to actually grow the buffer, use `resize` *before* this call.
    /// @param startRow The first row to insert.
    /// @param fillChar The character to fill the new rows with.
    /// @param count The number of rows to insert.
    /// @throws std::invalid_argument if `startRow` is out of bounds or `count` is negative or exceeds buffer size.
    void insertRows(Coordinate startRow, Char fillChar, int count = 1);
    /// @overload
    void insertRows(const Coordinate startRow, const int count = 1) { insertRows(startRow, Char::space(), count); }
    /// Insert columns in the buffer.
    /// The columns on the right side of the buffer will be erased to make room for the new columns.
    /// If you like to actually grow the buffer, use `resize` *before* this call.
    /// @param startColumn The first column to insert.
    /// @param fillChar The character to fill the new columns with.
    /// @param count The number of columns to insert.
    /// @throws std::invalid_argument if `startColumn` is out of bounds or `count` is negative or exceeds buffer size.
    void insertColumns(Coordinate startColumn, Char fillChar, int count = 1);
    /// @overload
    void insertColumns(const Coordinate startColumn, const int count = 1) {
        insertColumns(startColumn, Char::space(), count);
    }
    /// Move rows in the buffer by a given delta.
    /// A positive delta moves rows down, a negative delta moves rows up.
    /// This reshuffles the moved rows, but rows that get moved out of the buffer area are deleted and
    /// replaced by empty cells using the `fillChar`.
    /// @param startRow The first row to move.
    /// @param count The number of rows to move.
    /// @param delta The number of positions to move (positive = down, negative = up).
    /// @param fillChar The character to fill vacated cells with.
    /// @throws std::invalid_argument if `startRow` is out of bounds or `count` is negative or exceeds buffer size.
    void moveRows(Coordinate startRow, int count, Coordinate delta, Char fillChar);
    /// @overload
    void moveRows(const Coordinate startRow, const int count, const Coordinate delta) {
        moveRows(startRow, count, delta, Char::space());
    }
    /// Move columns in the buffer by a given delta.
    /// A positive delta moves columns right, a negative delta moves columns left.
    /// This reshuffles the moved columns, but columns that get moved out of the buffer area are deleted and
    /// replaced by empty cells using the `fillChar`.
    /// @param startColumn The first column to move.
    /// @param count The number of columns to move.
    /// @param delta The number of positions to move (positive = right, negative = left).
    /// @param fillChar The character to fill vacated cells with.
    /// @throws std::invalid_argument if `startColumn` is out of bounds or `count` is negative or exceeds buffer size.
    void moveColumns(Coordinate startColumn, int count, Coordinate delta, Char fillChar);
    /// @overload
    void moveColumns(const Coordinate startColumn, const int count, const Coordinate delta) {
        moveColumns(startColumn, count, delta, Char::space());
    }

public:
    /// Fill/clear the buffer with the given character.
    /// @note This will also reset the internal remapping indexes.
    /// @param fillBlock The block to use to fill the buffer.
    void fill(const Char &fillBlock) noexcept override;

private:
    struct MoveMapResult {
        CoordinateMap reorderedMap; ///< The reordered visible-to-stored mapping.
        CoordinateMap recycled;     ///< Stored coordinates that must be refilled after the operation.
    };

    /// Validate the buffer size.
    /// @throws std::invalid_argument if size is invalid
    [[nodiscard]] static auto validatedBufferSize(Size size) -> Size;
    /// Create a linear index map for the given size.
    /// @param size The size of the buffer.
    /// @return The linear index map.
    [[nodiscard]] static auto linearIndex(std::size_t size) -> CoordinateMap;
    /// Validate a count-only argument.
    /// @param count The count to validate.
    /// @param maximum The maximum accepted count.
    /// @param parameterName The parameter name for error reporting.
    /// @throws std::invalid_argument if the count is invalid.
    static void validateCount(int count, int maximum, std::string_view parameterName);
    /// Validate a span inside the currently visible range.
    /// @param start The first coordinate in the span.
    /// @param count The number of elements in the span.
    /// @param limit The size of the addressable axis.
    /// @param startName The parameter name for the start coordinate.
    /// @param countName The parameter name for the count.
    /// @throws std::invalid_argument if the span is invalid.
    static void validateExistingSpan(
        Coordinate start, int count, int limit, std::string_view startName, std::string_view countName);
    /// Validate an insert-style operation on an axis.
    /// @param start The insertion coordinate.
    /// @param count The number of inserted elements.
    /// @param limit The size of the addressable axis.
    /// @param startName The parameter name for the start coordinate.
    /// @param countName The parameter name for the count.
    /// @throws std::invalid_argument if the arguments are invalid.
    static void validateInsertArguments(
        Coordinate start, int count, int limit, std::string_view startName, std::string_view countName);
    /// Validate a directional count for shift and rotate.
    /// @param direction The direction to validate.
    /// @param count The number of cells to move.
    /// @throws std::invalid_argument if the count does not fit the addressed axis.
    void validateDirectionalCount(Direction direction, int count) const;
    /// Remap the position.
    /// @param pos The position to remap.
    /// @return The remapped position.
    [[nodiscard]] auto remapPosition(Position pos) const noexcept -> Position {
        return rect().contains(pos)
            ? Position{_columnRemap[static_cast<std::size_t>(pos.x())], _rowRemap[static_cast<std::size_t>(pos.y())]}
            : pos;
    }
    /// Get the buffer index for the given orientation and size.
    /// @param pos The stored position.
    /// @param size The size used to linearize the position.
    /// @param orientation The storage orientation.
    /// @return The storage index for `pos`.
    [[nodiscard]] static auto bufferIndex(Position pos, Size size, Orientation orientation) noexcept -> std::size_t;
    /// Get the storage index for the current size and orientation.
    /// @param pos The position for the index.
    /// @return The buffer index.
    [[nodiscard]] auto bufferIndex(Position pos) const noexcept -> std::size_t {
        return bufferIndex(pos, _size, _orientation);
    }
    /// Rotate a coordinate map to the logical front or back.
    /// @param map The map to rotate.
    /// @param count The number of elements to rotate.
    /// @param towardFront If `true`, rotate toward index `0`, otherwise toward the logical end.
    static void rotateMap(CoordinateMap &map, int count, bool towardFront) noexcept;
    /// Erase a span from a coordinate map and append the recycled coordinates at the end.
    /// @param map The map to modify.
    /// @param start The first element to erase.
    /// @param count The number of elements to erase.
    /// @return The recycled coordinates that must be refilled.
    [[nodiscard]] static auto eraseFromMap(CoordinateMap &map, Coordinate start, int count) -> CoordinateMap;
    /// Insert a span into a coordinate map using recycled coordinates from the end.
    /// @param map The map to modify.
    /// @param start The insertion coordinate.
    /// @param count The number of elements to insert.
    /// @return The recycled coordinates that must be refilled.
    [[nodiscard]] static auto insertIntoMap(CoordinateMap &map, Coordinate start, int count) -> CoordinateMap;
    /// Move a span inside a coordinate map.
    /// @param map The map to transform.
    /// @param start The first element to move.
    /// @param count The number of elements to move.
    /// @param delta The movement delta.
    /// @return The reordered map together with the recycled coordinates that must be refilled.
    [[nodiscard]] static auto moveInMap(CoordinateMap map, Coordinate start, int count, Coordinate delta)
        -> MoveMapResult;
    /// Fill the given stored rows.
    /// @param rows The stored row coordinates to fill.
    /// @param fillChar The fill character.
    void fillStoredRows(const CoordinateMap &rows, const Char &fillChar) noexcept;
    /// Fill the given stored columns.
    /// @param columns The stored column coordinates to fill.
    /// @param fillChar The fill character.
    void fillStoredColumns(const CoordinateMap &columns, const Char &fillChar) noexcept;
    /// Execute the fast resize path.
    /// @param newSize The validated new size.
    /// @param fillChar The fill character for newly appended storage cells.
    void fastResize(Size newSize, const Char &fillChar);
    /// Execute the ordered resize path.
    /// @param newSize The validated new size.
    /// @param fillChar The fill character for newly created logical cells.
    void reorderedResize(Size newSize, const Char &fillChar);

private:
    Size _size;                 ///< The current size of the buffer.
    Orientation _orientation;   ///< The orientation of the buffer layout.
    std::vector<Char> _buffer;  ///< The characters in the buffer.
    CoordinateMap _rowRemap;    ///< A map, `_rowRemap[addressed row] -> stored row`
    CoordinateMap _columnRemap; ///< A map, `_columnRemap[addressed column] -> stored column`
};

}
