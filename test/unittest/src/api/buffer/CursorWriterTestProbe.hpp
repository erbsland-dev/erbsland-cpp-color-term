// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/TestHelper.hpp"

#include <optional>

class CursorWriterProbe final : public CursorWriter {
public:
    [[nodiscard]] auto color() const noexcept -> Color override { return _color; }
    [[nodiscard]] auto charAttributes() const noexcept -> CharAttributes override { return _attributes; }
    void setColor(const Color color) noexcept override { _color = color; }
    void setCharAttributes(const CharAttributes attributes) noexcept override { _attributes = attributes; }
    void setForeground(const Foreground color) noexcept override { _color.setFg(color); }
    void setBackground(const Background color) noexcept override { _color.setBg(color); }
    [[nodiscard]] auto supportedCharAttributes() const noexcept -> CharAttributes override {
        return CharAttributes::all();
    }
    void moveCursor(const Position posOrDelta, const MoveMode mode) noexcept override {
        _lastMove = posOrDelta;
        _lastMoveMode = mode;
    }
    void setAutoWrap(const bool enabled) noexcept override { _autoWrap = enabled; }
    [[nodiscard]] auto size() const noexcept -> Size override { return Size{80, 25}; }
    void clearScreen() noexcept override { _clearScreenCallCount += 1; }
    void write(const Char &character) noexcept override { _writtenChars.push_back(character); }
    void write(const StringView &str) noexcept override { _writtenStrings.push_back(String{str}); }
    void write(const ReadableBuffer &) noexcept override { _writeBufferCallCount += 1; }
    void writeLineBreak() noexcept override { _lineBreakCount += 1; }

protected:
    auto printParagraphImpl(const StringView &paragraph, const ParagraphOptions &options) noexcept -> int override {
        _lastParagraph = String{paragraph};
        _lastParagraphAlignment = options.alignment();
        return 7;
    }

public:
    Color _color{};
    CharAttributes _attributes{};
    Position _lastMove{};
    MoveMode _lastMoveMode{MoveMode::Absolute};
    bool _autoWrap{false};
    int _clearScreenCallCount{0};
    std::vector<Char> _writtenChars;
    std::vector<String> _writtenStrings;
    int _writeBufferCallCount{0};
    int _lineBreakCount{0};
    String _lastParagraph{};
    Alignment _lastParagraphAlignment{Alignment::TopLeft};
};
