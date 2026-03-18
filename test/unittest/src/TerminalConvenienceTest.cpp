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

    void testPrintAndPrintLineAcceptAllSupportedArgumentKindsInTextMode() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = createTerminal(backend);
        terminal->setOutputMode(Terminal::OutputMode::Text);

        terminal->print(fg::Green, bg::Blue, Char{U'A'}, String{"B"}, std::string{"C"}, std::string_view{"D"}, "E");
        terminal->printLine("F");
        terminal->flush();

        REQUIRE_EQUAL(backend->output(), std::string{"ABCDEF\n"});
    }
};
