// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <erbsland/cterm/all.hpp>

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace demo::command_line_help {

using namespace erbsland::cterm;

/// Complete rendering configuration for the command-line help demo.
struct DemoConfig final {
    std::optional<int> terminalWidthOverride; ///< Simulated terminal width in cells.
    std::optional<int> descriptionColumn;     ///< Optional override for the option description column.
    int maximumDescriptionColumn{35};         ///< Maximum auto-selected option description column.
    Alignment alignment{Alignment::Left};     ///< Horizontal paragraph alignment.
    int lineIndent{0};                        ///< Base indent for regular preview paragraphs.
    std::optional<int> firstLineIndent;       ///< Optional override for first-line indentation.
    std::optional<int> wrappedLineIndent;     ///< Optional override for wrapped-line indentation.
    ParagraphBackgroundMode backgroundMode{ParagraphBackgroundMode::Default}; ///< Paragraph background handling.
    String lineBreakStartMark{};                                     ///< Marker inserted at the start of wrapped lines.
    String lineBreakEndMark{};                                       ///< Marker appended at the end of wrapped lines.
    ParagraphSpacing paragraphSpacing{ParagraphSpacing::SingleLine}; ///< Spacing after each paragraph.
    std::u32string wordSeparators{U" \t"};                           ///< Characters that split words.
    Char wordBreakMark{U"-"};                                        ///< Marker inserted when a long word is broken.
    int maximumLineWraps{0};                                         ///< Zero means unlimited line wrapping.
    String paragraphEllipsisMark{U"…"};                              ///< Marker used after the final permitted wrap.
    std::optional<std::vector<int>> tabStops;                        ///< Optional custom tab stops for option lines.
    ParagraphOnError onError{ParagraphOnError::PlainOutput};         ///< Fallback if the paragraph cannot be rendered.

    /// Build paragraph options for regular preview and body text.
    [[nodiscard]] auto bodyParagraphOptions() const -> ParagraphOptions;
    /// Build paragraph options for command-line option descriptions.
    [[nodiscard]] auto optionParagraphOptions(int descriptionColumnValue) const -> ParagraphOptions;
    /// Resolve the option description column from either the override or the option signature widths.
    [[nodiscard]] auto effectiveDescriptionColumn(int widestSignatureWidth) const noexcept -> int;
};

/// One command-line option together with its help output and parsing callback.
struct OptionSpec final {
    using ApplyValueFn = void (*)(DemoConfig &, std::string_view);

    std::string_view longName;    ///< Long GNU-style option name without leading `--`.
    char shortName{};             ///< Optional short one-letter name, or `\0`.
    std::string_view valueName;   ///< Value placeholder including angle brackets, or empty for flags.
    std::string_view description; ///< Human-readable explanation shown in the help screen.
    ApplyValueFn applyValue{};    ///< Callback that applies the parsed value to the config.

    /// Test whether this option expects an additional value.
    [[nodiscard]] auto requiresValue() const noexcept -> bool;
    /// Test whether this option matches the given long name.
    [[nodiscard]] auto matchesLongName(std::string_view name) const noexcept -> bool;
    /// Test whether this option matches the given short name.
    [[nodiscard]] auto matchesShortName(char name) const noexcept -> bool;
    /// Create a plain-text signature for narrow terminals and error messages.
    [[nodiscard]] auto plainSignature() const -> std::string;
    /// Create a colorized signature for the rich help output.
    [[nodiscard]] auto coloredSignature() const -> String;
    /// Calculate the visible width of the signature in terminal cells.
    [[nodiscard]] auto signatureWidth() const -> int;
};

/// The rendering mode for one help section.
enum class HelpSectionKind : uint8_t {
    Paragraphs, ///< Render regular explanatory paragraphs.
    Options,    ///< Render option paragraphs built from tab-aligned signatures and descriptions.
};

/// One titled section in the rendered help document.
struct HelpSection final {
    String title;                   ///< Section heading.
    HelpSectionKind kind{};         ///< Rendering style for this section.
    std::vector<String> paragraphs; ///< Paragraphs or option lines for the section.
};

/// Full help document assembled for the demo output.
struct HelpDocument final {
    String usageLine;                  ///< Colored usage line.
    std::vector<HelpSection> sections; ///< Ordered document sections.
};

}
