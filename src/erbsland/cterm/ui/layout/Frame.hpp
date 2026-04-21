// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "SingleContentLayout.hpp"

namespace erbsland::cterm::ui::layout {

class Frame;
using FramePtr = std::shared_ptr<Frame>;

/// A one-child layout that surrounds its content with a themed frame and optional title.
class Frame final : public SingleContentLayout {
public:
    /// Create a frame layout.
    explicit Frame(ProtectedTag) noexcept;
    ~Frame() override = default;

public:
    /// Create a frame layout.
    /// @return The new frame layout.
    [[nodiscard]] static auto create() -> FramePtr;

public:
    /// Access the frame title.
    /// @return The current frame title.
    [[nodiscard]] auto title() const noexcept -> const String &;
    /// Replace the frame title.
    /// @param title The new title.
    void setTitle(String title);
    /// Access the padding between frame and content.
    /// @return The padding.
    [[nodiscard]] auto padding() const noexcept -> Margins;
    /// Replace the padding between frame and content.
    /// @param padding The new padding.
    void setPadding(Margins padding) noexcept;

public: // implement Surface
    [[nodiscard]] auto onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept
        -> LayoutMetrics override;
    void onLayout(LayoutScope &scope) noexcept override;
    [[nodiscard]] auto isOpaque() const noexcept -> bool override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

private:
    /// Calculate the size consumed by the frame border and padding.
    [[nodiscard]] static auto decorationSize(Margins padding) noexcept -> Size;
    /// Calculate the content rectangle for a surface size.
    [[nodiscard]] auto contentRect(Size size) const noexcept -> Rectangle;
    /// Create a child proposal by removing border and padding.
    [[nodiscard]] static auto contentProposal(LayoutProposal proposal, Margins padding) noexcept -> LayoutProposal;
    /// Add frame and padding to content metrics.
    [[nodiscard]] auto framedMetrics(const LayoutMetrics &contentMetrics, const ThemeContext &themeContext) const
        -> LayoutMetrics;
    /// Build the rendered title block.
    [[nodiscard]] auto titleBlock(const ThemeContext &themeContext) const -> String;
    /// Draw the title into the top border.
    void drawTitle(WritableBuffer &buffer, const PaintContext &context, Rectangle frameRect) const;

private:
    String _title;                ///< The optional frame title.
    Margins _padding{1, 2, 1, 2}; ///< Padding between frame and content.
};

}
