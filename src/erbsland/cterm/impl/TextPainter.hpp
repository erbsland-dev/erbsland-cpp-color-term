// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../WritableBuffer.hpp"


namespace erbsland::cterm::impl {


class TextPainter final {
public:
    explicit TextPainter(WritableBuffer &buffer) : _buffer{buffer} {}

    // delete copy/move
    ~TextPainter() = default;
    TextPainter(const TextPainter &) = delete;
    TextPainter(TextPainter &&) = delete;
    auto operator=(const TextPainter &) -> TextPainter & = delete;
    auto operator=(TextPainter &&) -> TextPainter & = delete;

public:
    void drawText(Position pos, const String &str);
    void drawText(const Text &text, std::size_t animationCycle = 0);
    void drawText(
        std::string_view text,
        Rectangle rect,
        Alignment alignment = Alignment::TopLeft,
        Color color = {},
        std::size_t animationCycle = 0);
    void drawText(
        String text,
        Rectangle rect,
        Alignment alignment = Alignment::TopLeft,
        Color color = {},
        std::size_t animationCycle = 0);

private: // wrapper (to keep code simple)
    [[nodiscard]] auto size() const noexcept -> Size { return _buffer.size(); }
    [[nodiscard]] auto rect() const noexcept -> Rectangle { return _buffer.rect(); }
    [[nodiscard]] auto get(Position pos) const noexcept -> const Char & { return _buffer.get(pos); }
    void set(Position pos, const Char &block, const CharCombinationStylePtr &combinationStyle = {}) noexcept {
        _buffer.set(pos, block, combinationStyle);
    }
    void fill(Rectangle rect, const Char &fillBlock, const CharCombinationStylePtr &combinationStyle = {}) noexcept {
        _buffer.fill(rect, fillBlock, combinationStyle);
    }

private: // helper
    [[nodiscard]] auto buildSimpleTextLines(const Text &text) const -> StringLines;
    [[nodiscard]] auto buildFontTextLines(const Text &text, const String &paragraph) const -> StringLines;
    void applyTextLines(const Text &text, const StringLines &lines, std::size_t animationCycle) noexcept;
    [[nodiscard]] auto colorForTextPosition(
        const Text &text, const Char &character, Position position, std::size_t animationCycle) const noexcept -> Color;

private:
    WritableBuffer &_buffer;
};


}
