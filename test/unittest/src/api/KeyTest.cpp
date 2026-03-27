// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <format>
#include <functional>

TESTED_TARGETS(Key)
class KeyTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testDefaultConstructor() {
        Key key;
        REQUIRE_EQUAL(key.type(), Key::None);
        REQUIRE_EQUAL(key.character(), 0);
        REQUIRE_EQUAL(key.unicode(), 0);
        REQUIRE_EQUAL(key.combined(), std::u32string{});
        REQUIRE(!key.valid());
    }

    void testParameterizedConstructorAndEquality() {
        Key key{Key::Character, U'a'};
        REQUIRE_EQUAL(key.type(), Key::Character);
        REQUIRE_EQUAL(key.character(), 'a');
        REQUIRE_EQUAL(key.unicode(), U'a');
        REQUIRE_EQUAL(key.combined(), std::u32string{U"a"});
        REQUIRE(key.valid());
        REQUIRE(key == Key{Key::Character, U'a'});
        REQUIRE(key != Key{Key::Character, U'b'});

        const auto combined = Key{Key::Combined, U"e\u0301"};
        REQUIRE_EQUAL(combined.type(), Key::Combined);
        REQUIRE_EQUAL(combined.character(), 0);
        REQUIRE_EQUAL(combined.unicode(), 0);
        REQUIRE_EQUAL(combined.combined(), std::u32string{U"e\u0301"});
    }

    void testFromConsoleInputSpecialKeys() {
        struct Entry {
            const char *input;
            Key expected;
        };
        const std::array<Entry, 32> entries{{
            {"\n", Key{Key::Enter}},       {"\r", Key{Key::Enter}},         {"\t", Key{Key::Tab}},
            {"\x1b[Z", Key{Key::BackTab}}, {" ", Key{Key::Space}},          {"\x1b", Key{Key::Escape}},
            {"\x08", Key{Key::Backspace}}, {"\x7f", Key{Key::Backspace}},   {"\x1b[A", Key{Key::Up}},
            {"\x1b[B", Key{Key::Down}},    {"\x1b[C", Key{Key::Right}},     {"\x1b[D", Key{Key::Left}},
            {"\x1b[H", Key{Key::Home}},    {"\x1b[F", Key{Key::End}},       {"\x1bOH", Key{Key::Home}},
            {"\x1bOF", Key{Key::End}},     {"\x1b[2~", Key{Key::Insert}},   {"\x1b[3~", Key{Key::Delete}},
            {"\x1b[5~", Key{Key::PageUp}}, {"\x1b[6~", Key{Key::PageDown}}, {"\x1bOP", Key{Key::F1}},
            {"\x1bOQ", Key{Key::F2}},      {"\x1bOR", Key{Key::F3}},        {"\x1bOS", Key{Key::F4}},
            {"\x1b[15~", Key{Key::F5}},    {"\x1b[17~", Key{Key::F6}},      {"\x1b[18~", Key{Key::F7}},
            {"\x1b[19~", Key{Key::F8}},    {"\x1b[20~", Key{Key::F9}},      {"\x1b[21~", Key{Key::F10}},
            {"\x1b[23~", Key{Key::F11}},   {"\x1b[24~", Key{Key::F12}},
        }};
        for (std::size_t index = 0; index < entries.size(); ++index) {
            const auto &entry = entries[index];
            runWithContext(
                SOURCE_LOCATION(),
                [&]() {
                    const auto key = Key::fromConsoleInput(entry.input);
                    REQUIRE_EQUAL(key, entry.expected);
                    REQUIRE(key.valid());
                },
                [&]() -> std::string {
                    return std::format(
                        "index = {} / inputSize = {} / expectedKey = {}",
                        index,
                        std::string_view{entry.input}.size(),
                        entry.expected.toString());
                });
        }
    }

    void testFromConsoleInputPrintableCharacters() {
        struct Entry {
            std::string input;
            Key expected;
        };
        const std::array<Entry, 7> entries{{
            {"x", Key{Key::Character, U'x'}},
            {"X", Key{Key::Character, U'X'}},
            {"#", Key{Key::Character, U'#'}},
            {"+", Key{Key::Character, U'+'}},
            {"?", Key{Key::Character, U'?'}},
            {bytes({0xC3, 0xA4}), Key{Key::Character, U'\u00e4'}},
            {bytes({0x65, 0xCC, 0x81}), Key{Key::Combined, U"e\u0301"}},
        }};
        for (std::size_t index = 0; index < entries.size(); ++index) {
            const auto &entry = entries[index];
            runWithContext(
                SOURCE_LOCATION(),
                [&]() {
                    const auto key = Key::fromConsoleInput(entry.input);
                    REQUIRE_EQUAL(key, entry.expected);
                    REQUIRE(key.valid());
                },
                [&]() -> std::string { return std::format("index = {} / input = \"{}\"", index, entry.input); });
        }
    }

    void testFromConsoleInputInvalid() {
        auto key = Key::fromConsoleInput("");
        REQUIRE_EQUAL(key, Key{Key::None});
        REQUIRE(!key.valid());

        key = Key::fromConsoleInput("ab");
        REQUIRE_EQUAL(key, Key{Key::None});
        REQUIRE(!key.valid());

        key = Key::fromConsoleInput("\x01");
        REQUIRE_EQUAL(key, Key{Key::None});
        REQUIRE(!key.valid());
    }

    void testFromStringAndToString() {
        struct Entry {
            std::string input;
            Key expected;
            std::string text;
        };
        const std::array<Entry, 13> entries{{
            {"Return", Key{Key::Enter}, "enter"},
            {"TAB", Key{Key::Tab}, "tab"},
            {"shift_tab", Key{Key::BackTab}, "backtab"},
            {"Esc", Key{Key::Escape}, "escape"},
            {"del", Key{Key::Delete}, "delete"},
            {"page_up", Key{Key::PageUp}, "pageup"},
            {"PgDn", Key{Key::PageDown}, "pagedown"},
            {"Left", Key{Key::Left}, "left"},
            {"F12", Key{Key::F12}, "f12"},
            {"X", Key{Key::Character, U'X'}, "X"},
            {"?", Key{Key::Character, U'?'}, "?"},
            {bytes({0xC3, 0xA4}), Key{Key::Character, U'\u00e4'}, bytes({0xC3, 0xA4})},
            {bytes({0x65, 0xCC, 0x81}), Key{Key::Combined, U"e\u0301"}, bytes({0x65, 0xCC, 0x81})},
        }};
        for (std::size_t index = 0; index < entries.size(); ++index) {
            const auto &entry = entries[index];
            runWithContext(
                SOURCE_LOCATION(),
                [&]() {
                    const auto key = Key::fromString(entry.input);
                    REQUIRE_EQUAL(key, entry.expected);
                    REQUIRE_EQUAL(key.toString(), entry.text);
                },
                [&]() -> std::string { return std::format("index = {} / input = \"{}\"", index, entry.input); });
        }

        REQUIRE_EQUAL(Key{}.toString(), "");
    }

    void testLegacyAsciiAndUnicodeAccessors() {
        const auto ascii = Key{Key::Character, U'A'};
        const auto unicode = Key{Key::Character, U'\u00e4'};
        const auto combined = Key{Key::Combined, U"e\u0301"};

        REQUIRE_EQUAL(ascii.character(), 'A');
        REQUIRE_EQUAL(ascii.unicode(), U'A');
        REQUIRE_EQUAL(ascii.combined(), std::u32string{U"A"});

        REQUIRE_EQUAL(unicode.character(), 0);
        REQUIRE_EQUAL(unicode.unicode(), U'\u00e4');
        REQUIRE_EQUAL(unicode.combined(), std::u32string{U"\u00e4"});

        REQUIRE_EQUAL(combined.character(), 0);
        REQUIRE_EQUAL(combined.unicode(), 0);
        REQUIRE_EQUAL(combined.combined(), std::u32string{U"e\u0301"});
    }

    void testToDisplayText() {
        struct Entry {
            Key key;
            const char *withBrackets;
            const char *withoutBrackets;
        };
        const std::array<Entry, 10> entries{{
            {Key{Key::Enter}, "[↵]", "↵"},
            {Key{Key::Tab}, "[tab]", "tab"},
            {Key{Key::BackTab}, "[⇤]", "⇤"},
            {Key{Key::Escape}, "[esc]", "esc"},
            {Key{Key::Backspace}, "[⌫]", "⌫"},
            {Key{Key::Left}, "[←]", "←"},
            {Key{Key::PageDown}, "[pgdn]", "pgdn"},
            {Key{Key::F5}, "[F5]", "F5"},
            {Key{Key::Character, U'x'}, "[x]", "x"},
            {Key{Key::Combined, U"e\u0301"}, "[é]", "é"},
        }};
        for (std::size_t index = 0; index < entries.size(); ++index) {
            const auto &entry = entries[index];
            runWithContext(
                SOURCE_LOCATION(),
                [&]() {
                    REQUIRE_EQUAL(entry.key.toDisplayText(), entry.withBrackets);
                    REQUIRE_EQUAL(entry.key.toDisplayText(false), entry.withoutBrackets);
                },
                [&]() -> std::string {
                    return std::format(
                        "index = {} / expectedWithBrackets = \"{}\" / expectedWithoutBrackets = \"{}\"",
                        index,
                        entry.withBrackets,
                        entry.withoutBrackets);
                });
        }

        REQUIRE_EQUAL(Key{}.toDisplayText(), "");
        REQUIRE_EQUAL(Key{}.toDisplayText(false), "");
    }

    void testFromStringReplacesInvalidUtf8() {
        const auto key = Key::fromString(bytes({0xC3}));

        REQUIRE_EQUAL(key, (Key{Key::Character, U'\uFFFD'}));
        REQUIRE(key.valid());
    }

    void testFromStringReplacesStructurallyUnsupportedText() {
        const auto multiple = Key::fromString("ab");
        const auto control = Key::fromString("\x01");

        REQUIRE_EQUAL(multiple, (Key{Key::Character, U'\uFFFD'}));
        REQUIRE(multiple.valid());
        REQUIRE_EQUAL(control, (Key{Key::Character, U'\uFFFD'}));
        REQUIRE(control.valid());
    }

    void testHashMatchesStdHashAndIncludesCharacterPayload() {
        const auto characterA = Key{Key::Character, U'a'};
        const auto characterB = Key{Key::Character, U'b'};
        const auto combined = Key{Key::Combined, U"e\u0301"};
        const auto special = Key{Key::Enter};

        REQUIRE_EQUAL(characterA.hash(), std::hash<Key>{}(characterA));
        REQUIRE_EQUAL(special.hash(), std::hash<Key>{}(special));
        REQUIRE_EQUAL(combined.hash(), std::hash<Key>{}(combined));
        REQUIRE_NOT_EQUAL(characterA.hash(), characterB.hash());
        REQUIRE_NOT_EQUAL(characterA.hash(), special.hash());
        REQUIRE_NOT_EQUAL(combined.hash(), characterA.hash());
    }
};
