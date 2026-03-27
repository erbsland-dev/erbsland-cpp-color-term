// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "LayoutContext.hpp"
#include "LayoutFragment.hpp"
#include "LayoutLine.hpp"
#include "LayoutNewlineMode.hpp"
#include "LayoutPreparedSourceLine.hpp"
#include "LayoutResult.hpp"
#include "LayoutSourceLineRange.hpp"

#include "../../FastCharSet.hpp"
#include "../../ParagraphOptions.hpp"
#include "../../String.hpp"

#include <vector>


namespace erbsland::cterm::impl::paragraph {


/// Shared paragraph layout for wrapped terminal text.
class Layout final {
public:
    Layout(const String &text, int width, const ParagraphOptions &options, LayoutNewlineMode newlineMode) noexcept;
    ~Layout() = default;
    Layout(const Layout &) = delete;
    Layout(Layout &&) = delete;
    auto operator=(const Layout &) -> Layout & = delete;
    auto operator=(Layout &&) -> Layout & = delete;

    /// Build the wrapped paragraph layout.
    /// @return The layout result.
    [[nodiscard]] auto build() -> LayoutResult;

private:
    [[nodiscard]] auto splitIntoSourceLines() const -> std::vector<LayoutSourceLineRange>;
    [[nodiscard]] auto
    layoutParagraph(const std::vector<LayoutSourceLineRange> &sourceLines, std::vector<LayoutLine> &lines) -> bool;
    [[nodiscard]] auto layoutSourceLine(LayoutSourceLineRange sourceLine, std::vector<LayoutLine> &lines) -> bool;
    [[nodiscard]] auto prepareSourceLine(LayoutSourceLineRange sourceLine) const -> LayoutPreparedSourceLine;

private:
    LayoutContext _context;
    LayoutNewlineMode _newlineMode;
    FastCharSetPtr _wordSeparators;
};


}
