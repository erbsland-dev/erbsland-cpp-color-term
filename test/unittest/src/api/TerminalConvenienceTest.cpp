// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TerminalTestSupport.hpp"
#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <string_view>

TESTED_TARGETS(Terminal)
class TerminalConvenienceTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testCustomBackendConstructorClampsTheSizeAndTracksTheSafeMarginFlag() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = Terminal{backend, Size{0, 5000}};
        const auto expectedSize = Size{1, 2048};

        REQUIRE_EQUAL(terminal.size(), expectedSize);
        REQUIRE(terminal.safeMarginEnabled());

        terminal.setSafeMarginEnabled(false);

        REQUIRE_FALSE(terminal.safeMarginEnabled());
        REQUIRE_EQUAL(terminal.size(), expectedSize);
    }

    void testColorAccessorsAndStringWritesUseTheTrackedTerminalColor() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend);
        terminal->setLineBufferEnabled(false);

        terminal->setForeground(Foreground::Inherited);
        terminal->setBackground(Background::Inherited);
        terminal->setForeground(fg::Red);
        terminal->setBackground(bg::Blue);
        terminal->write(String{"A"});
        terminal->write(Char{U'B', fg::Inherited, bg::Green});
        terminal->flush();

        REQUIRE_EQUAL(terminal->color(), Color(fg::Red, bg::Green));
        REQUIRE_EQUAL(backend->output(), std::string{"\x1b[31m\x1b[44mA\x1b[42mB"});
    }

    void testAttributeAccessorsAndStringWritesUseTheTrackedTerminalAttributes() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend);
        terminal->setLineBufferEnabled(false);

        terminal->setBold(true);
        terminal->setUnderline(true);
        terminal->write(String{"A"});

        auto attributes = CharAttributes{};
        attributes.setBold(false);
        terminal->write(Char{U'B', Color{}, attributes});
        terminal->flush();

        REQUIRE(terminal->charAttributes().isUnderline());
        REQUIRE_FALSE(terminal->charAttributes().isBold());
        REQUIRE_EQUAL(backend->output(), std::string{"\x1b[1m\x1b[4mA\x1b[22mB"});
    }

    void testPrintAndPrintLineAcceptAllSupportedArgumentKindsInTextMode() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend);
        terminal->setOutputMode(Terminal::OutputMode::Text);

        auto bold = CharAttributes{};
        bold.setBold(true);
        const auto style = CharStyle{Color{fg::Green, bg::Blue}, bold};
        terminal->print(style, Char{U'A'}, String{"B"}, std::string{"C"}, std::string_view{"D"}, "E");
        terminal->printLine("F");
        terminal->flush();

        REQUIRE_EQUAL(backend->output(), std::string{"ABCDEF\n"});
    }

    void testStyleWrappersExposeAndApplyTheTrackedWriterStyle() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend);
        terminal->setLineBufferEnabled(false);

        auto emphasis = CharAttributes{};
        emphasis.setUnderline(true);
        terminal->setStyle(CharStyle{Color{fg::Red, bg::Blue}, emphasis});
        terminal->print(
            CharStyle{Color{fg::Inherited, bg::Green}, CharAttributes{}.withFlag(CharAttributes::Underline, false)},
            "A");
        terminal->flush();

        REQUIRE_EQUAL(terminal->style().color(), Color(fg::Red, bg::Green));
        REQUIRE_FALSE(terminal->style().attributes().isUnderline());
        REQUIRE_EQUAL(backend->output(), std::string{"\x1b[31;44m\x1b[4m\x1b[42m\x1b[24mA"});
    }

    void testBoldAndDimUseStableResetAndReapplySequences() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend);
        terminal->setLineBufferEnabled(false);

        terminal->setBold(true);
        terminal->setDim(true);
        terminal->setBold(false);
        terminal->flush();

        REQUIRE_EQUAL(backend->output(), std::string{"\x1b[1m\x1b[22;1;2m\x1b[22;2m"});
    }

    void testPrintParagraphRendersWrapMarksAndReturnsTheWrittenLineCount() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend, Size{6, 4});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto options = ParagraphOptions{};
        options.setWrappedLineIndent(3);
        options.setLineBreakStartMark(String{">"});
        options.setLineBreakEndMark(String{"<"});

        const auto writtenLines = terminal->printParagraph("AA BB CC", options);
        terminal->flush();

        REQUIRE_EQUAL(writtenLines, 2);
        REQUIRE_EQUAL(backend->output(), std::string{"AA BB<\n   >CC\n"});
    }

    void testPrintParagraphUsesWrappedLineIndentForSpecialTabStops() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend, Size{8, 4});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto options = ParagraphOptions{};
        options.setWrappedLineIndent(4);
        options.setTabStops({ParagraphOptions::cTabWrappedLineIndent});

        const auto writtenLines = terminal->printParagraph("AA\tBB CC", options);
        terminal->flush();

        REQUIRE_EQUAL(writtenLines, 2);
        REQUIRE_EQUAL(backend->output(), std::string{"AA  BB\n    CC\n"});
    }

    void testPrintParagraphKeepsMixedSeparatorAndTabTokensInOrder() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend, Size{6, 4});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto options = ParagraphOptions{};
        options.setWordSeparators(U" ,");
        options.setTabStops({4});

        const auto writtenLines = terminal->printParagraph("AA,\tBBBB CC", options);
        terminal->flush();

        REQUIRE_EQUAL(writtenLines, 3);
        REQUIRE_EQUAL(backend->output(), std::string{"AA\n    B-\nBBB CC\n"});
    }

    void testPrintParagraphBreaksAtNonAdvancingTabStopsWhenRequested() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend, Size{12, 4});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto options = ParagraphOptions{};
        options.setWrappedLineIndent(6);
        options.setLineBreakEndMark(String{"<"});
        options.setTabStops({6});
        options.setTabOverflowBehavior(TabOverflowBehavior::LineBreak);

        const auto writtenLines = terminal->printParagraph("Heading\ttext", options);
        terminal->flush();

        REQUIRE_EQUAL(writtenLines, 2);
        REQUIRE_EQUAL(backend->output(), std::string{"Heading    <\n      text\n"});
    }

    void testPrintParagraphReplacesNonAdvancingTabsWithSpacesByDefault() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend, Size{12, 4});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto options = ParagraphOptions{};
        options.setTabStops({6});

        const auto writtenLines = terminal->printParagraph("Heading\ttext", options);
        terminal->flush();

        REQUIRE_EQUAL(writtenLines, 1);
        REQUIRE_EQUAL(backend->output(), std::string{"Heading text\n"});
    }

    void testPrintParagraphReplacesTabsWithSpacesWhenTabStopsAreExhaustedByDefault() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend, Size{5, 4});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto options = ParagraphOptions{};
        options.setTabStops({2});

        const auto writtenLines = terminal->printParagraph("A\tB\tC", options);
        terminal->flush();

        REQUIRE_EQUAL(writtenLines, 1);
        REQUIRE_EQUAL(backend->output(), std::string{"A B C\n"});
    }

    void testPrintParagraphTreatsTabsAsCollapsedWordSeparatorsForRightAlignedText() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend, Size{5, 4});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto options = ParagraphOptions{Alignment::Right};

        const auto writtenLines = terminal->printParagraph("A\tB", options);
        terminal->flush();

        REQUIRE_EQUAL(writtenLines, 1);
        REQUIRE_EQUAL(backend->output(), std::string{"  A B\n"});
    }

    void testPrintParagraphUsesTheConfiguredParagraphSpacing() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend, Size{4, 3});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto options = ParagraphOptions{};
        options.setParagraphSpacing(ParagraphSpacing::DoubleLine);

        const auto writtenLines = terminal->printParagraph("A", options);
        terminal->flush();

        REQUIRE_EQUAL(writtenLines, 2);
        REQUIRE_EQUAL(backend->output(), std::string{"A\n\n"});
    }

    void testPrintParagraphHonorsHorizontalMargins() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend, Size{6, 4});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto options = ParagraphOptions{};
        options.setMargins(Margins{1, 0});

        const auto writtenLines = terminal->printParagraph("AB CD", options);
        terminal->flush();

        REQUIRE_EQUAL(writtenLines, 2);
        REQUIRE_EQUAL(backend->output(), std::string{" AB\n CD\n"});
    }

    void testPrintParagraphTreatsNewlinesAsHardBreaksAndResetsWrapCounting() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend, Size{5, 4});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto options = ParagraphOptions{};
        options.setMaximumLineWraps(1);

        const auto writtenLines = terminal->printParagraph("AA BB CC DD EE\nFF GG HH II JJ", options);
        terminal->flush();

        REQUIRE_EQUAL(writtenLines, 4);
        REQUIRE_EQUAL(backend->output(), std::string{"AA BB\nCC…\nFF GG\nHH…\n"});
    }

    void testPrintParagraphKeepsRightSidePaddingWhenFullRightBackgroundIsRequested() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend, Size{4, 4});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto options = ParagraphOptions{};
        options.setBackgroundMode(ParagraphBackgroundMode::FullRight);

        const auto writtenLines = terminal->printParagraph("AB CD", options);
        terminal->flush();

        REQUIRE_EQUAL(writtenLines, 2);
        REQUIRE_EQUAL(backend->output(), std::string{"AB  \nCD  \n"});
    }

    void testPrintParagraphFallsBackToPlainOutputWhenRequested() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend, Size{2, 4});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto options = ParagraphOptions{};
        options.setLineBreakEndMark(String{">>"});
        options.setOnError(ParagraphOnError::PlainOutput);

        const auto writtenLines = terminal->printParagraph("AA BB", options);
        terminal->flush();

        REQUIRE_EQUAL(writtenLines, 3);
        REQUIRE_EQUAL(backend->output(), std::string{"AA BB\n"});
    }

    void testPrintParagraphUsesExactlyTwoLineBreaksForAnEmptyDoubleSpacedParagraph() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend, Size{4, 3});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto options = ParagraphOptions{};
        options.setParagraphSpacing(ParagraphSpacing::DoubleLine);

        const auto writtenLines = terminal->printParagraph("", options);
        terminal->flush();

        REQUIRE_EQUAL(writtenLines, 2);
        REQUIRE_EQUAL(backend->output(), std::string{"\n\n"});
    }

    void testPrintParagraphStringOverloadReplacesInvalidUtf8() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend, Size{3, 2});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        const auto text = bytes({0x41, 0xC3, 0x42});

        const auto writtenLines = terminal->printParagraph(text);
        terminal->flush();

        REQUIRE_EQUAL(writtenLines, 1);
        REQUIRE_EQUAL(backend->output(), std::string{"A�B\n"});
    }
};
