// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "LayoutContext.hpp"
#include "LayoutFragment.hpp"
#include "LayoutLine.hpp"
#include "LayoutNewlineMode.hpp"
#include "LayoutPreparedSourceLine.hpp"
#include "LayoutResult.hpp"

#include "../../FastCharSet.hpp"
#include "../../IndexRange.hpp"
#include "../../ParagraphOptions.hpp"
#include "../../StringView.hpp"

#include <vector>

namespace erbsland::cterm::impl::paragraph {

/// Shared paragraph layout for wrapped terminal text.
class Layout final {
public:
    Layout(const StringView &text, int width, const ParagraphOptions &options, LayoutNewlineMode newlineMode) noexcept;
    ~Layout() = default;
    Layout(const Layout &) = delete;
    Layout(Layout &&) = delete;
    auto operator=(const Layout &) -> Layout & = delete;
    auto operator=(Layout &&) -> Layout & = delete;

    /// Build the wrapped paragraph layout.
    /// @return The layout result.
    [[nodiscard]] auto build() -> LayoutResult;

private:
    [[nodiscard]] auto splitIntoSourceLines() const -> std::vector<IndexRange>;
    [[nodiscard]] auto layoutParagraph(const std::vector<IndexRange> &sourceLines, std::vector<LayoutLine> &lines)
        -> bool;
    [[nodiscard]] auto layoutSourceLine(IndexRange sourceLine, std::vector<LayoutLine> &lines) -> bool;
    [[nodiscard]] auto prepareSourceLine(IndexRange sourceLine) const -> LayoutPreparedSourceLine;

private:
    LayoutContext _context;
    LayoutNewlineMode _newlineMode;
    FastCharSetPtr _wordSeparators;
};

}
