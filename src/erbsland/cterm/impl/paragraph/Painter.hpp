// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "RendererBase.hpp"

#include "../../ParagraphBackgroundMode.hpp"
#include "../../WritableBuffer.hpp"

#include <functional>
#include <optional>

namespace erbsland::cterm::impl::paragraph {

/// Paint a shared paragraph layout into a writable buffer.
class Painter final : public RendererBase {
public:
    using ColorResolver = std::function<Color(const Char &, Position)>;

public:
    Painter(
        WritableBuffer &buffer,
        const Rectangle rect,
        const Alignment alignment,
        const LayoutResult &layout,
        const StringView &sourceText,
        const ParagraphOptions &options,
        const ParagraphBackgroundMode backgroundMode,
        const ColorResolver &colorResolver = {}) noexcept :
        RendererBase{alignment, layout, sourceText, options, backgroundMode},
        _buffer{buffer},
        _rect{rect},
        _colorResolver{colorResolver} {}

    // defaults/deletions
    ~Painter() = default;
    Painter(const Painter &) = delete;
    Painter(Painter &&) = delete;
    auto operator=(const Painter &) -> Painter & = delete;
    auto operator=(Painter &&) -> Painter & = delete;

public:
    /// Paint the given paragraph layout into a rectangle.
    void paint();

private:
    [[nodiscard]] auto drawLine(const LayoutLine &line, Position pos) -> std::optional<Color>;
    [[nodiscard]] auto drawFragment(const LayoutFragment &fragment, Position &pos) -> std::optional<Color>;
    [[nodiscard]] auto drawCharacter(const Char &character, Position &pos) -> std::optional<Color>;
    void fillBackgroundRange(int y, int x1, int x2, Color color);

private:
    WritableBuffer &_buffer;
    Rectangle _rect;
    const ColorResolver &_colorResolver;
};

}
