// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "CommandLineHelpModel.hpp"

#include <algorithm>

namespace demo::command_line_help {

auto DemoConfig::bodyParagraphOptions() const -> ParagraphOptions {
    auto options = ParagraphOptions{};
    options.setAlignment(alignment);
    options.setLineIndent(lineIndent);
    if (firstLineIndent.has_value()) {
        options.setFirstLineIndent(*firstLineIndent);
    }
    if (wrappedLineIndent.has_value()) {
        options.setWrappedLineIndent(*wrappedLineIndent);
    }
    options.setBackgroundMode(backgroundMode);
    options.setLineBreakStartMark(lineBreakStartMark);
    options.setLineBreakEndMark(lineBreakEndMark);
    options.setParagraphSpacing(paragraphSpacing);
    options.setWordSeparators(wordSeparators);
    options.setWordBreakMark(wordBreakMark);
    options.setMaximumLineWraps(maximumLineWraps);
    options.setParagraphEllipsisMark(paragraphEllipsisMark);
    if (tabStops.has_value()) {
        options.setTabStops(*tabStops);
    }
    options.setOnError(onError);
    return options;
}

auto DemoConfig::optionParagraphOptions(const int descriptionColumnValue) const -> ParagraphOptions {
    auto options = bodyParagraphOptions();
    options.setLineIndent(0);
    options.setFirstLineIndent(0);
    options.setWrappedLineIndent(wrappedLineIndent.value_or(descriptionColumnValue));
    options.setTabOverflowBehavior(TabOverflowBehavior::LineBreak);
    if (tabStops.has_value()) {
        options.setTabStops(*tabStops);
    } else {
        options.setTabStops({ParagraphOptions::cTabWrappedLineIndent});
    }
    return options;
}

auto DemoConfig::effectiveDescriptionColumn(const int widestSignatureWidth) const noexcept -> int {
    if (descriptionColumn.has_value()) {
        return *descriptionColumn;
    }
    return std::clamp(widestSignatureWidth + 2, 30, maximumDescriptionColumn);
}

auto OptionSpec::requiresValue() const noexcept -> bool {
    return !valueName.empty();
}

auto OptionSpec::matchesLongName(const std::string_view name) const noexcept -> bool {
    return longName == name;
}

auto OptionSpec::matchesShortName(const char name) const noexcept -> bool {
    return shortName != '\0' && shortName == name;
}

auto OptionSpec::plainSignature() const -> std::string {
    auto result = std::string{"--"};
    result += longName;
    if (shortName != '\0') {
        result += '/';
        result += '-';
        result += shortName;
    }
    if (!valueName.empty()) {
        result += '=';
        result += valueName;
    }
    return result;
}

auto OptionSpec::coloredSignature() const -> String {
    auto result = String{};
    result.append(fg::BrightCyan, "--", longName);
    if (shortName != '\0') {
        const auto shortNameText = std::string{1, shortName};
        result.append(fg::White, "/", fg::BrightYellow, "-", shortNameText);
    }
    if (!valueName.empty()) {
        result.append(fg::White, "=", fg::Magenta, valueName);
    }
    return result;
}

auto OptionSpec::signatureWidth() const -> int {
    return coloredSignature().displayWidth();
}

}
