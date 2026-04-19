// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/impl/paragraph/Layout.hpp>

#include <utility>
#include <vector>

namespace paragraph = erbsland::cterm::impl::paragraph;

TESTED_TARGETS(Layout)
class LayoutTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void requireRenderedLinesEqual(
        const String &sourceText,
        const ParagraphOptions &options,
        const paragraph::LayoutResult &layout,
        const std::initializer_list<std::string_view> expectedLines) {
        auto expected = std::vector<std::string>{};
        expected.reserve(expectedLines.size());
        for (const auto line : expectedLines) {
            expected.emplace_back(line);
        }
        REQUIRE_EQUAL_LINES(renderLines(sourceText, options, layout), expected);
    }

    [[nodiscard]] static auto renderLine(
        const String &sourceText,
        const ParagraphOptions &options,
        const paragraph::LayoutResult &layout,
        const std::size_t index) -> std::string {
        auto result = std::string{};
        const auto &line = layout.lines()[index];
        result.append(static_cast<std::size_t>(line.indentWidth), ' ');
        for (const auto &fragment : line.fragments) {
            switch (fragment.type()) {
            case paragraph::LayoutFragment::Type::SourceRange:
                for (auto sourceIndex = fragment.startIndex(); sourceIndex < fragment.startIndex() + fragment.length();
                     ++sourceIndex) {
                    result += sourceText[sourceIndex].charStr();
                }
                break;
            case paragraph::LayoutFragment::Type::Spaces:
                result.append(static_cast<std::size_t>(fragment.width()), ' ');
                break;
            case paragraph::LayoutFragment::Type::LineBreakStartMark:
                for (const auto &character : options.lineBreakStartMark()) {
                    result += character.charStr();
                }
                break;
            case paragraph::LayoutFragment::Type::WordBreakMark:
                result += options.wordBreakMark().charStr();
                break;
            case paragraph::LayoutFragment::Type::ParagraphEllipsis:
                for (const auto &character : options.paragraphEllipsisMark()) {
                    result += character.charStr();
                }
                break;
            }
        }
        return result;
    }

    [[nodiscard]] static auto
    renderLines(const String &sourceText, const ParagraphOptions &options, const paragraph::LayoutResult &layout)
        -> std::vector<std::string> {
        auto result = std::vector<std::string>{};
        result.reserve(layout.size());
        for (auto index = std::size_t{0}; index < layout.size(); ++index) {
            result.push_back(renderLine(sourceText, options, layout, index));
        }
        return result;
    }

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
