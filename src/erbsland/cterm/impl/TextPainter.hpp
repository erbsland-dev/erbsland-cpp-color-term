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
    void drawText(Position pos, const StringView &str);
    void drawText(const Text &text, std::size_t animationCycle = 0);
    void drawText(const StringView &text, Rectangle rect, const TextOptions &options, std::size_t animationCycle = 0);
    void drawText(
        std::string_view text,
        Rectangle rect,
        Alignment alignment = Alignment::TopLeft,
        CharStyle style = {},
        std::size_t animationCycle = 0);
    void drawText(
        std::u32string_view text,
        Rectangle rect,
        Alignment alignment = Alignment::TopLeft,
        CharStyle style = {},
        std::size_t animationCycle = 0);
    void drawText(
        const StringView &text,
        Rectangle rect,
        Alignment alignment = Alignment::TopLeft,
        CharStyle style = {},
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
    [[nodiscard]] static auto simpleTextOptions(Alignment alignment, CharStyle style) noexcept -> TextOptions;
    [[nodiscard]] static auto contentRect(Rectangle rect, const ParagraphOptions &options) noexcept -> Rectangle;
    [[nodiscard]] auto buildSimpleTextLines(const StringView &text, Rectangle rect, ParagraphSpacing spacing) const
        -> StringLines;
    [[nodiscard]] auto buildFontTextLines(const TextOptions &options, const StringView &paragraph) const -> StringLines;
    void applyTextLines(
        Rectangle rect, const TextOptions &options, const StringLines &lines, std::size_t animationCycle) noexcept;
    [[nodiscard]] auto colorForTextPosition(
        const TextOptions &options, const Char &character, Position position, std::size_t animationCycle) const noexcept
        -> Color;

private:
    WritableBuffer &_buffer;
};

}
