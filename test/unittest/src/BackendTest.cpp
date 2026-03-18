// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TerminalTestSupport.hpp"
#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <string_view>


TESTED_TARGETS(Backend)
class BackendTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testTerminalUsesEmitColorWhenColorCodesAreUnavailable() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        backend->_supportsColorCodes = false;
        auto terminal = createTerminal(backend);

        REQUIRE_FALSE(terminal->lineBufferEnabled());

        terminal->write(Char{U'A', fg::Red, bg::Blue});
        terminal->flush();

        REQUIRE_EQUAL(backend->_emittedColors.size(), std::size_t{1});
        REQUIRE_EQUAL(backend->_emittedColors[0], Color(fg::Red, bg::Blue));
        REQUIRE_EQUAL(backend->output(), std::string{"A"});
        REQUIRE(backend->output().find("\x1b[") == std::string::npos);
    }

    void testTerminalUsesCursorFallbackHooksWhenCursorCodesAreUnavailable() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        backend->_supportsCursorCodes = false;
        auto terminal = createTerminal(backend);

        REQUIRE_FALSE(terminal->lineBufferEnabled());

        terminal->moveLeft(2);
        terminal->moveRight(3);
        terminal->moveUp(4);
        terminal->moveDown(5);
        terminal->moveTo(Position{6, 7});
        terminal->moveHome();
        terminal->clearScreen();
        terminal->flush();

        REQUIRE_EQUAL(backend->output(), std::string{});
        REQUIRE_EQUAL(backend->_clearScreenCallCount, 1);
        REQUIRE_EQUAL(backend->_cursorMoves.size(), std::size_t{6});
        REQUIRE_EQUAL(backend->_cursorMoves[0], (TerminalTestBackend::CursorMove{Position{-2, 0}, MoveMode::Relative}));
        REQUIRE_EQUAL(backend->_cursorMoves[1], (TerminalTestBackend::CursorMove{Position{3, 0}, MoveMode::Relative}));
        REQUIRE_EQUAL(backend->_cursorMoves[2], (TerminalTestBackend::CursorMove{Position{0, -4}, MoveMode::Relative}));
        REQUIRE_EQUAL(backend->_cursorMoves[3], (TerminalTestBackend::CursorMove{Position{0, 5}, MoveMode::Relative}));
        REQUIRE_EQUAL(backend->_cursorMoves[4], (TerminalTestBackend::CursorMove{Position{6, 7}, MoveMode::Absolute}));
        REQUIRE_EQUAL(backend->_cursorMoves[5], (TerminalTestBackend::CursorMove{Position{0, 0}, MoveMode::Absolute}));
    }

    void testTerminalUsesCursorVisibilityFallbackHookWhenVisibilityCodesAreUnavailable() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        backend->_supportsCursorVisibilityCodes = false;
        auto terminal = createTerminal(backend);

        terminal->setCursorVisible(false);
        terminal->flush();

        REQUIRE_EQUAL(backend->_cursorVisibilityChanges.size(), std::size_t{1});
        REQUIRE_FALSE(backend->_cursorVisibilityChanges[0]);
        REQUIRE_EQUAL(backend->output(), std::string{});

        backend->clearRecordedOperations();
        terminal->setCursorVisible(true);
        terminal->flush();

        REQUIRE_EQUAL(backend->_cursorVisibilityChanges.size(), std::size_t{1});
        REQUIRE(backend->_cursorVisibilityChanges[0]);
        REQUIRE_EQUAL(backend->output(), std::string{});
    }

    void testTextOutputModeSuppressesBackendControlHooks() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        backend->_supportsColorCodes = false;
        backend->_supportsCursorCodes = false;
        backend->_supportsCursorVisibilityCodes = false;
        auto terminal = createTerminal(backend, Size{2, 1});

        terminal->setOutputMode(Terminal::OutputMode::Text);
        terminal->clearScreen();
        terminal->moveHome();
        terminal->setCursorVisible(false);
        terminal->setColor(Color{fg::Red, bg::Blue});
        terminal->updateScreen(createBuffer({"HI"}));

        REQUIRE_EQUAL(backend->output(), std::string{"HI\n"});
        REQUIRE_EQUAL(backend->_emittedColors.size(), std::size_t{0});
        REQUIRE_EQUAL(backend->_cursorMoves.size(), std::size_t{0});
        REQUIRE_EQUAL(backend->_clearScreenCallCount, 0);
        REQUIRE_EQUAL(backend->_cursorVisibilityChanges.size(), std::size_t{0});
    }

    void testRestoreScreenUsesBackendFallbacksWhenCodesAreUnavailable() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        backend->_supportsColorCodes = false;
        backend->_supportsCursorVisibilityCodes = false;
        auto terminal = createTerminal(backend);

        terminal->setColor(Color{fg::Red, bg::Blue});
        terminal->flush();
        backend->clearRecordedOperations();

        terminal->restoreScreen();

        REQUIRE_EQUAL(backend->_emittedColors.size(), std::size_t{1});
        REQUIRE_EQUAL(backend->_emittedColors[0], Color::reset());
        REQUIRE_EQUAL(backend->_cursorVisibilityChanges.size(), std::size_t{1});
        REQUIRE(backend->_cursorVisibilityChanges[0]);
        REQUIRE_EQUAL(backend->output(), std::string{});
        REQUIRE_EQUAL(backend->_restorePlatformCallCount, 1);
    }

    void testResizeClearsTheScreenExactlyOnceBeforeTheNextUpdate() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        backend->_supportsCursorCodes = false;
        auto terminal = createTerminal(backend, Size{4, 1});

        terminal->updateScreen(createBuffer({"ABCD"}));
        backend->clearRecordedOperations();

        terminal->setSize(Size{5, 1});
        terminal->updateScreen(createBuffer({"ABCDE"}));

        REQUIRE_EQUAL(backend->_clearScreenCallCount, 1);
        REQUIRE_EQUAL(backend->_cursorMoves.size(), std::size_t{1});
        REQUIRE_EQUAL(backend->_cursorMoves[0], (TerminalTestBackend::CursorMove{Position{0, 0}, MoveMode::Absolute}));
        REQUIRE_EQUAL(backend->output(), std::string{"ABCDE"});
    }
};
