// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "ParagraphLayout.hpp"

#include "../ParagraphBackgroundMode.hpp"


namespace erbsland::cterm::impl {


/// Shared paragraph placement and background-fill rules for painters and printers.
class ParagraphRendererBase {
protected:
    /// Precomputed geometry for one rendered paragraph line.
    struct LinePlacement final {
        int textX = 0;        ///< The left column where the line text starts.
        int textWidth = 0;    ///< The display width of the line text.
        int endMarkX = 0;     ///< The left column where the line end mark starts.
        int endMarkWidth = 0; ///< The display width of the line end mark.
    };

protected:
    ParagraphRendererBase(
        Alignment alignment, const ParagraphLayout::Result &layout, ParagraphBackgroundMode backgroundMode) noexcept;
    ~ParagraphRendererBase() = default;
    ParagraphRendererBase(const ParagraphRendererBase &) = delete;
    ParagraphRendererBase(ParagraphRendererBase &&) = delete;
    auto operator=(const ParagraphRendererBase &) -> ParagraphRendererBase & = delete;
    auto operator=(ParagraphRendererBase &&) -> ParagraphRendererBase & = delete;

    [[nodiscard]] auto alignment() const noexcept -> Alignment { return _alignment; }
    [[nodiscard]] auto layout() const noexcept -> const ParagraphLayout::Result & { return _layout; }
    [[nodiscard]] auto usesLeftFill() const noexcept -> bool;
    [[nodiscard]] auto usesRightFill() const noexcept -> bool;
    [[nodiscard]] auto usesRightFillForLine(const ParagraphLayout::Line &line) const noexcept -> bool;
    [[nodiscard]] auto linePlacement(const ParagraphLayout::Line &line, int x1, int width) const noexcept
        -> LinePlacement;

private:
    Alignment _alignment;
    const ParagraphLayout::Result &_layout;
    ParagraphBackgroundMode _backgroundMode;
};


}
