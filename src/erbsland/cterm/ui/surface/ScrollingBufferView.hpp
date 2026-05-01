// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "AbstractScrollArea.hpp"

#include "../../BufferView.hpp"

#include <memory>

namespace erbsland::cterm::ui::surface {

class ScrollingBufferView;
using ScrollingBufferViewPtr = std::shared_ptr<ScrollingBufferView>;

/// A surface that renders a scrollable viewport into one readable buffer.
class ScrollingBufferView final : public AbstractScrollArea {
public:
    /// Create a scrolling buffer view.
    /// @param source The optional source buffer.
    explicit ScrollingBufferView(ReadableBufferPtr source, ProtectedTag) noexcept;
    ~ScrollingBufferView() override = default;

public:
    /// Create a scrolling buffer view for one source buffer.
    /// @param source The optional source buffer.
    /// @return The new scrolling buffer view.
    [[nodiscard]] static auto create(ReadableBufferPtr source = {}) -> ScrollingBufferViewPtr;

public:
    /// Access the source buffer.
    /// @return The current source buffer.
    [[nodiscard]] auto source() const noexcept -> const ReadableBufferPtr &;
    /// Replace the source buffer and clamp the stored view offset to the new content.
    /// @param source The new source buffer.
    void setSource(ReadableBufferPtr source) noexcept;
    /// Test whether crop indicator characters are shown.
    /// @return `true` if crop indicator characters are shown.
    [[nodiscard]] auto showCropCharacters() const noexcept -> bool;
    /// Enable or disable crop indicator characters.
    /// @param show `true` to show crop indicator characters.
    void setShowCropCharacters(bool show) noexcept;
    /// Get one crop indicator character.
    /// @param direction The crop direction to query.
    /// @return The configured crop indicator character.
    [[nodiscard]] auto cropCharacter(Direction direction) const noexcept -> Char;
    /// Replace one crop indicator character.
    /// @param direction The crop direction to update.
    /// @param character The new crop indicator character.
    void setCropCharacter(Direction direction, Char character) noexcept;

protected: // implement AbstractScrollArea
    [[nodiscard]] auto contentSizeForViewport(Size viewportSize) const noexcept -> Size override;
    [[nodiscard]] auto measureContentSizeForViewport(Size viewportSize, LayoutScope &scope) noexcept -> Size override;
    void onPaintArea(
        Position scrollOffset,
        Rectangle targetRect,
        WritableBuffer &buffer,
        const PaintContext &context) noexcept override;

private:
    ReadableBufferPtr _source; ///< The source buffer.
    BufferView _view;          ///< The cached source-buffer view.
};

}
