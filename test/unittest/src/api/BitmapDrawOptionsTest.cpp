// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(BitmapDrawOptions)
class BitmapDrawOptionsTest final : public el::UnitTest {
public:
    void testDefaultOptionsExposeTheExpectedDefaults() {
        const auto &options = BitmapDrawOptions::defaultOptions();

        REQUIRE_EQUAL(options.color().sequenceLength(), static_cast<std::size_t>(1));
        REQUIRE_EQUAL(options.color().color(0), Color(fg::Default, bg::Default));
        REQUIRE_EQUAL(options.colorMode(), BitmapColorMode::OneColor);
        REQUIRE_EQUAL(options.colorAnimationOffset(), static_cast<std::size_t>(0));
        REQUIRE(options.char16Style() == nullptr);
        REQUIRE(options.combinationStyle() == nullptr);
        REQUIRE_EQUAL(options.fullBlock(), U'█');
        REQUIRE_EQUAL(options.doubleBlocks().size(), static_cast<std::size_t>(2));
        REQUIRE_EQUAL(options.doubleBlocks()[0], U'█');
        REQUIRE_EQUAL(options.doubleBlocks()[1], U'█');
        REQUIRE_EQUAL(options.halfBlocks().size(), static_cast<std::size_t>(16));
        REQUIRE_EQUAL(options.halfBlocks()[0], U' ');
        REQUIRE_EQUAL(options.halfBlocks()[15], U'█');
        REQUIRE_EQUAL(options.scaleMode(), BitmapScaleMode::HalfBlock);
        REQUIRE(&BitmapDrawOptions::defaultOptions() == &options);
    }

    void testColorSequenceAndSettersReplaceTheConfiguredBaseColors() {
        auto options = BitmapDrawOptions{
            ColorSequence{
                Color{fg::Red, bg::Black},
                Color{fg::Blue, bg::Black},
            },
            BitmapColorMode::VerticalStripes};

        REQUIRE_EQUAL(options.color().sequenceLength(), static_cast<std::size_t>(2));
        REQUIRE_EQUAL(options.color().color(0), Color(fg::Red, bg::Black));
        REQUIRE_EQUAL(options.color().color(1), Color(fg::Blue, bg::Black));
        REQUIRE_EQUAL(options.colorMode(), BitmapColorMode::VerticalStripes);

        options.setColorMode(BitmapColorMode::HorizontalStripes);
        options.setColorAnimationOffset(5);
        REQUIRE_EQUAL(options.colorMode(), BitmapColorMode::HorizontalStripes);
        REQUIRE_EQUAL(options.colorAnimationOffset(), static_cast<std::size_t>(5));

        options.setColor(Color{fg::Yellow, bg::Blue});
        REQUIRE_EQUAL(options.color().sequenceLength(), static_cast<std::size_t>(1));
        REQUIRE_EQUAL(options.color().color(0), Color(fg::Yellow, bg::Blue));

        options.setColor({});
        REQUIRE_EQUAL(options.color().color(0), Color{});

        options.setColor(Foreground{fg::Green});
        REQUIRE_EQUAL(options.color().color(0), Color(fg::Green, bg::Inherited));

        options.setColor(fg::Magenta);
        REQUIRE_EQUAL(options.color().color(0), Color(fg::Magenta, bg::Inherited));

        options.setColor(Foreground{fg::Cyan}, Background{bg::Black});
        REQUIRE_EQUAL(options.color().color(0), Color(fg::Cyan, bg::Black));

        options.setColor(Background{bg::Yellow});
        REQUIRE_EQUAL(options.color().color(0), Color(fg::Inherited, bg::Yellow));

        options.setColor(bg::Blue);
        REQUIRE_EQUAL(options.color().color(0), Color(fg::Inherited, bg::Blue));

        options.setColorSequence(
            ColorSequence{Color{fg::BrightWhite, bg::Red}}, BitmapColorMode::BackwardDiagonalStripes);
        REQUIRE_EQUAL(options.color().sequenceLength(), static_cast<std::size_t>(1));
        REQUIRE_EQUAL(options.color().color(0), Color(fg::BrightWhite, bg::Red));
        REQUIRE_EQUAL(options.colorMode(), BitmapColorMode::BackwardDiagonalStripes);
    }

    void testStylePointersAndConfiguredBlocksCanBeAccessed() {
        auto options = BitmapDrawOptions{};
        const auto char16Style = Char16Style::lightFrame();
        const auto combinationStyle = CharCombinationStyle::colorOverlay();

        options.setChar16Style(char16Style);
        options.setCombinationStyle(combinationStyle);
        options.setScaleMode(BitmapScaleMode::DoubleBlock);
        options.setFullBlock(Char{U'#', fg::Green, bg::Black});
        options.setDoubleBlocks(String{"[]"});
        options.setHalfBlocks(String{"abcdefghijklmnop"});

        REQUIRE(options.char16Style() == char16Style);
        REQUIRE(options.combinationStyle() == combinationStyle);
        REQUIRE_EQUAL(options.scaleMode(), BitmapScaleMode::DoubleBlock);
        REQUIRE_EQUAL(options.fullBlock(), U'#');
        REQUIRE_EQUAL(options.fullBlock().color(), Color(fg::Green, bg::Black));
        REQUIRE_EQUAL(options.doubleBlocks().size(), static_cast<std::size_t>(2));
        REQUIRE_EQUAL(options.doubleBlocks()[0], U'[');
        REQUIRE_EQUAL(options.doubleBlocks()[1], U']');
        REQUIRE_EQUAL(options.halfBlocks().size(), static_cast<std::size_t>(16));
        REQUIRE_EQUAL(options.halfBlocks()[0], U'a');
        REQUIRE_EQUAL(options.halfBlocks()[15], U'p');
    }

    void testBlockValidationRejectsInvalidWidthsAndCounts() {
        auto options = BitmapDrawOptions{};

        REQUIRE_THROWS_AS(std::invalid_argument, options.setFullBlock(Char{U'界'}));
        REQUIRE_THROWS_AS(std::invalid_argument, options.setDoubleBlocks(String{"X"}));
        REQUIRE_THROWS_AS(std::invalid_argument, options.setDoubleBlocks(String{U"界X"}));
        REQUIRE_THROWS_AS(std::invalid_argument, options.setHalfBlocks(String{"short"}));
        REQUIRE_THROWS_AS(std::invalid_argument, options.setHalfBlocks(String{U"abcdefghijklmno界"}));
    }
};
