// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(InputDefinition)
class InputDefinitionTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testDefaultConstructor() {
        InputDefinition definition;
        REQUIRE_EQUAL(definition.forMode(), InputDefinition::ForMode::Both);
        REQUIRE(!definition.valid());
    }

    void testParameterizedConstructor() {
        InputDefinition definition{Key{Key::Enter}, InputDefinition::ForMode::Key};
        REQUIRE_EQUAL(definition.keyPress(), Key{Key::Enter});
        REQUIRE_EQUAL(definition.forMode(), InputDefinition::ForMode::Key);
        REQUIRE(definition.valid());
    }

    void testFromStringBasic() {
        auto definition = InputDefinition::fromString("enter");
        REQUIRE_EQUAL(definition.keyPress(), Key{Key::Enter});
        REQUIRE_EQUAL(definition.forMode(), InputDefinition::ForMode::Both);
    }

    void testFromStringModes() {
        auto definition = InputDefinition::fromString(">up");
        REQUIRE_EQUAL(definition.keyPress(), Key{Key::Up});
        REQUIRE_EQUAL(definition.forMode(), InputDefinition::ForMode::ReadLine);

        definition = InputDefinition::fromString("+page_up");
        REQUIRE_EQUAL(definition.keyPress(), Key{Key::PageUp});
        REQUIRE_EQUAL(definition.forMode(), InputDefinition::ForMode::Key);
    }

    void testFromStringCharacter() {
        auto definition = InputDefinition::fromString("a");
        REQUIRE_EQUAL(definition.keyPress(), Key(Key::Character, U'a'));
        REQUIRE(definition.valid());
    }

    void testFromStringCaseInsensitive() {
        auto definition = InputDefinition::fromString("EnTeR");
        REQUIRE_EQUAL(definition.keyPress(), Key{Key::Enter});

        definition = InputDefinition::fromString(">Esc");
        REQUIRE_EQUAL(definition.keyPress(), Key{Key::Escape});
        REQUIRE_EQUAL(definition.forMode(), InputDefinition::ForMode::ReadLine);
    }

    void testFromStringInvalid() {
        auto definition = InputDefinition::fromString("");
        REQUIRE_EQUAL(definition.keyPress(), Key{Key::None});
        REQUIRE(!definition.valid());
    }

    void testFromStringReplacesInvalidUtf8() {
        const auto definition = InputDefinition::fromString(bytes({0xC3}));

        REQUIRE_EQUAL(definition.keyPress(), (Key{Key::Character, U'\uFFFD'}));
        REQUIRE(definition.valid());
    }

    void testFromStringReplacesStructurallyUnsupportedText() {
        const auto definition = InputDefinition::fromString("ab");

        REQUIRE_EQUAL(definition.keyPress(), (Key{Key::Character, U'\uFFFD'}));
        REQUIRE(definition.valid());
    }

    void testFromStringEscapedPrefixCharacters() {
        auto definition = InputDefinition::fromString("\\+");
        REQUIRE_EQUAL(definition.keyPress(), (Key{Key::Character, U'+'}));
        REQUIRE_EQUAL(definition.forMode(), InputDefinition::ForMode::Both);

        definition = InputDefinition::fromString("\\>");
        REQUIRE_EQUAL(definition.keyPress(), (Key{Key::Character, U'>'}));
        REQUIRE_EQUAL(definition.forMode(), InputDefinition::ForMode::Both);

        definition = InputDefinition::fromString("+\\+");
        REQUIRE_EQUAL(definition.keyPress(), (Key{Key::Character, U'+'}));
        REQUIRE_EQUAL(definition.forMode(), InputDefinition::ForMode::Key);
    }

    void testToString() {
        InputDefinition definition{Key{Key::Enter}, InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toString(), "enter");

        definition = InputDefinition{Key{Key::Left}, InputDefinition::ForMode::ReadLine};
        REQUIRE_EQUAL(definition.toString(), ">left");

        definition = InputDefinition{Key{Key::F5}, InputDefinition::ForMode::Key};
        REQUIRE_EQUAL(definition.toString(), "+f5");

        definition = InputDefinition{Key{Key::Character, U'x'}, InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toString(), "x");

        definition = InputDefinition{Key{Key::Character, U'+'}, InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toString(), "\\+");

        definition = InputDefinition{Key{Key::Character, U'>'}, InputDefinition::ForMode::Key};
        REQUIRE_EQUAL(definition.toString(), "+\\>");

        definition = InputDefinition{Key{Key::None}, InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toString(), "");
    }

    void testToDisplayText() {
        InputDefinition definition{Key{Key::Enter}, InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toDisplayText(), "[↵]");
        REQUIRE_EQUAL(definition.toDisplayText(false), "↵");

        definition = InputDefinition{Key{Key::Escape}, InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toDisplayText(), "[esc]");
        REQUIRE_EQUAL(definition.toDisplayText(false), "esc");

        definition = InputDefinition{Key{Key::PageDown}, InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toDisplayText(), "[pgdn]");
        REQUIRE_EQUAL(definition.toDisplayText(false), "pgdn");

        definition = InputDefinition{Key{Key::F12}, InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toDisplayText(), "[F12]");
        REQUIRE_EQUAL(definition.toDisplayText(false), "F12");

        definition = InputDefinition{Key{Key::Character, U'x'}, InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toDisplayText(), "[x]");
        REQUIRE_EQUAL(definition.toDisplayText(false), "x");

        definition = InputDefinition{Key{Key::None}, InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toDisplayText(), "");
        REQUIRE_EQUAL(definition.toDisplayText(false), "");
    }
};
