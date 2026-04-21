// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(FastCharSet)
class FastCharSetTest final : public el::UnitTest {
public:
    void testCreateCanonicalizesCharactersAndBuildsCustomMode() {
        const auto characterSet = FastCharSet::create(U"bcaab");

        REQUIRE(characterSet != nullptr);
        REQUIRE_EQUAL(characterSet->characters(), std::u32string{U"abc"});
        REQUIRE(characterSet != FastCharSet::onlySpace());
        REQUIRE(characterSet != FastCharSet::spaceAndTab());
    }

    void testCreateUsesSharedDefaultsForCommonPatterns() {
        const auto onlySpace = FastCharSet::create(U"  ");
        const auto onlySpaceDefault = FastCharSet::onlySpace();
        const auto spaceAndTab = FastCharSet::create(U"\t \t");
        const auto spaceAndTabDefault = FastCharSet::spaceAndTab();

        REQUIRE(onlySpace == onlySpaceDefault);
        REQUIRE(spaceAndTab == spaceAndTabDefault);
        REQUIRE_EQUAL(spaceAndTab->characters(), std::u32string{U"\t "});
    }

    void testContainsChecksAsciiAndNonAsciiCharacters() {
        const auto characterSet = FastCharSet::create(U".\u00a0\u2026");

        REQUIRE(characterSet->contains(U'.'));
        REQUIRE(characterSet->contains(U'\u00a0'));
        REQUIRE(characterSet->contains(U'\u2026'));
        REQUIRE(!characterSet->contains(U','));
    }

    void testContainsCharOnlyMatchesSingleCodePointCharacters() {
        const auto characterSet = FastCharSet::create(U".");

        REQUIRE(characterSet->contains(Char{U'.'}));
        REQUIRE(!characterSet->contains(Char{U".."}));
    }

    void testSharedDefaultsContainTheExpectedCharacters() {
        const auto onlySpace = FastCharSet::onlySpace();
        const auto spaceAndTab = FastCharSet::spaceAndTab();

        REQUIRE_EQUAL(onlySpace->characters(), std::u32string{U" "});
        REQUIRE_EQUAL(spaceAndTab->characters(), std::u32string{U"\t "});
        REQUIRE(onlySpace->contains(U' '));
        REQUIRE(!onlySpace->contains(U'\t'));
        REQUIRE(spaceAndTab->contains(U' '));
        REQUIRE(spaceAndTab->contains(U'\t'));
    }
};
