// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/Terminal.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>


class StdoutCapture final {
public:
    StdoutCapture() : _originalBuffer{std::cout.rdbuf(_buffer.rdbuf())} {}

    ~StdoutCapture() { std::cout.rdbuf(_originalBuffer); }

    [[nodiscard]] auto str() const -> std::string { return _buffer.str(); }

private:
    std::ostringstream _buffer;
    std::streambuf *_originalBuffer{nullptr};
};


TESTED_TARGETS(Terminal)
class TerminalUpdateTest final : public el::UnitTest {
public:
    void testUpdateSettingsDefaultValues() {
        const auto settings = term::UpdateSettings{};

        REQUIRE_EQUAL(settings.minimumSize(), Size(0, 0));
        REQUIRE_EQUAL(settings.minimumSizeMark().charStr(), std::string{"X"});
        REQUIRE_EQUAL(settings.minimumSizeMark().color(), term::Color(term::fg::BrightRed, term::bg::Black));
        REQUIRE_EQUAL(settings.showCropMarks(), false);
        REQUIRE_EQUAL(settings.cropMarkRight().charStr(), std::string{"▶︎"});
        REQUIRE_EQUAL(settings.cropMarkRight().color(), term::Color(term::fg::BrightYellow, term::bg::Black));
        REQUIRE_EQUAL(settings.cropMarkBottom().charStr(), std::string{"▼"});
        REQUIRE_EQUAL(settings.cropMarkBottom().color(), term::Color(term::fg::BrightYellow, term::bg::Black));
    }

    void testUpdateSettingsSettersOverrideAllValues() {
        auto settings = term::UpdateSettings{};
        settings.setMinimumSize(Size{7, 6});
        settings.setMinimumSizeMark(term::Char{"!", term::Color{term::fg::Yellow, term::bg::Blue}});
        settings.setShowCropMarks(true);
        settings.setCropMarkRight(term::Char{">", term::Color{term::fg::Green, term::bg::Black}});
        settings.setCropMarkBottom(term::Char{"v", term::Color{term::fg::Cyan, term::bg::Black}});

        REQUIRE_EQUAL(settings.minimumSize(), Size(7, 6));
        REQUIRE_EQUAL(settings.minimumSizeMark().charStr(), std::string{"!"});
        REQUIRE_EQUAL(settings.minimumSizeMark().color(), term::Color(term::fg::Yellow, term::bg::Blue));
        REQUIRE_EQUAL(settings.showCropMarks(), true);
        REQUIRE_EQUAL(settings.cropMarkRight().charStr(), std::string{">"});
        REQUIRE_EQUAL(settings.cropMarkRight().color(), term::Color(term::fg::Green, term::bg::Black));
        REQUIRE_EQUAL(settings.cropMarkBottom().charStr(), std::string{"v"});
        REQUIRE_EQUAL(settings.cropMarkBottom().color(), term::Color(term::fg::Cyan, term::bg::Black));
    }

    void testUpdateScreenRendersVisibleBuffer() {
        const auto buffer = createBuffer({"AB", "CD"});
        auto terminal = term::Terminal{Size{4, 4}};
        terminal.setColorEnabled(false);

        const auto capture = StdoutCapture{};
        terminal.updateScreen(buffer);

        REQUIRE_EQUAL(capture.str(), std::string{"AB\nCD\n"});
    }

    void testUpdateScreenRendersCropMarksWhenEnabled() {
        const auto buffer = createBuffer({"ABC", "DEF", "GHI"});
        auto terminal = term::Terminal{Size{3, 3}};
        terminal.setColorEnabled(false);
        auto settings = term::UpdateSettings{};
        settings.setShowCropMarks(true);

        const auto capture = StdoutCapture{};
        terminal.updateScreen(buffer, settings);

        REQUIRE_EQUAL(capture.str(), std::string{"A▶︎\n▼▼\n"});
    }

    void testUpdateScreenRendersMinimumSizeMarkWhenTerminalIsTooSmall() {
        const auto buffer = createBuffer({"AB", "CD"});
        auto terminal = term::Terminal{Size{4, 4}};
        terminal.setColorEnabled(false);
        auto settings = term::UpdateSettings{};
        settings.setMinimumSize(Size{5, 5});

        const auto capture = StdoutCapture{};
        terminal.updateScreen(buffer, settings);

        REQUIRE_EQUAL(capture.str(), std::string{"X"});
    }

    void testClearScreenUsesConfiguredClearSequence() {
        auto terminal = term::Terminal{Size{4, 4}};
        terminal.setRefreshMode(term::Terminal::RefreshMode::Clear);

        const auto capture = StdoutCapture{};
        terminal.clearScreen();

        REQUIRE_EQUAL(capture.str(), std::string{"\x1b[2J\x1b[1;1H"});
    }

    void testClearScreenUsesConfiguredOverwriteSequence() {
        auto terminal = term::Terminal{Size{4, 4}};
        terminal.setRefreshMode(term::Terminal::RefreshMode::Overwrite);

        const auto capture = StdoutCapture{};
        terminal.clearScreen();

        REQUIRE_EQUAL(capture.str(), std::string{"\x1b[H"});
    }

    void testClearScreenUsesStrongSequenceAfterManualResize() {
        auto terminal = term::Terminal{Size{4, 4}};
        terminal.setRefreshMode(term::Terminal::RefreshMode::Overwrite);
        terminal.setSize(Size{6, 5});

        const auto capture = StdoutCapture{};
        terminal.clearScreen();

        REQUIRE_EQUAL(capture.str(), std::string{"\x1b[2J\x1b[1;1H"});
    }

    void testClearScreenUsesStrongSequenceOnlyOnceAfterManualResize() {
        auto terminal = term::Terminal{Size{4, 4}};
        terminal.setRefreshMode(term::Terminal::RefreshMode::Overwrite);
        terminal.setSize(Size{6, 5});

        auto firstOutput = std::string{};
        {
            const auto capture = StdoutCapture{};
            terminal.clearScreen();
            firstOutput = capture.str();
        }
        REQUIRE_EQUAL(firstOutput, std::string{"\x1b[2J\x1b[1;1H"});

        auto secondOutput = std::string{};
        {
            const auto capture = StdoutCapture{};
            terminal.clearScreen();
            secondOutput = capture.str();
        }
        REQUIRE_EQUAL(secondOutput, std::string{"\x1b[H"});
    }

private:
    [[nodiscard]] static auto createBuffer(const std::initializer_list<std::string_view> rows) -> term::Buffer {
        auto width = 0;
        for (const auto row : rows) {
            width = std::max(width, static_cast<int>(row.size()));
        }
        auto buffer = term::Buffer{Size{width, static_cast<int>(rows.size())}};
        auto y = 0;
        for (const auto row : rows) {
            auto x = 0;
            for (const auto character : row) {
                buffer.set(Position{x, y}, term::Char{std::string(1, character), term::Color{}});
                ++x;
            }
            ++y;
        }
        return buffer;
    }
};
