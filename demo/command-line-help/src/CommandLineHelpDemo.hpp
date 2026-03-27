// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "CommandLineHelpModel.hpp"
#include "TerminalApplication.hpp"

#include <optional>


namespace demo::command_line_help {


/// Demonstrate colorful command help output together with `Terminal::printParagraph()`.
class CommandLineHelpDemo final : public TerminalApplication {
public:
    /// Parse the command line and store the rendering configuration.
    auto onCommandLine(const std::vector<std::string_view> &args) -> int override;
    /// Render the help output once and exit the demo.
    auto beforeRun() -> int override;

private:
    struct ParsedArguments final {
        DemoConfig config;
        std::vector<std::string> errors;
    };

private:
    [[nodiscard]] static auto optionSpecs() -> const std::vector<OptionSpec> &;
    [[nodiscard]] static auto parseArguments(const std::vector<std::string_view> &args) -> ParsedArguments;
    static void parseLongOption(
        std::string_view argument,
        std::optional<std::string_view> nextArgument,
        std::size_t &index,
        DemoConfig &config,
        std::vector<std::string> &errors);
    static void parseShortOption(
        std::string_view argument,
        std::optional<std::string_view> nextArgument,
        std::size_t &index,
        DemoConfig &config,
        std::vector<std::string> &errors);
    [[nodiscard]] static auto findOptionByLongName(std::string_view name) -> const OptionSpec *;
    [[nodiscard]] static auto findOptionByShortName(char name) -> const OptionSpec *;
    static void applyOptionValue(
        const OptionSpec &spec,
        std::string_view optionLabel,
        std::optional<std::string_view> inlineValue,
        std::optional<std::string_view> nextArgument,
        std::size_t &index,
        DemoConfig &config,
        std::vector<std::string> &errors);

    static void applyHelpFlag(DemoConfig &config, std::string_view value);
    static void applyTerminalWidth(DemoConfig &config, std::string_view value);
    static void applyDescriptionColumn(DemoConfig &config, std::string_view value);
    static void applyMaximumDescriptionColumn(DemoConfig &config, std::string_view value);
    static void applyAlignment(DemoConfig &config, std::string_view value);
    static void applyLineIndent(DemoConfig &config, std::string_view value);
    static void applyFirstLineIndent(DemoConfig &config, std::string_view value);
    static void applyWrappedLineIndent(DemoConfig &config, std::string_view value);
    static void applyBackgroundMode(DemoConfig &config, std::string_view value);
    static void applyLineBreakStartMark(DemoConfig &config, std::string_view value);
    static void applyLineBreakEndMark(DemoConfig &config, std::string_view value);
    static void applyParagraphSpacing(DemoConfig &config, std::string_view value);
    static void applyWordSeparators(DemoConfig &config, std::string_view value);
    static void applyWordBreakMark(DemoConfig &config, std::string_view value);
    static void applyMaximumLineWraps(DemoConfig &config, std::string_view value);
    static void applyParagraphEllipsisMark(DemoConfig &config, std::string_view value);
    static void applyTabStops(DemoConfig &config, std::string_view value);
    static void applyOnError(DemoConfig &config, std::string_view value);

    [[nodiscard]] static auto parseIntInRange(std::string_view value, std::string_view optionName, int min, int max)
        -> int;
    [[nodiscard]] static auto parseParagraphString(std::string_view value, std::string_view optionName) -> String;
    [[nodiscard]] static auto parseSingleCharacter(std::string_view value, std::string_view optionName) -> Char;
    [[nodiscard]] static auto parseWordSeparators(std::string_view value) -> std::u32string;
    [[nodiscard]] static auto parseTabStopList(std::string_view value) -> std::vector<int>;
    [[nodiscard]] static auto trim(std::string_view text) noexcept -> std::string_view;
    [[nodiscard]] static auto splitCommaSeparated(std::string_view text) -> std::vector<std::string_view>;

    [[nodiscard]] static auto buildHelpDocument(const DemoConfig &config) -> HelpDocument;
    [[nodiscard]] static auto buildIntroParagraph() -> String;
    [[nodiscard]] static auto buildPreviewParagraph() -> String;
    [[nodiscard]] static auto buildClosingParagraph() -> String;
    [[nodiscard]] static auto buildOptionParagraph(const OptionSpec &spec) -> String;
    [[nodiscard]] static auto widestSignatureWidth() -> int;

    static void renderErrors(Terminal &terminal, const std::vector<std::string> &errors) noexcept;
    static void renderDocument(Terminal &terminal, const HelpDocument &document, const DemoConfig &config) noexcept;
    static void renderWideDocument(Terminal &terminal, const HelpDocument &document, const DemoConfig &config) noexcept;
    static void
    renderNarrowDocument(Terminal &terminal, const HelpDocument &document, const DemoConfig &config) noexcept;
    static void printUsageLine(Terminal &terminal, const String &usageLine) noexcept;
    static void printSectionTitle(Terminal &terminal, const String &title) noexcept;
    static void printNarrowParagraph(Terminal &terminal, const String &paragraph) noexcept;
    static void printNarrowOption(Terminal &terminal, const OptionSpec &spec) noexcept;
    static void prepareTerminal(Terminal &terminal, const DemoConfig &config) noexcept;

private:
    DemoConfig _config;
    std::vector<std::string> _errors;
};


}
