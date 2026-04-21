// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/TestHelper.hpp"

#include <erbsland/cterm/impl/paragraph/Layout.hpp>

#include <vector>

class LayoutTestSupport : public TestHelper {
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

    [[nodiscard]] auto renderLine(
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

    [[nodiscard]] auto
    renderLines(const String &sourceText, const ParagraphOptions &options, const paragraph::LayoutResult &layout)
        -> std::vector<std::string> {
        auto result = std::vector<std::string>{};
        result.reserve(layout.size());
        for (auto index = std::size_t{0}; index < layout.size(); ++index) {
            result.push_back(renderLine(sourceText, options, layout, index));
        }
        return result;
    }
};
