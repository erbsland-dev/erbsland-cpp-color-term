// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Buffer.hpp"
#include "WritableBuffer.hpp"

#include <array>
#include <cstddef>
#include <utility>

namespace erbsland::cterm {

/// The shared base for writable buffer clipping wrappers.
///
/// A write-clipped buffer exposes a source-coordinate rectangle and maps accepted operations into a target
/// rectangle of another writable buffer. Positions outside the source rectangle are ignored for writes, while reads are
/// translated into the wrapped buffer whenever the translated target position exists.
class WriteClippedBufferBase : public WritableBuffer {
public:
    using WritableBuffer::drawBitmap;
    using WritableBuffer::drawText;
    using WritableBuffer::fill;
    using WritableBuffer::get;
    using WritableBuffer::resize;
    using WritableBuffer::set;

public:
    /// Create an empty write-clipped buffer.
    WriteClippedBufferBase() = default;
    /// Create a write-clipped buffer for the given source offset and target rectangle.
    /// @param sourceOffset The top-left source coordinate exposed by this wrapper.
    /// @param targetRect The rectangle where accepted operations land in the wrapped buffer.
    WriteClippedBufferBase(Position sourceOffset, Rectangle targetRect) noexcept :
        _sourceOffset{sourceOffset}, _targetRect{targetRect} {}

