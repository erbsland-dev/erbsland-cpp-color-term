// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "LayoutTestSupport.hpp"

#include <erbsland/cterm/impl/paragraph/Layout.hpp>

#include <utility>
#include <vector>

TESTED_TARGETS(Layout)
class LayoutTest final : public UNITTEST_SUBCLASS(LayoutTestSupport) {
public:
    void testLayoutCollapsesSeparatorRunsIntoOneGeneratedSpace() {
        const auto text = String{U"AA   BB"};
        auto layoutBuilder =
            paragraph::Layout{text, 20, ParagraphOptions{}, paragraph::LayoutNewlineMode::HardLineBreak};
        const auto layout = layoutBuilder.build();

        REQUIRE(layout.valid());
        requireRenderedLinesEqual(text, ParagraphOptions{}, layout, {"AA BB"});
    }

    void testLayoutPreservesLeadingTabsForLeftAlignedLines() {
        const auto text = String{U"\tAA"};
        auto options = ParagraphOptions{};
        options.setWordSeparators(U" ");
        options.setTabStops({4});
        auto layoutBuilder = paragraph::Layout{text, 20, options, paragraph::LayoutNewlineMode::HardLineBreak};
        const auto layout = layoutBuilder.build();

        REQUIRE(layout.valid());
        requireRenderedLinesEqual(text, options, layout, {"    AA"});
    }

    void testLayoutConsumesNonAdvancingTabsWhenLineBreakOverflowIsRequested() {
        const auto text = String{U"Head\ttext"};
        auto options = ParagraphOptions{};
        options.setWordSeparators(U" ");
        options.setTabStops({4});
        options.setTabOverflowBehavior(TabOverflowBehavior::LineBreak);
        auto layoutBuilder = paragraph::Layout{text, 20, options, paragraph::LayoutNewlineMode::HardLineBreak};
        const auto layout = layoutBuilder.build();

        REQUIRE(layout.valid());
        requireRenderedLinesEqual(text, options, layout, {"Head", "text"});
    }

    void testLayoutKeepsSeparatorAndTabTokensInOrderAcrossWraps() {
        const auto text = String{U"AA,\tBBBB CC"};
        auto options = ParagraphOptions{};
        options.setWordSeparators(U" ,");
        options.setTabStops({4});
        auto layoutBuilder = paragraph::Layout{text, 6, options, paragraph::LayoutNewlineMode::HardLineBreak};
        const auto layout = layoutBuilder.build();

        REQUIRE(layout.valid());
        requireRenderedLinesEqual(text, options, layout, {"AA", "    B-", "BBB CC"});
    }

    void testParagraphBreakModeTreatsEachSourceLineAsItsOwnParagraph() {
        const auto text = String{U"Alpha Beta\nGamma Delta"};
        auto options = ParagraphOptions{};
        options.setParagraphSpacing(ParagraphSpacing::DoubleLine);
        auto layoutBuilder = paragraph::Layout{text, 20, options, paragraph::LayoutNewlineMode::ParagraphBreak};
        const auto layout = layoutBuilder.build();

        REQUIRE(layout.valid());
        requireRenderedLinesEqual(text, options, layout, {"Alpha Beta", "", "Gamma Delta"});
    }

    void testLayoutResultConvenienceApiStoresOnlyRenderedLinesAndValidity() {
        auto result = paragraph::LayoutResult::create();

        REQUIRE(result.valid());
        REQUIRE(result.empty());
        REQUIRE_EQUAL(result.size(), std::size_t{0});

        result.appendEmptyLine();
        REQUIRE(!result.empty());
        REQUIRE_EQUAL(result.size(), std::size_t{1});

        auto line = paragraph::LayoutLine{};
        result.appendLine(std::move(line));
        REQUIRE_EQUAL(result.size(), std::size_t{2});

        const auto invalid = paragraph::LayoutResult::invalid();
        REQUIRE(!invalid.valid());
        REQUIRE(invalid.empty());
        REQUIRE_EQUAL(invalid.size(), std::size_t{0});
    }
};
