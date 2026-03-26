// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ParagraphRendererBase.hpp"

#include "../CursorWriter.hpp"

#include <optional>


namespace erbsland::cterm::impl {


/// Print a shared paragraph layout sequentially to a cursor writer.
class ParagraphPrinter final : private ParagraphRendererBase {
public:
    ParagraphPrinter(
        CursorWriter &writer,
        int width,
        Alignment alignment,
        const ParagraphLayout::Result &layout,
        ParagraphBackgroundMode backgroundMode) noexcept;
    ~ParagraphPrinter() = default;
    ParagraphPrinter(const ParagraphPrinter &) = delete;
    ParagraphPrinter(ParagraphPrinter &&) = delete;
    auto operator=(const ParagraphPrinter &) -> ParagraphPrinter & = delete;
    auto operator=(ParagraphPrinter &&) -> ParagraphPrinter & = delete;

    /// Print the paragraph layout line by line.
    /// @return The number of physical lines written.
    [[nodiscard]] auto print() -> int;

private:
    [[nodiscard]] auto backgroundFillColor(Color color) const noexcept -> Color;
    [[nodiscard]] auto resolvedWithoutIndent(const ParagraphLayout::Line &line) const noexcept -> String;
    [[nodiscard]] auto writeResolved(const String &text) -> std::optional<Color>;
    void writeSpaces(int count, Color color);

private:
    CursorWriter &_writer;
    int _width;
    Color _baseColor;
};


}
