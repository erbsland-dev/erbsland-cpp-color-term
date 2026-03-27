// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "CommandLineHelpDemo.hpp"

#include <algorithm>
#include <charconv>
#include <format>
#include <stdexcept>


namespace demo::command_line_help {


auto CommandLineHelpDemo::onCommandLine(const std::vector<std::string_view> &args) -> int {
    const auto parsedArguments = parseArguments(args);
    _config = parsedArguments.config;
    _errors = parsedArguments.errors;
    return 0;
}


auto CommandLineHelpDemo::beforeRun() -> int {
    const auto renderConfig = _errors.empty() ? _config : DemoConfig{};
    prepareTerminal(_terminal, renderConfig);
    renderErrors(_terminal, _errors);
    renderDocument(_terminal, buildHelpDocument(renderConfig), renderConfig);
    _terminal.setDefaultColor();
    _terminal.flush();
    return -1;
}


auto CommandLineHelpDemo::optionSpecs() -> const std::vector<OptionSpec> & {
    static const auto cOptionSpecs = std::vector<OptionSpec>{
        {
            .longName = "help",
            .shortName = 'h',
            .valueName = {},
            .description = "Render this formatted help output. The flag is mostly here so the demo behaves like "
                           "a familiar command-line tool.",
            .applyValue = &CommandLineHelpDemo::applyHelpFlag,
        },
        {
            .longName = "terminal-width",
            .shortName = 't',
            .valueName = "<columns>",
            .description = "Disable automatic width detection and simulate a terminal width between 20 and 200 "
                           "cells for deterministic wrapping.",
            .applyValue = &CommandLineHelpDemo::applyTerminalWidth,
        },
        {
            .longName = "description-column",
            .shortName = 'c',
            .valueName = "<column>",
            .description = "Override the description tab stop for the options list with a fixed value. Valid values "
                           "are 12 to 60.",
            .applyValue = &CommandLineHelpDemo::applyDescriptionColumn,
        },
        {
            .longName = "maximum-description-column",
            .shortName = 'd',
            .valueName = "<column>",
            .description = "Cap the automatically chosen description tab stop. The default is 35 to force some "
                           "wrapping in the options list. Valid values are 20 to 60.",
            .applyValue = &CommandLineHelpDemo::applyMaximumDescriptionColumn,
        },
        {
            .longName = "alignment",
            .shortName = 'a',
            .valueName = "<left|center|right>",
            .description = "Choose the horizontal alignment used for the wrapped preview paragraphs.",
            .applyValue = &CommandLineHelpDemo::applyAlignment,
        },
        {
            .longName = "line-indent",
            .shortName = 'l',
            .valueName = "<columns>",
            .description = "Indent the preview paragraphs by 0 to 10 columns before any wrapping takes place.",
            .applyValue = &CommandLineHelpDemo::applyLineIndent,
        },
        {
            .longName = "first-line-indent",
            .shortName = 'f',
            .valueName = "<columns>",
            .description = "Override the first-line indent for preview paragraphs. Valid values are 0 to 12.",
            .applyValue = &CommandLineHelpDemo::applyFirstLineIndent,
        },
        {
            .longName = "wrapped-line-indent",
            .shortName = 'w',
            .valueName = "<columns>",
            .description = "Override the indentation of wrapped lines. Values between 0 and 30 make the effect easy "
                           "to inspect.",
            .applyValue = &CommandLineHelpDemo::applyWrappedLineIndent,
        },
        {
            .longName = "background-mode",
            .shortName = 'b',
            .valueName = "<mode>",
            .description = "Set the background fill strategy: default, wrapped-left, wrapped-right, wrapped-both, "
                           "full-right, or full-both.",
            .applyValue = &CommandLineHelpDemo::applyBackgroundMode,
        },
        {
            .longName = "line-break-start-mark",
            .shortName = 's',
            .valueName = "<text>",
            .description = "Insert a one- or two-character marker at the start of each wrapped continuation line.",
            .applyValue = &CommandLineHelpDemo::applyLineBreakStartMark,
        },
        {
            .longName = "line-break-end-mark",
            .shortName = 'm',
            .valueName = "<text>",
            .description = "Append a one- or two-character marker at the right edge when a line wraps.",
            .applyValue = &CommandLineHelpDemo::applyLineBreakEndMark,
        },
        {
            .longName = "paragraph-spacing",
            .shortName = 'p',
            .valueName = "<single|double>",
            .description = "Switch between compact paragraphs or double-spaced output with one empty row in "
                           "between.",
            .applyValue = &CommandLineHelpDemo::applyParagraphSpacing,
        },
        {
            .longName = "word-separators",
            .shortName = 'i',
            .valueName = "<tokens>",
            .description = "Use comma-separated separator tokens such as space,tab,slash or one-character literals "
                           "like ; and |.",
            .applyValue = &CommandLineHelpDemo::applyWordSeparators,
        },
        {
            .longName = "word-break-mark",
            .shortName = 'k',
            .valueName = "<char>",
            .description = "Set the single character inserted when a word is split because it does not fit on the "
                           "current line.",
            .applyValue = &CommandLineHelpDemo::applyWordBreakMark,
        },
        {
            .longName = "maximum-line-wraps",
            .shortName = 'r',
            .valueName = "<count>",
            .description = "Limit the number of automatic wraps per paragraph. Use 0 for unlimited or values up to 8 "
                           "to trigger ellipsis behaviour.",
            .applyValue = &CommandLineHelpDemo::applyMaximumLineWraps,
        },
        {
            .longName = "paragraph-ellipsis-mark",
            .shortName = 'x',
            .valueName = "<text>",
            .description = "Choose the marker that signals clipped paragraphs after the configured wrap limit has been "
                           "reached.",
            .applyValue = &CommandLineHelpDemo::applyParagraphEllipsisMark,
        },
        {
            .longName = "tab-stops",
            .shortName = 'u',
            .valueName = "<list>",
            .description = "Provide comma-separated tab stops like 1,24,40 or use wrapped to align a stop with the "
                           "wrapped-line indent.",
            .applyValue = &CommandLineHelpDemo::applyTabStops,
        },
        {
            .longName = "on-error",
            .shortName = 'o',
            .valueName = "<plain|empty>",
            .description = "Choose the fallback when the paragraph cannot be laid out: plain output or empty output.",
            .applyValue = &CommandLineHelpDemo::applyOnError,
        },
    };
    return cOptionSpecs;
}


auto CommandLineHelpDemo::parseArguments(const std::vector<std::string_view> &args) -> ParsedArguments {
    auto result = ParsedArguments{};
    for (auto index = std::size_t{1}; index < args.size(); ++index) {
        const auto argument = args[index];
        const auto nextArgument =
            (index + 1U < args.size()) ? std::optional<std::string_view>{args[index + 1U]} : std::nullopt;
        if (argument.starts_with("--")) {
            parseLongOption(argument, nextArgument, index, result.config, result.errors);
            continue;
        }
        if (argument.starts_with('-') && argument.size() >= 2) {
            parseShortOption(argument, nextArgument, index, result.config, result.errors);
            continue;
        }
        result.errors.push_back(std::format("Unexpected positional argument: {}", argument));
    }
    return result;
}


void CommandLineHelpDemo::parseLongOption(
    const std::string_view argument,
    const std::optional<std::string_view> nextArgument,
    std::size_t &index,
    DemoConfig &config,
    std::vector<std::string> &errors) {

    const auto optionText = argument.substr(2);
    const auto separator = optionText.find('=');
    const auto name = optionText.substr(0, separator);
    const auto *option = findOptionByLongName(name);
    if (option == nullptr) {
        errors.push_back(std::format("Unknown option: {}", argument));
        return;
    }
    const auto inlineValue =
        separator == std::string_view::npos ? std::optional<std::string_view>{} : optionText.substr(separator + 1);
    applyOptionValue(*option, std::format("--{}", name), inlineValue, nextArgument, index, config, errors);
}


void CommandLineHelpDemo::parseShortOption(
    const std::string_view argument,
    const std::optional<std::string_view> nextArgument,
    std::size_t &index,
    DemoConfig &config,
    std::vector<std::string> &errors) {

    const auto shortName = argument[1];
    const auto *option = findOptionByShortName(shortName);
    if (option == nullptr) {
        errors.push_back(std::format("Unknown option: {}", argument));
        return;
    }
    const auto suffix = argument.substr(2);
    auto inlineValue = std::optional<std::string_view>{};
    if (!suffix.empty()) {
        if (!suffix.starts_with('=')) {
            errors.push_back(std::format("Short option values must use '=': {}", argument));
            return;
        }
        inlineValue = suffix.substr(1);
    }
    applyOptionValue(*option, std::format("-{}", shortName), inlineValue, nextArgument, index, config, errors);
}


auto CommandLineHelpDemo::findOptionByLongName(const std::string_view name) -> const OptionSpec * {
    for (const auto &spec : optionSpecs()) {
        if (spec.matchesLongName(name)) {
            return &spec;
        }
    }
    return nullptr;
}


auto CommandLineHelpDemo::findOptionByShortName(const char name) -> const OptionSpec * {
    for (const auto &spec : optionSpecs()) {
        if (spec.matchesShortName(name)) {
            return &spec;
        }
    }
    return nullptr;
}


void CommandLineHelpDemo::applyOptionValue(
    const OptionSpec &spec,
    const std::string_view optionLabel,
    const std::optional<std::string_view> inlineValue,
    const std::optional<std::string_view> nextArgument,
    std::size_t &index,
    DemoConfig &config,
    std::vector<std::string> &errors) {

    auto value = std::string_view{};
    if (spec.requiresValue()) {
        if (inlineValue.has_value()) {
            value = *inlineValue;
        } else if (nextArgument.has_value()) {
            value = *nextArgument;
            ++index;
        } else {
            errors.push_back(std::format("Missing value for {}", optionLabel));
            return;
        }
    } else if (inlineValue.has_value()) {
        errors.push_back(std::format("{} does not accept a value.", optionLabel));
        return;
    }
    try {
        spec.applyValue(config, value);
    } catch (const std::exception &exception) {
        errors.push_back(std::format("{}: {}", optionLabel, exception.what()));
    }
}


void CommandLineHelpDemo::applyHelpFlag(DemoConfig &, const std::string_view) {
}


void CommandLineHelpDemo::applyTerminalWidth(DemoConfig &config, const std::string_view value) {
    config.terminalWidthOverride = parseIntInRange(value, "terminal width", 20, 200);
}


void CommandLineHelpDemo::applyDescriptionColumn(DemoConfig &config, const std::string_view value) {
    config.descriptionColumn = parseIntInRange(value, "description column", 12, 60);
}


void CommandLineHelpDemo::applyMaximumDescriptionColumn(DemoConfig &config, const std::string_view value) {
    config.maximumDescriptionColumn = parseIntInRange(value, "maximum description column", 20, 60);
}


void CommandLineHelpDemo::applyAlignment(DemoConfig &config, const std::string_view value) {
    if (value == "left") {
        config.alignment = Alignment::Left;
        return;
    }
    if (value == "center") {
        config.alignment = Alignment::HCenter;
        return;
    }
    if (value == "right") {
        config.alignment = Alignment::Right;
        return;
    }
    throw std::invalid_argument{"alignment must be left, center, or right."};
}


void CommandLineHelpDemo::applyLineIndent(DemoConfig &config, const std::string_view value) {
    config.lineIndent = parseIntInRange(value, "line indent", 0, 10);
}


void CommandLineHelpDemo::applyFirstLineIndent(DemoConfig &config, const std::string_view value) {
    config.firstLineIndent = parseIntInRange(value, "first-line indent", 0, 12);
}


void CommandLineHelpDemo::applyWrappedLineIndent(DemoConfig &config, const std::string_view value) {
    config.wrappedLineIndent = parseIntInRange(value, "wrapped-line indent", 0, 30);
}


void CommandLineHelpDemo::applyBackgroundMode(DemoConfig &config, const std::string_view value) {
    if (value == "default") {
        config.backgroundMode = ParagraphBackgroundMode::Default;
        return;
    }
    if (value == "wrapped-left") {
        config.backgroundMode = ParagraphBackgroundMode::WrappedLeft;
        return;
    }
    if (value == "wrapped-right") {
        config.backgroundMode = ParagraphBackgroundMode::WrappedRight;
        return;
    }
    if (value == "wrapped-both") {
        config.backgroundMode = ParagraphBackgroundMode::WrappedBoth;
        return;
    }
    if (value == "full-right") {
        config.backgroundMode = ParagraphBackgroundMode::FullRight;
        return;
    }
    if (value == "full-both") {
        config.backgroundMode = ParagraphBackgroundMode::FullBoth;
        return;
    }
    throw std::invalid_argument{
        "background mode must be default, wrapped-left, wrapped-right, wrapped-both, full-right, or full-both."};
}


void CommandLineHelpDemo::applyLineBreakStartMark(DemoConfig &config, const std::string_view value) {
    config.lineBreakStartMark = parseParagraphString(value, "line-break-start-mark");
    auto options = ParagraphOptions{};
    options.setLineBreakStartMark(config.lineBreakStartMark);
}


void CommandLineHelpDemo::applyLineBreakEndMark(DemoConfig &config, const std::string_view value) {
    config.lineBreakEndMark = parseParagraphString(value, "line-break-end-mark");
    auto options = ParagraphOptions{};
    options.setLineBreakEndMark(config.lineBreakEndMark);
}


void CommandLineHelpDemo::applyParagraphSpacing(DemoConfig &config, const std::string_view value) {
    if (value == "single") {
        config.paragraphSpacing = ParagraphSpacing::SingleLine;
        return;
    }
    if (value == "double") {
        config.paragraphSpacing = ParagraphSpacing::DoubleLine;
        return;
    }
    throw std::invalid_argument{"paragraph spacing must be single or double."};
}


void CommandLineHelpDemo::applyWordSeparators(DemoConfig &config, const std::string_view value) {
    config.wordSeparators = parseWordSeparators(value);
}


void CommandLineHelpDemo::applyWordBreakMark(DemoConfig &config, const std::string_view value) {
    config.wordBreakMark = parseSingleCharacter(value, "word-break-mark");
}


void CommandLineHelpDemo::applyMaximumLineWraps(DemoConfig &config, const std::string_view value) {
    config.maximumLineWraps = parseIntInRange(value, "maximum-line-wraps", 0, 8);
}


void CommandLineHelpDemo::applyParagraphEllipsisMark(DemoConfig &config, const std::string_view value) {
    config.paragraphEllipsisMark = parseParagraphString(value, "paragraph-ellipsis-mark");
}


void CommandLineHelpDemo::applyTabStops(DemoConfig &config, const std::string_view value) {
    config.tabStops = parseTabStopList(value);
}


void CommandLineHelpDemo::applyOnError(DemoConfig &config, const std::string_view value) {
    if (value == "plain") {
        config.onError = ParagraphOnError::PlainOutput;
        return;
    }
    if (value == "empty") {
        config.onError = ParagraphOnError::Empty;
        return;
    }
    throw std::invalid_argument{"on-error must be plain or empty."};
}


auto CommandLineHelpDemo::parseIntInRange(
    const std::string_view value, const std::string_view optionName, const int min, const int max) -> int {

    auto parsedValue = 0;
    const auto *begin = value.data();
    const auto *end = value.data() + value.size();
    const auto result = std::from_chars(begin, end, parsedValue);
    if (result.ec != std::errc{} || result.ptr != end) {
        throw std::invalid_argument{std::format("{} must be an integer.", optionName)};
    }
    if (parsedValue < min || parsedValue > max) {
        throw std::invalid_argument{std::format("{} must be between {} and {}.", optionName, min, max)};
    }
    return parsedValue;
}


auto CommandLineHelpDemo::parseParagraphString(const std::string_view value, const std::string_view optionName)
    -> String {
    try {
        return String{value};
    } catch (const std::exception &exception) {
        throw std::invalid_argument{std::format("{} is not valid UTF-8: {}", optionName, exception.what())};
    }
}


auto CommandLineHelpDemo::parseSingleCharacter(const std::string_view value, const std::string_view optionName)
    -> Char {
    const auto stringValue = parseParagraphString(value, optionName);
    if (stringValue.size() != 1) {
        throw std::invalid_argument{std::format("{} must contain exactly one terminal character.", optionName)};
    }
    return stringValue[0];
}


auto CommandLineHelpDemo::parseWordSeparators(const std::string_view value) -> std::u32string {
    auto result = std::u32string{};
    for (const auto token : splitCommaSeparated(value)) {
        if (token.empty()) {
            continue;
        }
        if (token == "space") {
            result += U' ';
        } else if (token == "tab") {
            result += U'\t';
        } else if (token == "slash") {
            result += U'/';
        } else if (token == "comma") {
            result += U',';
        } else if (token == "pipe") {
            result += U'|';
        } else if (token == "colon") {
            result += U':';
        } else if (token == "semicolon") {
            result += U';';
        } else if (token == "hyphen") {
            result += U'-';
        } else if (token.size() == 1) {
            result += static_cast<char32_t>(token.front());
        } else {
            throw std::invalid_argument{
                std::format("unknown separator token '{}'. Use named tokens or one-character literals.", token)};
        }
    }
    if (result.empty()) {
        throw std::invalid_argument{"separator list must not be empty."};
    }
    return result;
}


auto CommandLineHelpDemo::parseTabStopList(const std::string_view value) -> std::vector<int> {
    auto result = std::vector<int>{};
    for (const auto token : splitCommaSeparated(value)) {
        if (token.empty()) {
            continue;
        }
        if (token == "wrapped") {
            result.push_back(ParagraphOptions::cTabWrappedLineIndent);
            continue;
        }
        result.push_back(parseIntInRange(token, "tab stop", 1, 80));
    }
    if (result.empty()) {
        throw std::invalid_argument{"tab stop list must contain at least one value."};
    }
    return result;
}


auto CommandLineHelpDemo::trim(const std::string_view text) noexcept -> std::string_view {
    auto begin = text.find_first_not_of(" \t");
    if (begin == std::string_view::npos) {
        return {};
    }
    const auto end = text.find_last_not_of(" \t");
    return text.substr(begin, end - begin + 1);
}


auto CommandLineHelpDemo::splitCommaSeparated(const std::string_view text) -> std::vector<std::string_view> {
    auto result = std::vector<std::string_view>{};
    auto start = std::size_t{0};
    while (start <= text.size()) {
        const auto comma = text.find(',', start);
        const auto part = comma == std::string_view::npos ? text.substr(start) : text.substr(start, comma - start);
        result.push_back(trim(part));
        if (comma == std::string_view::npos) {
            break;
        }
        start = comma + 1;
    }
    return result;
}


auto CommandLineHelpDemo::buildHelpDocument(const DemoConfig &) -> HelpDocument {
    auto document = HelpDocument{};
    document.usageLine.append(
        fg::BrightGreen, "command-line-help", fg::White, " [", fg::Magenta, "<options>", fg::White, "]");

    document.sections.push_back(
        HelpSection{
            .title = String{"Summary", Color{fg::BrightWhite}},
            .kind = HelpSectionKind::Paragraphs,
            .paragraphs = {buildIntroParagraph()},
        });
    document.sections.push_back(
        HelpSection{
            .title = String{"Preview Paragraph", Color{fg::BrightWhite}},
            .kind = HelpSectionKind::Paragraphs,
            .paragraphs = {buildPreviewParagraph()},
        });

    auto optionParagraphs = std::vector<String>{};
    optionParagraphs.reserve(optionSpecs().size());
    for (const auto &spec : optionSpecs()) {
        optionParagraphs.push_back(buildOptionParagraph(spec));
    }
    document.sections.push_back(
        HelpSection{
            .title = String{"Options", Color{fg::BrightWhite}},
            .kind = HelpSectionKind::Options,
            .paragraphs = std::move(optionParagraphs),
        });
    document.sections.push_back(
        HelpSection{
            .title = String{"Try It", Color{fg::BrightWhite}},
            .kind = HelpSectionKind::Paragraphs,
            .paragraphs = {buildClosingParagraph()},
        });
    return document;
}


auto CommandLineHelpDemo::buildIntroParagraph() -> String {
    auto result = String{};
    result.append(
        fg::White,
        "This demo prints a fictive command-line help page that adapts to the terminal width. On wider terminals it "
        "uses ",
        fg::BrightWhite,
        "Terminal::printParagraph()",
        fg::White,
        " to keep descriptions aligned, wrapped, and easy to scan. Below 40 columns it intentionally falls back to "
        "plain line-oriented output so you can compare both styles.");
    return result;
}


auto CommandLineHelpDemo::buildPreviewParagraph() -> String {
    auto result = String{};
    result.append(
        bg::Blue,
        fg::BrightYellow,
        "Preview\t",
        fg::BrightWhite,
        "This paragraph keeps a visible background so options like background mode, wrap markers, word breaks, "
        "ellipsis handling, and alignment become easier to inspect while you adjust the rendering settings.");
    return result;
}


auto CommandLineHelpDemo::buildClosingParagraph() -> String {
    auto result = String{};
    result.append(
        fg::White,
        "Experiment with ",
        fg::BrightCyan,
        "--terminal-width",
        fg::White,
        ", wrap markers, custom tab stops, and the preview paragraph settings to see how a real command help screen "
        "can stay readable across narrow and wide terminals.");
    return result;
}


auto CommandLineHelpDemo::buildOptionParagraph(const OptionSpec &spec) -> String {
    auto result = spec.coloredSignature();
    result.append("\t");
    result.append(fg::White, spec.description);
    return result;
}


auto CommandLineHelpDemo::widestSignatureWidth() -> int {
    auto result = 0;
    for (const auto &spec : optionSpecs()) {
        result = std::max(result, spec.signatureWidth());
    }
    return result;
}


void CommandLineHelpDemo::renderErrors(Terminal &terminal, const std::vector<std::string> &errors) noexcept {
    if (errors.empty()) {
        return;
    }
    for (const auto &error : errors) {
        terminal.printLine(fg::BrightRed, "Argument error: ", fg::White, error);
    }
    terminal.writeLineBreak();
}


void CommandLineHelpDemo::renderDocument(
    Terminal &terminal, const HelpDocument &document, const DemoConfig &config) noexcept {

    if (terminal.size().width() < 40) {
        renderNarrowDocument(terminal, document, config);
    } else {
        renderWideDocument(terminal, document, config);
    }
}


void CommandLineHelpDemo::renderWideDocument(
    Terminal &terminal, const HelpDocument &document, const DemoConfig &config) noexcept {

    printUsageLine(terminal, document.usageLine);
    terminal.writeLineBreak();

    const auto optionColumn = config.effectiveDescriptionColumn(widestSignatureWidth());
    for (const auto &section : document.sections) {
        printSectionTitle(terminal, section.title);
        const auto paragraphOptions = section.kind == HelpSectionKind::Options
            ? config.optionParagraphOptions(optionColumn)
            : config.bodyParagraphOptions();
        for (const auto &paragraph : section.paragraphs) {
            terminal.printParagraph(paragraph, paragraphOptions);
        }
        terminal.writeLineBreak();
    }
}


void CommandLineHelpDemo::renderNarrowDocument(
    Terminal &terminal, const HelpDocument &document, const DemoConfig &) noexcept {

    printUsageLine(terminal, document.usageLine);
    terminal.writeLineBreak();
    for (const auto &section : document.sections) {
        printSectionTitle(terminal, section.title);
        if (section.kind == HelpSectionKind::Options) {
            for (const auto &spec : optionSpecs()) {
                printNarrowOption(terminal, spec);
            }
        } else {
            for (const auto &paragraph : section.paragraphs) {
                printNarrowParagraph(terminal, paragraph);
            }
        }
        terminal.writeLineBreak();
    }
}


void CommandLineHelpDemo::printUsageLine(Terminal &terminal, const String &usageLine) noexcept {
    terminal.printLine(usageLine);
}


void CommandLineHelpDemo::printSectionTitle(Terminal &terminal, const String &title) noexcept {
    terminal.printLine(fg::BrightWhite, title);
}


void CommandLineHelpDemo::printNarrowParagraph(Terminal &terminal, const String &paragraph) noexcept {
    terminal.printLine(paragraph);
    terminal.writeLineBreak();
}


void CommandLineHelpDemo::printNarrowOption(Terminal &terminal, const OptionSpec &spec) noexcept {
    terminal.printLine(spec.coloredSignature());
    terminal.printLine(fg::White, "    ", spec.description);
}


void CommandLineHelpDemo::prepareTerminal(Terminal &terminal, const DemoConfig &config) noexcept {
    terminal.testScreenSize();
    if (config.terminalWidthOverride.has_value()) {
        terminal.setSize(Size{*config.terminalWidthOverride, terminal.size().height()});
    }
}


}
