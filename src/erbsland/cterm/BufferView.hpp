// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Char.hpp"
#include "CropEdges.hpp"
#include "Direction.hpp"
#include "ReadableBuffer.hpp"

namespace erbsland::cterm {

/// The base class for all buffer views.
class BufferViewBase : public ReadableBuffer {
public:
    /// Create a buffer view with the given visible rectangle in the source buffer.
    /// @param viewRectangle The rectangle of the underlying content that shall be exposed through the view.
    explicit BufferViewBase(const Rectangle viewRectangle) noexcept : _viewRect{viewRectangle} {}

    // defaults
    BufferViewBase() = default;
    ~BufferViewBase() override = default;
    BufferViewBase(const BufferViewBase &) = default;
    BufferViewBase(BufferViewBase &&) = default;
    auto operator=(const BufferViewBase &) -> BufferViewBase & = default;
    auto operator=(BufferViewBase &&) -> BufferViewBase & = default;

public: // implement ReadableBuffer
    [[nodiscard]] auto size() const noexcept -> Size override;
    [[nodiscard]] auto rect() const noexcept -> Rectangle override;
    [[nodiscard]] auto clone() const -> WritableBufferPtr override;

public:
    /// Get the rectangle in the underlying content that is currently visible through this view.
    [[nodiscard]] auto viewRect() const noexcept -> const Rectangle &;
    /// Set the rectangle in the underlying content that shall be visible through this view.
    /// @param rect The new source rectangle for the view.
    void setViewRect(Rectangle rect) noexcept;
    /// Test whether crop indicator characters are shown when the view is clipped by the source buffer.
    [[nodiscard]] auto showCropCharacters() const noexcept -> bool;
    /// Enable or disable crop indicator characters.
    /// @param show `true` to render crop indicators inside the view.
    void setShowCropCharacters(bool show) noexcept;
    /// Get the crop indicator character for one direction.
    /// @param direction The direction for which to retrieve the crop indicator.
    /// @return The configured crop character, or an empty character for an invalid direction.
    [[nodiscard]] auto cropCharacter(Direction direction) const noexcept -> Char;
    /// Set the crop indicator character for one direction.
    /// @param direction The direction to update.
    /// @param character The new crop indicator character.
    void setCropCharacter(Direction direction, Char character) noexcept;

protected:
    Rectangle _viewRect{0, 0, 1, 1};
    bool _showCropCharacters = false;
    std::array<Char, Direction::cCount> _cropCharacters{
        Char{U' '},
        Char{U'▲'},
        Char{U'◥'},
        Char{U'▶'},
        Char{U'◢'},
        Char{U'▼'},
        Char{U'◣'},
        Char{U'◀'},
        Char{U'◤'},
    };
};

/// A view that uses a shared pointer to the content.
class BufferView final : public BufferViewBase {
public:
    /// Create an empty view.
    /// This creates a 1x1 view that returns the 'Direction::None' character.
    BufferView() = default;
    /// Create an empty view of a given size.
    /// This creates a view that returns the 'Direction::None' character.
    /// @param viewSize The size of the view.
    explicit BufferView(const Size viewSize) noexcept : BufferViewBase{Rectangle{Position{0, 0}, viewSize}} {};
    /// Create a view of the given content, with a given size.
    /// The view shares the top-left corner with the buffer.
    /// @param content The buffer to create the view from.
    /// @param viewSize The size of the view.
    BufferView(ReadableBufferPtr content, const Size viewSize) noexcept :
        BufferViewBase{Rectangle{Position{0, 0}, viewSize}}, _content{std::move(content)} {}
    /// Create a view of the given content.
    /// @param content The buffer to create the view from.
    /// @param viewRect The rectangle of the view.
    BufferView(ReadableBufferPtr content, const Rectangle viewRect) noexcept :
        BufferViewBase{viewRect}, _content{std::move(content)} {}

    // defaults
    ~BufferView() override = default;
    BufferView(const BufferView &) = default;
    BufferView(BufferView &&) = default;
    auto operator=(const BufferView &) -> BufferView & = default;
    auto operator=(BufferView &&) -> BufferView & = default;

public: // implement ReadableBuffer
    [[nodiscard]] auto get(const Position pos) const noexcept -> const Char & override {
        if (_content == nullptr || _viewRect.size() == Size{0, 0}) {
            return _cropCharacters[Direction::None];
        }
        const auto translatedPos = pos + _viewRect.topLeft();
        if (_content->size().contains(translatedPos)) {
            if (_showCropCharacters) {
                const auto cropEdges = CropEdges::fromView(_viewRect, _content->rect());
                const auto cropDirection = cropEdges.edgeForView(translatedPos, _viewRect);
                if (cropDirection != Direction::None) {
                    return _cropCharacters[cropDirection];
                }
            }
            return _content->get(translatedPos);
        }
        return _cropCharacters[Direction::None];
    }

public:
    /// Access the content.
    [[nodiscard]] auto content() const noexcept -> const ReadableBufferPtr &;
    /// Replace the content.
    void setContent(ReadableBufferPtr buffer) noexcept;

private:
    ReadableBufferPtr _content;
};

/// A view that uses a reference to the content.
/// This view is to use as a thin temporary wrapper on the stack.
class BufferConstRefView final : public BufferViewBase {
public:
    /// Create a view of the given content, with a given size.
    /// The view shares the top-left corner with the buffer.
    /// @param content A reference to the content buffer.
    /// @param viewSize The size of the view.
    BufferConstRefView(const ReadableBuffer &content, const Size viewSize) noexcept :
        BufferViewBase{Rectangle{Position{0, 0}, viewSize}}, _buffer{content} {}
    /// Create a view of the given content.
    /// @param content A reference to the content buffer.
    /// @param viewRect The rectangle of the view.
    BufferConstRefView(const ReadableBuffer &content, const Rectangle viewRect) noexcept :
        BufferViewBase{viewRect}, _buffer{content} {}

    // defaults
    ~BufferConstRefView() override = default;

    // delete copy/assign/move
    BufferConstRefView(const BufferConstRefView &) = delete;
    BufferConstRefView(BufferConstRefView &&) = delete;
    auto operator=(const BufferConstRefView &) -> BufferConstRefView & = delete;
    auto operator=(BufferConstRefView &&) -> BufferConstRefView & = delete;

public: // implement ReadableBuffer
    [[nodiscard]] auto get(const Position pos) const noexcept -> const Char & override {
        if (_viewRect.size() == Size{0, 0}) {
            return _cropCharacters[Direction::None];
        }
        const auto translatedPos = pos + _viewRect.topLeft();
        if (_buffer.size().contains(translatedPos)) {
            if (_showCropCharacters) {
                const auto cropEdges = CropEdges::fromView(_viewRect, _buffer.rect());
                const auto cropDirection = cropEdges.edgeForView(translatedPos, _viewRect);
                if (cropDirection != Direction::None) {
                    return _cropCharacters[cropDirection];
                }
            }
            return _buffer.get(translatedPos);
        }
        return _cropCharacters[Direction::None];
    }

private:
    const ReadableBuffer &_buffer; ///< The reference to the buffer.
};

}