    // defaults
    ~WriteClippedBufferBase() override = default;
    WriteClippedBufferBase(const WriteClippedBufferBase &) = default;
    WriteClippedBufferBase(WriteClippedBufferBase &&) = default;
    auto operator=(const WriteClippedBufferBase &) -> WriteClippedBufferBase & = default;
    auto operator=(WriteClippedBufferBase &&) -> WriteClippedBufferBase & = default;

public: // implement ReadableBuffer
    /// Get the visible source size.
    /// @return The size of the target rectangle.
    [[nodiscard]] auto size() const noexcept -> Size final { return _targetRect.size(); }
    /// Get the visible source rectangle.
    /// @return The source rectangle exposed by this wrapper.
    [[nodiscard]] auto rect() const noexcept -> Rectangle final { return sourceRect(); }
    /// Create a zero-based writable copy of the visible clipped content.
    /// @return A standalone buffer containing the visible clipped content.
    [[nodiscard]] auto clone() const -> WritableBufferPtr final {
        auto result = std::make_shared<Buffer>(size());
        size().forEach([&](const Position pos) -> void { result->set(pos, get(_sourceOffset + pos)); });
        return result;
    }

public: // implement WritableBuffer
    /// Resize the visible source rectangle.
    ///
    /// This only changes the wrapper target size. It never resizes the wrapped buffer.
    /// @param newSize The new visible size.
    void resize(Size newSize) final { _targetRect.setSize(newSize); }
    /// Resize the visible source rectangle.
    ///
    /// The resize mode and fill character are ignored because the wrapped buffer is never resized by this wrapper.
    /// @param newSize The new visible size.
    /// @param mode Ignored.
    /// @param fillChar Ignored.
    void resize(Size newSize, BufferResizeMode mode, Char fillChar) final {
        (void)mode;
        (void)fillChar;
        resize(newSize);
    }
    /// Write a block at the given source position using a combination style.
    /// @param pos The source coordinates.
    /// @param block The block value to store.
    /// @param combinationStyle The combination style for overwriting existing characters.
    void set(Position pos, const Char &block, const CharCombinationStylePtr &combinationStyle) noexcept final {
        if (!rect().contains(pos)) {
            return;
        }
        if (combinationStyle == nullptr) {
            set(pos, block);
            return;
        }
        if (combinationStyle->isSurroundingAware()) {
            std::array<const Char *, 9> surroundingBlocks{};
            for (std::size_t i = 0; i < 9; ++i) {
                const auto surroundPosition =
                    pos + Position{static_cast<Coordinate>(i) % 3 - 1, static_cast<Coordinate>(i) / 3 - 1};
                if (targetBounds().contains(translateToTarget(surroundPosition))) {
                    surroundingBlocks[i] = &get(surroundPosition);
                }
            }
            set(pos, combinationStyle->combine(surroundingBlocks, block));
            return;
        }
        set(pos, combinationStyle->combine(get(pos), block));
    }

public: // accessors
    /// Get the top-left source coordinate exposed by this wrapper.
    /// @return The source offset.
    [[nodiscard]] auto sourceOffset() const noexcept -> const Position & { return _sourceOffset; }
    /// Set the top-left source coordinate exposed by this wrapper.
    /// @param sourceOffset The new source offset.
    void setSourceOffset(Position sourceOffset) noexcept { _sourceOffset = sourceOffset; }
    /// Get the target rectangle in the wrapped buffer.
    /// @return The target rectangle.
    [[nodiscard]] auto targetRect() const noexcept -> const Rectangle & { return _targetRect; }
    /// Set the target rectangle in the wrapped buffer.
    /// @param targetRect The new target rectangle.
    void setTargetRect(Rectangle targetRect) noexcept { _targetRect = targetRect; }
    /// Get the visible source rectangle.
    /// @return The rectangle in source coordinates that maps to the target rectangle.
    [[nodiscard]] auto sourceRect() const noexcept -> Rectangle { return Rectangle{_sourceOffset, _targetRect.size()}; }

protected: // implementation
    /// Implement block filling.
    /// @param rect The source rectangle to fill.
    /// @param fillBlock The block to use for filling.
    /// @param combinationStyle The combination style for overwriting existing characters.
    void
    fillImpl(Rectangle rect, const Char &fillBlock, const CharCombinationStylePtr &combinationStyle) noexcept final {
        (rect & sourceRect()).forEach([&](const Position pos) -> void { set(pos, fillBlock, combinationStyle); });
    }
    /// Implement tile filling.
    /// @param rect The source rectangle to fill.
    /// @param style The tile style to repeat across the rectangle.
    /// @param baseStyle The base style underneath the tile style.
    /// @param combinationStyle The combination style for overwriting existing characters.
    void fillImpl(
        Rectangle rect,
        const Tile9StylePtr &style,
        CharStyle baseStyle,
        const CharCombinationStylePtr &combinationStyle) noexcept final {
        if (style == nullptr) {
            return;
        }
        (rect & sourceRect()).forEach([&](const Position pos) -> void {
            set(pos, style->block(rect, pos).withBase(baseStyle), combinationStyle);
        });
    }
    /// Translate a source position to the wrapped target buffer position.
    /// @param pos The source position.
    /// @return The translated target position.
    [[nodiscard]] auto translateToTarget(Position pos) const noexcept -> Position {
        return _targetRect.topLeft() + pos - _sourceOffset;
    }
    /// Get the wrapped buffer bounds.
    /// @return The bounds for the wrapped buffer, or an empty rectangle when no wrapped buffer is available.
    [[nodiscard]] virtual auto targetBounds() const noexcept -> Rectangle = 0;
    /// Test whether the given block can be written without escaping the source or target rectangle.
    /// @param sourcePos The source position.
    /// @param targetPos The translated target position.
    /// @param block The block to write.
    /// @param targetBounds The wrapped buffer bounds.
    /// @return `true` if the block write is contained by all relevant rectangles.
    [[nodiscard]] auto
    canWriteBlock(Position sourcePos, Position targetPos, const Char &block, Rectangle targetBounds) const noexcept
        -> bool {
        if (!sourceRect().contains(sourcePos) || !targetBounds.contains(targetPos)) {
            return false;
        }
        const auto displayWidth = block.displayWidth();
        if (displayWidth <= 1) {
            return true;
        }
        return sourcePos.x() + displayWidth <= sourceRect().x2() && targetPos.x() + displayWidth <= _targetRect.x2() &&
            targetPos.x() + displayWidth <= targetBounds.x2();
    }

protected:
    Position _sourceOffset;            ///< The top-left source coordinate exposed by this wrapper.
    Rectangle _targetRect{0, 0, 1, 1}; ///< The target rectangle in the wrapped buffer.
};

/// A write-clipped buffer that owns a shared pointer to the wrapped buffer.
class WriteClippedBuffer final : public WriteClippedBufferBase {
public:
    using WriteClippedBufferBase::drawBitmap;
    using WriteClippedBufferBase::drawText;
    using WriteClippedBufferBase::fill;
    using WriteClippedBufferBase::get;
    using WriteClippedBufferBase::resize;
    using WriteClippedBufferBase::set;

public:
    /// Create an empty write-clipped buffer.
    WriteClippedBuffer() = default;
    /// Create an empty write-clipped buffer with a given visible size.
    /// @param size The visible source size.
    explicit WriteClippedBuffer(Size size) noexcept : WriteClippedBufferBase{{}, Rectangle{{}, size}} {}
    /// Create a write-clipped buffer with the given content and visible size.
    /// @param content The wrapped writable buffer.
    /// @param size The visible source size.
    WriteClippedBuffer(WritableBufferPtr content, Size size) noexcept :
        WriteClippedBufferBase{{}, Rectangle{{}, size}}, _content{std::move(content)} {}
    /// Create a write-clipped buffer with the given content and target rectangle.
    /// @param content The wrapped writable buffer.
    /// @param targetRect The target rectangle in the wrapped buffer.
    WriteClippedBuffer(WritableBufferPtr content, Rectangle targetRect) noexcept :
        WriteClippedBufferBase{{}, targetRect}, _content{std::move(content)} {}
    /// Create a write-clipped buffer with the given content, source offset, and target rectangle.
    /// @param content The wrapped writable buffer.
    /// @param sourceOffset The top-left source coordinate exposed by this wrapper.
    /// @param targetRect The target rectangle in the wrapped buffer.
    WriteClippedBuffer(WritableBufferPtr content, Position sourceOffset, Rectangle targetRect) noexcept :
        WriteClippedBufferBase{sourceOffset, targetRect}, _content{std::move(content)} {}

