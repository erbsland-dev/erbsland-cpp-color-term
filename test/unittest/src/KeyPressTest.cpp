// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <array>

TESTED_TARGETS(Key)
class KeyPressTest final : public el::UnitTest {
public:
    void testDefaultConstructor() {
        Key key;
        REQUIRE_EQUAL(key.type(), Key::None);
        REQUIRE_EQUAL(key.character(), 0);
        REQUIRE(!key.valid());
    }

    void testParameterizedConstructorAndEquality() {
        Key key{Key::Character, 'a'};
        REQUIRE_EQUAL(key.type(), Key::Character);
        REQUIRE_EQUAL(key.character(), 'a');
        REQUIRE(key.valid());
        REQUIRE(key == Key{Key::Character, 'a'});
        REQUIRE(key != Key{Key::Character, 'b'});
    }

    void testFromConsoleInputSpecialKeys() {
        struct Entry {
            const char *input;
            Key expected;
        };
        const std::array<Entry, 31> entries{{
            {"\n", Key{Key::Enter}},         {"\r", Key{Key::Enter}},       {"\t", Key{Key::Tab}},
            {" ", Key{Key::Space}},          {"\x1b", Key{Key::Escape}},    {"\x08", Key{Key::Backspace}},
            {"\x7f", Key{Key::Backspace}},   {"\x1b[A", Key{Key::Up}},      {"\x1b[B", Key{Key::Down}},
            {"\x1b[C", Key{Key::Right}},     {"\x1b[D", Key{Key::Left}},    {"\x1b[H", Key{Key::Home}},
            {"\x1b[F", Key{Key::End}},       {"\x1bOH", Key{Key::Home}},    {"\x1bOF", Key{Key::End}},
            {"\x1b[2~", Key{Key::Insert}},   {"\x1b[3~", Key{Key::Delete}}, {"\x1b[5~", Key{Key::PageUp}},
            {"\x1b[6~", Key{Key::PageDown}}, {"\x1bOP", Key{Key::F1}},      {"\x1bOQ", Key{Key::F2}},
            {"\x1bOR", Key{Key::F3}},        {"\x1bOS", Key{Key::F4}},      {"\x1b[15~", Key{Key::F5}},
            {"\x1b[17~", Key{Key::F6}},      {"\x1b[18~", Key{Key::F7}},    {"\x1b[19~", Key{Key::F8}},
            {"\x1b[20~", Key{Key::F9}},      {"\x1b[21~", Key{Key::F10}},   {"\x1b[23~", Key{Key::F11}},
            {"\x1b[24~", Key{Key::F12}},
        }};
        for (const auto &entry : entries) {
            auto key = Key::fromConsoleInput(entry.input);
            REQUIRE_EQUAL(key, entry.expected);
            REQUIRE(key.valid());
        }
    }

    void testFromConsoleInputCharacter() {
        auto key = Key::fromConsoleInput("x");
        REQUIRE_EQUAL(key, Key(Key::Character, 'x'));
        REQUIRE(key.valid());
    }

    void testFromConsoleInputInvalid() {
        auto key = Key::fromConsoleInput("?");
        REQUIRE_EQUAL(key, Key{Key::None});
        REQUIRE(!key.valid());

        key = Key::fromConsoleInput("ab");
        REQUIRE_EQUAL(key, Key{Key::None});
        REQUIRE(!key.valid());
    }

    void testFromStringAndToString() {
        struct Entry {
            const char *input;
            Key expected;
            const char *text;
        };
        const std::array<Entry, 9> entries{{
            {"Return", Key{Key::Enter}, "enter"},
            {"TAB", Key{Key::Tab}, "tab"},
            {"Esc", Key{Key::Escape}, "escape"},
            {"del", Key{Key::Delete}, "delete"},
            {"page_up", Key{Key::PageUp}, "pageup"},
            {"PgDn", Key{Key::PageDown}, "pagedown"},
            {"Left", Key{Key::Left}, "left"},
            {"F12", Key{Key::F12}, "f12"},
            {"X", Key{Key::Character, 'x'}, "x"},
        }};
        for (const auto &entry : entries) {
            auto key = Key::fromString(entry.input);
            REQUIRE_EQUAL(key, entry.expected);
            REQUIRE_EQUAL(key.toString(), entry.text);
        }

        REQUIRE_EQUAL(Key::fromString("?"), Key{Key::None});
        REQUIRE_EQUAL(Key{}.toString(), "");
    }

    void testToDisplayText() {
        struct Entry {
            Key key;
            const char *withBrackets;
            const char *withoutBrackets;
        };
        const std::array<Entry, 8> entries{{
            {Key{Key::Enter}, "[↵]", "↵"},
            {Key{Key::Tab}, "[tab]", "tab"},
            {Key{Key::Escape}, "[esc]", "esc"},
            {Key{Key::Backspace}, "[⌫]", "⌫"},
            {Key{Key::Left}, "[←]", "←"},
            {Key{Key::PageDown}, "[pgdn]", "pgdn"},
            {Key{Key::F5}, "[F5]", "F5"},
            {Key{Key::Character, 'x'}, "[x]", "x"},
        }};
        for (const auto &entry : entries) {
            REQUIRE_EQUAL(entry.key.toDisplayText(), entry.withBrackets);
            REQUIRE_EQUAL(entry.key.toDisplayText(false), entry.withoutBrackets);
        }

        REQUIRE_EQUAL(Key{}.toDisplayText(), "");
        REQUIRE_EQUAL(Key{}.toDisplayText(false), "");
    }
};
