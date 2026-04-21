// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <atomic>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

TESTED_TARGETS(String)
class StringTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testUtf8StringIsSplitIntoTerminalCharacters() {
        const auto text = String{"äöü"};
        REQUIRE_EQUAL(text.size(), std::size_t{3});
        REQUIRE_EQUAL(text[0], U'ä');
        REQUIRE_EQUAL(text[1], U'ö');
        REQUIRE_EQUAL(text[2], U'ü');
    }

    void testUtf32StringIsSplitIntoTerminalCharacters() {
        const auto text = String{U"äöü"};
        REQUIRE_EQUAL(text.size(), std::size_t{3});
        REQUIRE_EQUAL(text[0], U'ä');
        REQUIRE_EQUAL(text[1], U'ö');
        REQUIRE_EQUAL(text[2], U'ü');
    }

    void testDisplayWidth() {
        const auto text = String{"A中B"};
        REQUIRE_EQUAL(text.displayWidth(), 4);
    }

    void testCountAndIndexOfRespectColorSpecificAndColorAgnosticSearches() {
        auto text = String{};
        text.append(Char{U'A', fg::Red, bg::Black});
        text.append(Char{U'B', fg::Blue, bg::Black});
        text.append(Char{U'A', fg::Green, bg::Black});
        text.append(Char{U"e\u0301"});

        REQUIRE_EQUAL(text.count(Char{U'A', fg::Red, bg::Black}), std::size_t{1});
        REQUIRE_EQUAL(text.count(Char{U'A', fg::Green, bg::Black}), std::size_t{1});
        REQUIRE_EQUAL(text.count(Char{U'A', fg::White, bg::Black}), std::size_t{0});
        REQUIRE_EQUAL(text.count(U'A'), std::size_t{2});
        REQUIRE_EQUAL(text.count(U'e'), std::size_t{0});

        REQUIRE_EQUAL(text.indexOf(Char{U'A', fg::Green, bg::Black}), std::size_t{2});
        REQUIRE_EQUAL(text.indexOf(Char{U'A', fg::Red, bg::Black}, 1), String::npos);
        REQUIRE_EQUAL(text.indexOf(U'A'), std::size_t{0});
        REQUIRE_EQUAL(text.indexOf(U'A', 1), std::size_t{2});
        REQUIRE_EQUAL(text.indexOf(U'Z'), String::npos);
        REQUIRE_EQUAL(text.indexOf(U'B', 99), String::npos);
    }

    void testSubstrReturnsRequestedWindowAndClampsBounds() {
        const auto text = String{"AB界D"};

        const auto middle = text.substr(1, 2);
        const auto tail = text.substr(2);

        REQUIRE_EQUAL(middle.size(), std::size_t{2});
        REQUIRE_EQUAL(middle[0], U'B');
        REQUIRE_EQUAL(middle[1], U'界');
        REQUIRE_EQUAL(tail.size(), std::size_t{2});
        REQUIRE_EQUAL(tail[0], U'界');
        REQUIRE_EQUAL(tail[1], U'D');
        REQUIRE(text.substr(2, 0).empty());
        REQUIRE(text.substr(99).empty());
    }

    void testTrimRemovesMatchingCharactersFromBothEndsOnly() {
        const auto text = String{"\n\t alpha \r\n"};
        const auto trimmed = text.trimmed(U"\n\r\t ");

        REQUIRE_EQUAL(trimmed.size(), std::size_t{5});
        REQUIRE_EQUAL(trimmed[0], U'a');
        REQUIRE_EQUAL(trimmed[4], U'a');
    }

    void testWrapIntoLinesUsesTerminalWidth() {
        const auto text = String{"漢字テスト"};
        const auto lines = text.wrapIntoLines(4);
        REQUIRE_EQUAL(lines.size(), std::size_t{3});
        REQUIRE_EQUAL(lines[0][0], U'漢');
        REQUIRE_EQUAL(lines[0][1], U'字');
        REQUIRE_EQUAL(lines[1][0], U'テ');
        REQUIRE_EQUAL(lines[1][1], U'ス');
        REQUIRE_EQUAL(lines[2][0], U'ト');
    }

    void testWrapIntoLinesUsesSingleParagraphSpacingByDefault() {
        const auto text = String{"alpha beta\ngamma"};
        const auto lines = text.wrapIntoLines(6);
        REQUIRE_EQUAL(lines.size(), std::size_t{3});
        REQUIRE_EQUAL(lines[0][0], U'a');
        REQUIRE_EQUAL(lines[0][4], U'a');
        REQUIRE_EQUAL(lines[1][0], U'b');
        REQUIRE_EQUAL(lines[2][0], U'g');
    }

    void testWrapIntoLinesAddsExtraSpacingForDoubleParagraphSpacing() {
        const auto text = String{"alpha beta\ngamma"};
        const auto lines = text.wrapIntoLines(6, ParagraphSpacing::DoubleLine);

        REQUIRE_EQUAL(lines.size(), std::size_t{4});
        REQUIRE_EQUAL(lines[0][0], U'a');
        REQUIRE_EQUAL(lines[1][0], U'b');
        REQUIRE(lines[2].empty());
        REQUIRE_EQUAL(lines[3][0], U'g');
    }

    void testWrapIntoLinesPreservesExplicitEmptyParagraphs() {
        const auto lines = String{"alpha\n\nbeta"}.wrapIntoLines(10);

        REQUIRE_EQUAL(renderLines(lines), std::vector<std::string>({"alpha", "", "beta"}));
    }

    void testSplitLinesPreservesEmptyLinesWithoutTrailingEmptyLine() {
        const auto lines = String{"alpha\n\nbeta\n"}.splitLines();

        REQUIRE_EQUAL(lines.size(), std::size_t{3});
        REQUIRE_EQUAL(lines[0].size(), std::size_t{5});
        REQUIRE_EQUAL(lines[0][0], U'a');
        REQUIRE(lines[1].empty());
        REQUIRE_EQUAL(lines[2].size(), std::size_t{4});
        REQUIRE_EQUAL(lines[2][0], U'b');
    }

    void testSplitLinesHandlesEmptyLeadingAndSingleNewlineStrings() {
        REQUIRE(String{}.splitLines().empty());

        const auto leadingEmptyLine = String{"\nalpha"}.splitLines();
        REQUIRE_EQUAL(leadingEmptyLine.size(), std::size_t{2});
        REQUIRE(leadingEmptyLine[0].empty());
        REQUIRE_EQUAL(leadingEmptyLine[1][0], U'a');

        const auto singleNewline = String{"\n"}.splitLines();
        REQUIRE_EQUAL(singleNewline.size(), std::size_t{1});
        REQUIRE(singleNewline[0].empty());
    }

    void testWrapIntoLinesPreservesColoredSpacingBetweenWords() {
        auto text = String{};
        text.append(Char{U'A', fg::Red, bg::Black});
        text.append(Char{U' ', fg::Inherited, bg::Blue});
        text.append(Char{U' ', fg::Inherited, bg::Cyan});
        text.append(Char{U'B', fg::Green, bg::Black});

        const auto lines = text.wrapIntoLines(8);

        REQUIRE_EQUAL(lines.size(), std::size_t{1});
        REQUIRE_EQUAL(lines[0].size(), std::size_t{4});
        REQUIRE_EQUAL(lines[0][1].color(), Color(fg::Inherited, bg::Blue));
        REQUIRE_EQUAL(lines[0][2].color(), Color(fg::Inherited, bg::Cyan));
    }

    void testWrapIntoLinesTrimsOuterSpacingAndKeepsInternalSpacingThatFits() {
        const auto lines = String{"  alpha   beta  "}.wrapIntoLines(12);

        REQUIRE_EQUAL(renderLines(lines), std::vector<std::string>({"alpha   beta"}));
    }

    void testWrapIntoLinesSplitsOversizedWordsIntoStandaloneChunks() {
        const auto lines = String{"supercalifragilistic test"}.wrapIntoLines(5);

        REQUIRE_EQUAL(renderLines(lines), std::vector<std::string>({"super", "calif", "ragil", "istic", "test"}));
    }

    void testWrapIntoLinesSplitsWideWordsAtCharacterBoundaries() {
        const auto lines = String{"漢字abcde"}.wrapIntoLines(5);

        REQUIRE_EQUAL(renderLines(lines), std::vector<std::string>({"漢字a", "bcde"}));
    }

    void testWrapIntoLinesDoesNotDuplicateWordsBetweenTokens() {
        const auto lines =
            String{"Text Gallery  |  alignment, wrapping, wide characters, and bitmap fonts"}.wrapIntoLines(200);

        REQUIRE_EQUAL(
            renderLines(lines),
            std::vector<std::string>({"Text Gallery  |  alignment, wrapping, wide characters, and bitmap fonts"}));
    }

    void testTerminalLinesCountsWrappedAndExplicitLineBreaks() {
        REQUIRE_EQUAL(String{}.terminalLines(4), 0);
        REQUIRE_EQUAL(String{"AA BB"}.terminalLines(2), 3);
        REQUIRE_EQUAL(String{"AA\nBB"}.terminalLines(2), 3);
        REQUIRE_EQUAL(String{"AA\n"}.terminalLines(2), 2);
    }

    void testNaturalTextSizeMeasuresExplicitLines() {
        REQUIRE_EQUAL(String{}.naturalTextSize(), (Size{1, 1}));
        REQUIRE_EQUAL(String{"A界\nBC"}.naturalTextSize(), (Size{3, 2}));
        REQUIRE_EQUAL(String{"AB\n\nC\n"}.naturalTextSize(), (Size{2, 3}));
    }

    void testWrappedTextHeightUsesParagraphLayoutAndMargins() {
        auto options = TextOptions{};
        options.setMargins(Margins{1, 0, 2, 0});

        REQUIRE_EQUAL(String{"alpha beta gamma"}.wrappedTextHeight(10, options), 5);
    }

    void testWrappedTextHeightUsesPlainOutputFallbackForInvalidParagraphSettings() {
        auto options = TextOptions{};
        options.setLineBreakEndMark(String{">>"});
        options.setOnError(ParagraphOnError::PlainOutput);

        REQUIRE_EQUAL(String{"AA BB"}.wrappedTextHeight(2, options), 2);

        options.setOnError(ParagraphOnError::Empty);
        REQUIRE_EQUAL(String{"AA BB"}.wrappedTextHeight(2, options), 0);
    }

    void testCombiningCharactersStayInOneCell() {
        const auto text = String{bytes({0x65, 0xCC, 0x81})};
        REQUIRE_EQUAL(text.size(), std::size_t{1});
        REQUIRE_EQUAL(text.displayWidth(), 1);
        REQUIRE_EQUAL(text[0].charStr(), bytes({0x65, 0xCC, 0x81}));
    }

    void testControlCodesAreFilteredExceptTabAndNewline() {
        const auto text = String{"A\r\x01\t\nB"};

        REQUIRE_EQUAL(text.size(), std::size_t{4});
        REQUIRE_EQUAL(text[0], U'A');
        REQUIRE_EQUAL(text[1].charStr(), std::string{"\t"});
        REQUIRE_EQUAL(text[2].charStr(), std::string{"\n"});
        REQUIRE_EQUAL(text[3], U'B');
    }

    void testInvalidUtf8Fails() {
        REQUIRE_THROWS_AS(std::invalid_argument, (String{bytes({0xC3}), EncodingErrors::Throw}));
    }

    void testUtf8StringCanIgnoreEncodingErrors() {
        const auto text = String{bytes({0x41, 0xC3, 0x42}), EncodingErrors::Ignore};

        REQUIRE_EQUAL(text.size(), std::size_t{2});
        REQUIRE_EQUAL(text[0], U'A');
        REQUIRE_EQUAL(text[1], U'B');
    }

    void testUtf8StringCanReplaceEncodingErrorsDeterministically() {
        const auto text = String{bytes({0x41, 0xE2, 0x28, 0xA1, 0x42}), EncodingErrors::Replace};

        REQUIRE_EQUAL(text.size(), std::size_t{5});
        REQUIRE_EQUAL(text[0], U'A');
        REQUIRE_EQUAL(text[1], U'\uFFFD');
        REQUIRE_EQUAL(text[2], U'(');
        REQUIRE_EQUAL(text[3], U'\uFFFD');
        REQUIRE_EQUAL(text[4], U'B');
    }

    void testStyledUtf8StringCanReplaceEncodingErrors() {
        auto attributes = CharAttributes{};
        attributes.setBold(true);
        const auto style = CharStyle{Color{fg::Red, bg::Blue}, attributes};
        const auto text = String{bytes({0xC3}), style, EncodingErrors::Replace};

        REQUIRE_EQUAL(text.size(), std::size_t{1});
        REQUIRE_EQUAL(text[0], U'\uFFFD');
        REQUIRE_EQUAL(text[0].style(), style);
    }

    void testAppendStyledAppendsUtf32TextWithoutChangingExistingCharacters() {
        auto text = String{"A"};
        auto attributes = CharAttributes{};
        attributes.setBold(true);
        const auto style = CharStyle{Color{fg::BrightGreen, bg::Blue}, attributes};

        text.appendStyled(U"BC", style);

        REQUIRE_EQUAL(text.size(), std::size_t{3});
        REQUIRE_EQUAL(text[0], U'A');
        REQUIRE_EQUAL(text[1], U'B');
        REQUIRE_EQUAL(text[2], U'C');
        REQUIRE_EQUAL(text[1].style(), style);
        REQUIRE_EQUAL(text[2].style(), style);
    }

    void testAppendRangeAndAppendStyledRangeCopyOnlyTheRequestedCharacters() {
        auto source = String{};
        source.append(Char{U'A', fg::Red, bg::Black});
        source.append(Char{U'B', fg::Green, bg::Black});
        source.append(Char{U'C', fg::Blue, bg::Black});
        source.append(Char{U'D', fg::Yellow, bg::Black});

        auto copied = String{};
        copied.appendRange(source, 1, 2);
        const auto expectedB = Char{U'B', fg::Green, bg::Black};
        const auto expectedC = Char{U'C', fg::Blue, bg::Black};

        REQUIRE_EQUAL(copied.size(), std::size_t{2});
        REQUIRE_EQUAL(copied[0], expectedB);
        REQUIRE_EQUAL(copied[1], expectedC);

        auto attributes = CharAttributes{};
        attributes.setUnderline(true);
        const auto style = CharStyle{Color{fg::BrightWhite, bg::BrightBlack}, attributes};
        auto sourceWithInherited = String{};
        sourceWithInherited.append(Char{U'B', fg::Inherited, bg::Inherited});
        sourceWithInherited.append(Char{U'C', fg::Inherited, bg::Inherited});
        auto restyled = String{};
        restyled.appendRangeWithBaseStyle(sourceWithInherited, 0, 2, style);

        REQUIRE_EQUAL(restyled.size(), std::size_t{2});
        REQUIRE_EQUAL(restyled[0], U'B');
        REQUIRE_EQUAL(restyled[1], U'C');
        REQUIRE_EQUAL(restyled[0].style(), style);
        REQUIRE_EQUAL(restyled[1].style(), style);
    }

    void testStringViewAppendOperatorsReuseReadOnlyRanges() {
        const auto source = String{"ABCDE"};
        const auto middle = StringView{source}.substr(1, 3);

        auto appended = String{"A"};
        appended += middle.substr(0, 2);

        const auto combined = String{"A"} + middle.substr(2, 1);

        REQUIRE_EQUAL(render(appended), std::string{"ABC"});
        REQUIRE_EQUAL(render(combined), std::string{"AD"});
    }

    void testCopyOnWriteKeepsCopiedStringsIndependent() {
        const auto source = String{"alpha"};
        auto copy = source;

        copy[0] = Char{U'A'};
        copy.append(Char{U'!'});

        REQUIRE_EQUAL(render(source), std::string{"alpha"});
        REQUIRE_EQUAL(render(copy), std::string{"Alpha!"});
    }

    void testSubstrSharesStorageUntilTheSliceIsModified() {
        const auto source = String{"ABCDE"};
        auto slice = source.substr(1, 3);

        slice[0] = Char{U'X'};

        REQUIRE_EQUAL(render(source), std::string{"ABCDE"});
        REQUIRE_EQUAL(render(slice), std::string{"XCD"});
    }

    void testMutableIteratorsDetachBeforeWriting() {
        const auto source = String{"ABCD"};
        auto copy = source;

        *copy.begin() = Char{U'Z'};

        REQUIRE_EQUAL(render(source), std::string{"ABCD"});
        REQUIRE_EQUAL(render(copy), std::string{"ZBCD"});
    }

    void testReserveAndClearDoNotModifySharedCopies() {
        const auto source = String{"ABCD"};
        auto copy = source;

        copy.reserve(64);
        copy.clear();

        REQUIRE_EQUAL(render(source), std::string{"ABCD"});
        REQUIRE(copy.empty());
    }

    void testSharedCopiesSupportConcurrentReads() {
        const auto source = String{"alpha beta gamma"};
        auto readFailed = std::atomic<bool>{false};
        auto worker = [&source, &readFailed]() {
            for (auto iteration = 0; iteration < 2'000; ++iteration) {
                const auto copy = source;
                if (copy.displayWidth() != 16 || copy.indexOf(U'b') != 6 || copy.substr(6, 4).displayWidth() != 4) {
                    readFailed.store(true);
                }
            }
        };

        auto a = std::thread{worker};
        auto b = std::thread{worker};
        auto c = std::thread{worker};

        a.join();
        b.join();
        c.join();

        REQUIRE_FALSE(readFailed.load());
        REQUIRE_EQUAL(render(source), std::string{"alpha beta gamma"});
    }

    void testCopiedStringsCanMutateIndependentlyOnMultipleThreads() {
        const auto source = String{"Base"};
        auto left = source;
        auto right = source;

        auto leftDone = std::atomic<bool>{false};
        auto rightDone = std::atomic<bool>{false};

        auto leftWorker = std::thread{[&left, &leftDone]() {
            left.clear();
            left.append("left");
            left[0] = Char{U'L'};
            leftDone.store(true);
        }};
        auto rightWorker = std::thread{[&right, &rightDone]() {
            right.clear();
            right.append("right");
            right[0] = Char{U'R'};
            rightDone.store(true);
        }};

        leftWorker.join();
        rightWorker.join();

        REQUIRE(leftDone.load());
        REQUIRE(rightDone.load());
        REQUIRE_EQUAL(render(source), std::string{"Base"});
        REQUIRE_EQUAL(render(left), std::string{"Left"});
        REQUIRE_EQUAL(render(right), std::string{"Right"});
    }

private:
    [[nodiscard]] static auto render(const String &text) -> std::string {
        auto result = std::string{};
        for (const auto &character : text) {
            character.appendTo(result);
        }
        return result;
    }

    [[nodiscard]] static auto renderLines(const StringLines &lines) -> std::vector<std::string> {
        auto result = std::vector<std::string>{};
        result.reserve(lines.size());
        for (const auto &line : lines) {
            auto text = std::string{};
            for (const auto &character : line) {
                character.appendTo(text);
            }
            result.push_back(text);
        }
        return result;
    }
};
