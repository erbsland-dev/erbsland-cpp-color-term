// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ParagraphRendererBase.hpp"

#include "../ParagraphBackgroundMode.hpp"
#include "../WritableBuffer.hpp"

#include <functional>
#include <optional>


namespace erbsland::cterm::impl {


/// Paint a shared paragraph layout into a writable buffer.
class ParagraphPainter final : private ParagraphRendererBase {
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
    [[nodiscard]] auto drawSegment(const String &text, Position pos) -> std::optional<Color>;
    void fillBackgroundRange(int y, int x1, int x2, Color color);

private:
    WritableBuffer &_buffer;
    Rectangle _rect;
    const ColorResolver &_colorResolver;
};


}
