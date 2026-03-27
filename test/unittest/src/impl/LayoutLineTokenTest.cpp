// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/impl/paragraph/LayoutLineToken.hpp>


TESTED_TARGETS(LayoutLineToken)
class LayoutLineTokenTest final : public el::UnitTest {
public:
    void testRemainingWidthUsesTheCachedFullWidthForOffsetZero() {
        const auto text = String{U"ABCD"};
        const auto word =
            term::impl::paragraph::LayoutLineToken{term::impl::paragraph::LayoutLineToken::Type::Word, 1, 3, 3};

        REQUIRE_EQUAL(word.remainingWidth(text, 0), 3);
        REQUIRE_EQUAL(word.remainingWidth(text, 1), 2);
    }

    void testSplitAccountsForTheTrailingMarkerOnlyWhenTheWordContinues() {
        const auto text = String{U"ABCD"};
        const auto word =
            term::impl::paragraph::LayoutLineToken{term::impl::paragraph::LayoutLineToken::Type::Word, 0, 4, 4};

        const auto splitWithMarker = word.split(text, 0, 3, 1);
        REQUIRE(splitWithMarker.has_value());
        REQUIRE_EQUAL(splitWithMarker->sourceCharacterCount, std::size_t{2});
        REQUIRE_EQUAL(splitWithMarker->sourceWidth, 2);
        REQUIRE_EQUAL(splitWithMarker->width, 3);

        const auto finalSplit = word.split(text, 2, 3, 1);
        REQUIRE(finalSplit.has_value());
        REQUIRE_EQUAL(finalSplit->sourceCharacterCount, std::size_t{2});
        REQUIRE_EQUAL(finalSplit->sourceWidth, 2);
        REQUIRE_EQUAL(finalSplit->width, 2);
    }
};
