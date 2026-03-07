// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/unittest/UnitTest.hpp>

#include <array>

#include "TestHelper.hpp"

TESTED_TARGETS(Key)
class KeyPressTest final : public el::UnitTest {
public:
    void testDefaultConstructor() {
        term::Key key;
        REQUIRE_EQUAL(key.type(), term::Key::None);
        REQUIRE_EQUAL(key.character(), 0);
        REQUIRE(!key.valid());
    }

    void testParameterizedConstructorAndEquality() {
        term::Key key{term::Key::Character, 'a'};
        REQUIRE_EQUAL(key.type(), term::Key::Character);
        REQUIRE_EQUAL(key.character(), 'a');
        REQUIRE(key.valid());
        REQUIRE(key == term::Key{term::Key::Character, 'a'});
        REQUIRE(key != term::Key{term::Key::Character, 'b'});
    }

    void testFromConsoleInputSpecialKeys() {
        struct Entry {
            const char *input;
            term::Key expected;
        };
        const std::array<Entry, 31> entries{{
            {"\n", term::Key{term::Key::Enter}},
            {"\r", term::Key{term::Key::Enter}},
            {"\t", term::Key{term::Key::Tab}},
            {" ", term::Key{term::Key::Space}},
            {"\x1b", term::Key{term::Key::Escape}},
            {"\x08", term::Key{term::Key::Backspace}},
            {"\x7f", term::Key{term::Key::Backspace}},
            {"\x1b[A", term::Key{term::Key::Up}},
            {"\x1b[B", term::Key{term::Key::Down}},
            {"\x1b[C", term::Key{term::Key::Right}},
            {"\x1b[D", term::Key{term::Key::Left}},
            {"\x1b[H", term::Key{term::Key::Home}},
            {"\x1b[F", term::Key{term::Key::End}},
            {"\x1bOH", term::Key{term::Key::Home}},
            {"\x1bOF", term::Key{term::Key::End}},
            {"\x1b[2~", term::Key{term::Key::Insert}},
            {"\x1b[3~", term::Key{term::Key::Delete}},
            {"\x1b[5~", term::Key{term::Key::PageUp}},
            {"\x1b[6~", term::Key{term::Key::PageDown}},
            {"\x1bOP", term::Key{term::Key::F1}},
            {"\x1bOQ", term::Key{term::Key::F2}},
            {"\x1bOR", term::Key{term::Key::F3}},
            {"\x1bOS", term::Key{term::Key::F4}},
            {"\x1b[15~", term::Key{term::Key::F5}},
            {"\x1b[17~", term::Key{term::Key::F6}},
            {"\x1b[18~", term::Key{term::Key::F7}},
            {"\x1b[19~", term::Key{term::Key::F8}},
            {"\x1b[20~", term::Key{term::Key::F9}},
            {"\x1b[21~", term::Key{term::Key::F10}},
            {"\x1b[23~", term::Key{term::Key::F11}},
            {"\x1b[24~", term::Key{term::Key::F12}},
        }};
        for (const auto &entry : entries) {
            auto key = term::Key::fromConsoleInput(entry.input);
            REQUIRE_EQUAL(key, entry.expected);
            REQUIRE(key.valid());
        }
    }

    void testFromConsoleInputCharacter() {
        auto key = term::Key::fromConsoleInput("x");
        REQUIRE_EQUAL(key, term::Key(term::Key::Character, 'x'));
        REQUIRE(key.valid());
    }

    void testFromConsoleInputInvalid() {
        auto key = term::Key::fromConsoleInput("?");
        REQUIRE_EQUAL(key, term::Key{term::Key::None});
        REQUIRE(!key.valid());

        key = term::Key::fromConsoleInput("ab");
        REQUIRE_EQUAL(key, term::Key{term::Key::None});
        REQUIRE(!key.valid());
    }

    void testFromStringAndToString() {
        struct Entry {
            const char *input;
            term::Key expected;
            const char *text;
        };
        const std::array<Entry, 9> entries{{
            {"Return", term::Key{term::Key::Enter}, "enter"},
            {"TAB", term::Key{term::Key::Tab}, "tab"},
            {"Esc", term::Key{term::Key::Escape}, "escape"},
            {"del", term::Key{term::Key::Delete}, "delete"},
            {"page_up", term::Key{term::Key::PageUp}, "pageup"},
            {"PgDn", term::Key{term::Key::PageDown}, "pagedown"},
            {"Left", term::Key{term::Key::Left}, "left"},
            {"F12", term::Key{term::Key::F12}, "f12"},
            {"X", term::Key{term::Key::Character, 'x'}, "x"},
        }};
        for (const auto &entry : entries) {
            auto key = term::Key::fromString(entry.input);
            REQUIRE_EQUAL(key, entry.expected);
            REQUIRE_EQUAL(key.toString(), entry.text);
        }

        REQUIRE_EQUAL(term::Key::fromString("?"), term::Key{term::Key::None});
        REQUIRE_EQUAL(term::Key{}.toString(), "");
    }

    void testToDisplayText() {
        struct Entry {
            term::Key key;
            const char *withBrackets;
            const char *withoutBrackets;
        };
        const std::array<Entry, 8> entries{{
            {term::Key{term::Key::Enter}, "[↵]", "↵"},
            {term::Key{term::Key::Tab}, "[tab]", "tab"},
            {term::Key{term::Key::Escape}, "[esc]", "esc"},
            {term::Key{term::Key::Backspace}, "[⌫]", "⌫"},
            {term::Key{term::Key::Left}, "[←]", "←"},
            {term::Key{term::Key::PageDown}, "[pgdn]", "pgdn"},
            {term::Key{term::Key::F5}, "[F5]", "F5"},
            {term::Key{term::Key::Character, 'x'}, "[x]", "x"},
        }};
        for (const auto &entry : entries) {
            REQUIRE_EQUAL(entry.key.toDisplayText(), entry.withBrackets);
            REQUIRE_EQUAL(entry.key.toDisplayText(false), entry.withoutBrackets);
        }

        REQUIRE_EQUAL(term::Key{}.toDisplayText(), "");
        REQUIRE_EQUAL(term::Key{}.toDisplayText(false), "");
    }
};
