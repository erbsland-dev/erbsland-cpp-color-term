// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ParagraphLayout.hpp"

#include "../ParagraphBackgroundMode.hpp"
#include "../WritableBuffer.hpp"

#include <functional>
#include <optional>


namespace erbsland::cterm::impl {


/// Paint a shared paragraph layout into a writable buffer.
class ParagraphPainter final {
public:
    using ColorResolver = std::function<Color(const Char &, Position)>;

public:
    ParagraphPainter(
        WritableBuffer &buffer,
        Rectangle rect,
        Alignment alignment,
        const ParagraphLayout::Result &layout,
        ParagraphBackgroundMode backgroundMode,
        const ColorResolver &colorResolver = {}) noexcept;
    ~ParagraphPainter() = default;
    ParagraphPainter(const ParagraphPainter &) = delete;
    ParagraphPainter(ParagraphPainter &&) = delete;
    auto operator=(const ParagraphPainter &) -> ParagraphPainter & = delete;
    auto operator=(ParagraphPainter &&) -> ParagraphPainter & = delete;

    /// Paint the given paragraph layout into a rectangle.
    void paint();

private:
    [[nodiscard]] auto usesLeftFill() const noexcept -> bool;
    [[nodiscard]] auto usesRightFill() const noexcept -> bool;
    [[nodiscard]] auto usesRightFillForLine(const ParagraphLayout::Line &line) const noexcept -> bool;
    [[nodiscard]] auto drawSegment(const String &text, Position pos) -> std::optional<Color>;
    void fillBackgroundRange(int y, int x1, int x2, Color color);

private:
    WritableBuffer &_buffer;
    Rectangle _rect;
    Alignment _alignment;
    const ParagraphLayout::Result &_layout;
    ParagraphBackgroundMode _backgroundMode;
    const ColorResolver &_colorResolver;
};


}