    // defaults
    ~WriteClippedBuffer() override = default;
    WriteClippedBuffer(const WriteClippedBuffer &) = default;
    WriteClippedBuffer(WriteClippedBuffer &&) = default;
    auto operator=(const WriteClippedBuffer &) -> WriteClippedBuffer & = default;
    auto operator=(WriteClippedBuffer &&) -> WriteClippedBuffer & = default;

public: // implement ReadableBuffer
    /// Read a block from the wrapped buffer.
    /// @param pos The source position.
    /// @return The wrapped block, or a space if the translated target position is outside the wrapped buffer.
    [[nodiscard]] auto get(Position pos) const noexcept -> const Char & override {
        if (_content == nullptr) {
            return Char::space();
        }
        const auto targetPos = translateToTarget(pos);
        if (!_content->rect().contains(targetPos)) {
            return Char::space();
        }
        return _content->get(targetPos);
    }

public: // implement WritableBuffer
    /// Write a block into the wrapped buffer.
    /// @param pos The source position.
    /// @param block The block to write.
    void set(Position pos, const Char &block) noexcept override {
        if (_content == nullptr) {
            return;
        }
        const auto targetPos = translateToTarget(pos);
        if (!canWriteBlock(pos, targetPos, block, _content->rect())) {
            return;
        }
        _content->set(targetPos, block);
    }

public:
    /// Access the wrapped writable buffer.
    /// @return The wrapped writable buffer pointer.
    [[nodiscard]] auto content() const noexcept -> const WritableBufferPtr & { return _content; }
    /// Replace the wrapped writable buffer.
    /// @param content The new wrapped writable buffer pointer.
    void setContent(WritableBufferPtr content) noexcept { _content = std::move(content); }

private: // implement WriteClippedBufferBase
    [[nodiscard]] auto targetBounds() const noexcept -> Rectangle override {
        return _content != nullptr ? _content->rect() : Rectangle{};
    }

private:
    WritableBufferPtr _content; ///< The wrapped writable buffer.
};

/// A write-clipped buffer that stores a reference to the wrapped buffer.
///
/// This wrapper is intended as a lightweight temporary object on the stack.
class WriteClippedBufferRef final : public WriteClippedBufferBase {
public:
    using WriteClippedBufferBase::drawBitmap;
    using WriteClippedBufferBase::drawText;
    using WriteClippedBufferBase::fill;
    using WriteClippedBufferBase::get;
    using WriteClippedBufferBase::resize;
    using WriteClippedBufferBase::set;

public:
    /// Create a write-clipped buffer reference with the given visible size.
    /// @param buffer The wrapped writable buffer.
    /// @param size The visible source size.
    WriteClippedBufferRef(WritableBuffer &buffer, Size size) noexcept :
        WriteClippedBufferBase{{}, Rectangle{{}, size}}, _buffer{buffer} {}
    /// Create a write-clipped buffer reference with the given target rectangle.
    /// @param buffer The wrapped writable buffer.
    /// @param targetRect The target rectangle in the wrapped buffer.
    WriteClippedBufferRef(WritableBuffer &buffer, Rectangle targetRect) noexcept :
        WriteClippedBufferBase{{}, targetRect}, _buffer{buffer} {}
    /// Create a write-clipped buffer reference with the given source offset and target rectangle.
    /// @param buffer The wrapped writable buffer.
    /// @param sourceOffset The top-left source coordinate exposed by this wrapper.
    /// @param targetRect The target rectangle in the wrapped buffer.
    WriteClippedBufferRef(WritableBuffer &buffer, Position sourceOffset, Rectangle targetRect) noexcept :
        WriteClippedBufferBase{sourceOffset, targetRect}, _buffer{buffer} {}

    // defaults
    ~WriteClippedBufferRef() override = default;

    // delete copy/assign/move
    WriteClippedBufferRef(const WriteClippedBufferRef &) = delete;
    WriteClippedBufferRef(WriteClippedBufferRef &&) = delete;
    auto operator=(const WriteClippedBufferRef &) -> WriteClippedBufferRef & = delete;
    auto operator=(WriteClippedBufferRef &&) -> WriteClippedBufferRef & = delete;

public: // implement ReadableBuffer
    /// Read a block from the wrapped buffer.
    /// @param pos The source position.
    /// @return The wrapped block, or a space if the translated target position is outside the wrapped buffer.
    [[nodiscard]] auto get(Position pos) const noexcept -> const Char & override {
        const auto targetPos = translateToTarget(pos);
        if (!_buffer.rect().contains(targetPos)) {
            return Char::space();
        }
        return _buffer.get(targetPos);
    }

public: // implement WritableBuffer
    /// Write a block into the wrapped buffer.
    /// @param pos The source position.
    /// @param block The block to write.
    void set(Position pos, const Char &block) noexcept override {
        const auto targetPos = translateToTarget(pos);
        if (!canWriteBlock(pos, targetPos, block, _buffer.rect())) {
            return;
        }
        _buffer.set(targetPos, block);
    }

private: // implement WriteClippedBufferBase
    [[nodiscard]] auto targetBounds() const noexcept -> Rectangle override { return _buffer.rect(); }

private:
    WritableBuffer &_buffer; ///< The wrapped writable buffer.
};

}
