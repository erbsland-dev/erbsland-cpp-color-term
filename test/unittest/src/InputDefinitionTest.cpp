// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(InputDefinition)
class InputDefinitionTest final : public el::UnitTest {
public:
    void testDefaultConstructor() {
        term::InputDefinition definition;
        REQUIRE_EQUAL(definition.forMode(), term::InputDefinition::ForMode::Both);
        REQUIRE(!definition.valid());
    }

    void testParameterizedConstructor() {
        term::InputDefinition definition{term::Key{term::Key::Enter}, term::InputDefinition::ForMode::Key};
        REQUIRE_EQUAL(definition.keyPress(), term::Key{term::Key::Enter});
        REQUIRE_EQUAL(definition.forMode(), term::InputDefinition::ForMode::Key);
        REQUIRE(definition.valid());
    }

    void testFromStringBasic() {
        auto definition = term::InputDefinition::fromString("enter");
        REQUIRE_EQUAL(definition.keyPress(), term::Key{term::Key::Enter});
        REQUIRE_EQUAL(definition.forMode(), term::InputDefinition::ForMode::Both);
    }

    void testFromStringModes() {
        auto definition = term::InputDefinition::fromString(">up");
        REQUIRE_EQUAL(definition.keyPress(), term::Key{term::Key::Up});
        REQUIRE_EQUAL(definition.forMode(), term::InputDefinition::ForMode::ReadLine);

        definition = term::InputDefinition::fromString("+page_up");
        REQUIRE_EQUAL(definition.keyPress(), term::Key{term::Key::PageUp});
        REQUIRE_EQUAL(definition.forMode(), term::InputDefinition::ForMode::Key);
    }

    void testFromStringCharacter() {
        auto definition = term::InputDefinition::fromString("a");
        REQUIRE_EQUAL(definition.keyPress(), term::Key(term::Key::Character, 'a'));
        REQUIRE(definition.valid());
    }

    void testFromStringCaseInsensitive() {
        auto definition = term::InputDefinition::fromString("EnTeR");
        REQUIRE_EQUAL(definition.keyPress(), term::Key{term::Key::Enter});

        definition = term::InputDefinition::fromString(">Esc");
        REQUIRE_EQUAL(definition.keyPress(), term::Key{term::Key::Escape});
        REQUIRE_EQUAL(definition.forMode(), term::InputDefinition::ForMode::ReadLine);
    }

    void testFromStringInvalid() {
        auto definition = term::InputDefinition::fromString("?");
        REQUIRE_EQUAL(definition.keyPress(), term::Key{term::Key::None});
        REQUIRE(!definition.valid());
    }

    void testToString() {
        term::InputDefinition definition{term::Key{term::Key::Enter}, term::InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toString(), "enter");

        definition = term::InputDefinition{term::Key{term::Key::Left}, term::InputDefinition::ForMode::ReadLine};
        REQUIRE_EQUAL(definition.toString(), ">left");

        definition = term::InputDefinition{term::Key{term::Key::F5}, term::InputDefinition::ForMode::Key};
        REQUIRE_EQUAL(definition.toString(), "+f5");

        definition = term::InputDefinition{term::Key{term::Key::Character, 'x'}, term::InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toString(), "x");

        definition = term::InputDefinition{term::Key{term::Key::None}, term::InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toString(), "");
    }

    void testToDisplayText() {
        term::InputDefinition definition{term::Key{term::Key::Enter}, term::InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toDisplayText(), "[↵]");
        REQUIRE_EQUAL(definition.toDisplayText(false), "↵");

        definition = term::InputDefinition{term::Key{term::Key::Escape}, term::InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toDisplayText(), "[esc]");
        REQUIRE_EQUAL(definition.toDisplayText(false), "esc");

        definition = term::InputDefinition{term::Key{term::Key::PageDown}, term::InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toDisplayText(), "[pgdn]");
        REQUIRE_EQUAL(definition.toDisplayText(false), "pgdn");

        definition = term::InputDefinition{term::Key{term::Key::F12}, term::InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toDisplayText(), "[F12]");
        REQUIRE_EQUAL(definition.toDisplayText(false), "F12");

        definition = term::InputDefinition{term::Key{term::Key::Character, 'x'}, term::InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toDisplayText(), "[x]");
        REQUIRE_EQUAL(definition.toDisplayText(false), "x");

        definition = term::InputDefinition{term::Key{term::Key::None}, term::InputDefinition::ForMode::Both};
        REQUIRE_EQUAL(definition.toDisplayText(), "");
        REQUIRE_EQUAL(definition.toDisplayText(false), "");
    }
};
