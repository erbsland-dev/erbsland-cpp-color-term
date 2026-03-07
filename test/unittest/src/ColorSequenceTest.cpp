// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/unittest/UnitTest.hpp>

#include "TestHelper.hpp"


class ColorSequenceTest : public el::UnitTest {
public:
    void testDefaultSequenceIsEmpty() {
        const auto sequence = term::ColorSequence{};
        REQUIRE(sequence.empty());
        REQUIRE_EQUAL(sequence.entryCount(), 0U);
        REQUIRE_EQUAL(sequence.sequenceLength(), 0U);
        REQUIRE_EQUAL(sequence.color(0U), term::Color{});
        REQUIRE_EQUAL(sequence.colorNormalized(0.5), term::Color{});
    }

    void testAddAndCyclicLookup() {
        auto sequence = term::ColorSequence{};
        sequence.add(term::Color{term::fg::Black, term::bg::Black}, 20);
        sequence.add(term::Color{term::fg::Blue, term::bg::Black});
        sequence.add(term::Color{term::fg::White, term::bg::Black});
        sequence.add(term::Color{term::fg::Blue, term::bg::Black});

        REQUIRE_FALSE(sequence.empty());
        REQUIRE_EQUAL(sequence.entryCount(), 4U);
        REQUIRE_EQUAL(sequence.sequenceLength(), 23U);
        REQUIRE_EQUAL(sequence.color(0), term::Color(term::fg::Black, term::bg::Black));
        REQUIRE_EQUAL(sequence.color(19), term::Color(term::fg::Black, term::bg::Black));
        REQUIRE_EQUAL(sequence.color(20), term::Color(term::fg::Blue, term::bg::Black));
        REQUIRE_EQUAL(sequence.color(21), term::Color(term::fg::White, term::bg::Black));
        REQUIRE_EQUAL(sequence.color(22), term::Color(term::fg::Blue, term::bg::Black));
        REQUIRE_EQUAL(sequence.color(23), term::Color(term::fg::Black, term::bg::Black));
    }

    void testConstructionFromCodeInitializers() {
        const auto colorListSequence = term::ColorSequence{
            term::Color{term::fg::Red, term::bg::Black},
            term::Color{term::fg::Green, term::bg::Black},
            term::Color{term::fg::Blue, term::bg::Black},
        };
        REQUIRE_EQUAL(colorListSequence.sequenceLength(), 3U);
        REQUIRE_EQUAL(colorListSequence.color(0), term::Color(term::fg::Red, term::bg::Black));
        REQUIRE_EQUAL(colorListSequence.color(1), term::Color(term::fg::Green, term::bg::Black));
        REQUIRE_EQUAL(colorListSequence.color(2), term::Color(term::fg::Blue, term::bg::Black));

        const auto countedSequence = term::ColorSequence{
            {term::Color{term::fg::BrightWhite, term::bg::Black}, 2},
            {term::Color{term::fg::BrightYellow, term::bg::Black}, 4},
            {term::Color{term::fg::Yellow, term::bg::Black}, 3},
        };
        REQUIRE_EQUAL(countedSequence.sequenceLength(), 9U);
        REQUIRE_EQUAL(countedSequence.color(0), term::Color(term::fg::BrightWhite, term::bg::Black));
        REQUIRE_EQUAL(countedSequence.color(1), term::Color(term::fg::BrightWhite, term::bg::Black));
        REQUIRE_EQUAL(countedSequence.color(2), term::Color(term::fg::BrightYellow, term::bg::Black));
        REQUIRE_EQUAL(countedSequence.color(5), term::Color(term::fg::BrightYellow, term::bg::Black));
        REQUIRE_EQUAL(countedSequence.color(6), term::Color(term::fg::Yellow, term::bg::Black));
        REQUIRE_EQUAL(countedSequence.color(8), term::Color(term::fg::Yellow, term::bg::Black));
    }

    void testNormalizedLookupWithClamping() {
        const auto sequence = term::ColorSequence{
            {term::Color{term::fg::Black, term::bg::Black}, 2},
            {term::Color{term::fg::Blue, term::bg::Black}, 1},
            {term::Color{term::fg::White, term::bg::Black}, 1},
        };

        REQUIRE_EQUAL(sequence.colorNormalized(-1.0), term::Color(term::fg::Black, term::bg::Black));
        REQUIRE_EQUAL(sequence.colorNormalized(0.0), term::Color(term::fg::Black, term::bg::Black));
        REQUIRE_EQUAL(sequence.colorNormalized(0.49), term::Color(term::fg::Black, term::bg::Black));
        REQUIRE_EQUAL(sequence.colorNormalized(0.70), term::Color(term::fg::Blue, term::bg::Black));
        REQUIRE_EQUAL(sequence.colorNormalized(1.0), term::Color(term::fg::White, term::bg::Black));
        REQUIRE_EQUAL(sequence.colorNormalized(1.5), term::Color(term::fg::White, term::bg::Black));
    }
};
