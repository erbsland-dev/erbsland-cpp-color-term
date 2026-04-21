// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "RemappedBufferTestSupport.hpp"

#include <erbsland/cterm/RemappedBuffer.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <algorithm>
#include <array>
#include <format>
#include <random>
#include <string>
#include <vector>

TESTED_TARGETS(RemappedBuffer)
class RemappedBufferTest final : public UNITTEST_SUBCLASS(RemappedBufferTestSupport) {
public:
    void testConstructorCloneAndWideCharacters() {
        forEachOrientation([&](const Orientation orientation) -> void {
            const auto fillChar = Char{U'X', fg::Yellow, bg::Blue};
            auto buffer = RemappedBuffer{Size{3, 2}, orientation, fillChar};

            REQUIRE_EQUAL(buffer.size(), Size(3, 2));
            buffer.size().forEach([&](const Position pos) -> void {
                REQUIRE_EQUAL(buffer.get(pos), U'X');
                REQUIRE_EQUAL(buffer.get(pos).color(), Color(fg::Yellow, bg::Blue));
            });

            buffer.set(Position{0, 1}, Char{U'界', fg::Green, bg::Black});
            REQUIRE_EQUAL(buffer.get(Position{0, 1}), U'界');
            REQUIRE(buffer.get(Position{1, 1}).isEmpty());
            REQUIRE_EQUAL(buffer.get(Position{1, 1}).color(), Color(fg::Green, bg::Black));

            const auto clone = buffer.clone();
            REQUIRE(clone != nullptr);
            clone->set(Position{2, 0}, Char{U'Z', fg::Red, bg::Black});

            REQUIRE_EQUAL(buffer.get(Position{2, 0}), U'X');
            REQUIRE_EQUAL(clone->get(Position{2, 0}), U'Z');
            REQUIRE_EQUAL(clone->get(Position{2, 0}).color(), Color(fg::Red, bg::Black));
        });

        REQUIRE_THROWS_AS(std::invalid_argument, RemappedBuffer(Size{0, 1}));
        REQUIRE_THROWS_AS(std::invalid_argument, RemappedBuffer(Size{1, 0}));
        REQUIRE_THROWS_AS(std::invalid_argument, RemappedBuffer(Size{10'001, 1}));
    }

    void testResizeWithPreserveContentKeepsVisibleContent() {
        forEachOrientation([&](const Orientation orientation) -> void {
            auto buffer = createPatternBuffer(Size{3, 2}, orientation);
            auto model = createPatternModel(Size{3, 2});

            scramble(buffer, model);
            buffer.resize(Size{4, 3}, BufferResizeMode::PreserveContent, Char{U'.'});
            model.resize(Size{4, 3}, BufferResizeMode::PreserveContent, Char{U'.'});
            requireMatches(buffer, model, std::format("resize preserve expand {}", orientationName(orientation)));

            buffer.resize(Size{2, 2}, BufferResizeMode::PreserveContent, Char{U'.'});
            model.resize(Size{2, 2}, BufferResizeMode::PreserveContent, Char{U'.'});
            requireMatches(buffer, model, std::format("resize preserve shrink {}", orientationName(orientation)));
        });
    }

    void testResizeWithPreserveContentKeepsVisibleContentForPrimaryAxisChanges() {
        forEachOrientation([&](const Orientation orientation) -> void {
            auto buffer = createPatternBuffer(Size{4, 4}, orientation);
            auto model = createPatternModel(Size{4, 4});

            scramble(buffer, model);
            const auto expandedSize = orientation == Orientation::Vertical ? Size{4, 5} : Size{5, 4};
            buffer.resize(expandedSize, BufferResizeMode::PreserveContent, Char{U'.'});
            model.resize(expandedSize, BufferResizeMode::PreserveContent, Char{U'.'});
            requireMatches(buffer, model, std::format("primary-axis expand {}", orientationName(orientation)));

            const auto shrunkSize = orientation == Orientation::Vertical ? Size{4, 3} : Size{3, 4};
            buffer.resize(shrunkSize, BufferResizeMode::PreserveContent, Char{U'.'});
            model.resize(shrunkSize, BufferResizeMode::PreserveContent, Char{U'.'});
            requireMatches(buffer, model, std::format("primary-axis shrink {}", orientationName(orientation)));
        });
    }

    void testResizeWithPreserveContentFallsBackToFullPreserveForCrossAxisChanges() {
        forEachOrientation([&](const Orientation orientation) -> void {
            auto buffer = createPatternBuffer(Size{4, 3}, orientation);
            auto model = createPatternModel(Size{4, 3});

            scramble(buffer, model);
            const auto changedCrossAxisSize = orientation == Orientation::Vertical ? Size{5, 3} : Size{4, 4};
            buffer.resize(changedCrossAxisSize, BufferResizeMode::PreserveContent, Char{U'.'});
            model.resize(changedCrossAxisSize, BufferResizeMode::PreserveContent, Char{U'.'});
            requireMatches(buffer, model, std::format("primary-axis fallback {}", orientationName(orientation)));
        });
    }

    void testFastResizeKeepsBufferUsableAfterScrambledMaps() {
        forEachOrientation([&](const Orientation orientation) -> void {
            auto buffer = createPatternBuffer(Size{4, 3}, orientation);
            auto model = createPatternModel(Size{5, 4});

            scramble(buffer);
            buffer.resize(Size{5, 4});
            REQUIRE_EQUAL(buffer.size(), Size(5, 4));

            buffer.fill(Char{U'.', fg::White, bg::Black});
            fillPattern(buffer);
            requireMatches(buffer, model, std::format("fast resize refill {}", orientationName(orientation)));
        });
    }

    void testShiftRotateInsertEraseAndMoveMatchReferenceModel() {
        forEachOrientation([&](const Orientation orientation) -> void {
            auto buffer = createPatternBuffer(Size{5, 4}, orientation);
            auto model = createPatternModel(Size{5, 4});

            buffer.shift(Direction::NorthEast, Char{U'.'}, 1);
            model.shift(Direction::NorthEast, Char{U'.'}, 1);
            requireMatches(buffer, model, std::format("shift northeast {}", orientationName(orientation)));

            buffer.rotate(Direction::SouthWest, 2);
            model.rotate(Direction::SouthWest, 2);
            requireMatches(buffer, model, std::format("rotate southwest {}", orientationName(orientation)));

            buffer.insertRows(1, Char{U'+'}, 2);
            model.insertRows(1, Char{U'+'}, 2);
            requireMatches(buffer, model, std::format("insert rows {}", orientationName(orientation)));

            buffer.eraseColumns(2, Char{U'-'}, 2);
            model.eraseColumns(2, Char{U'-'}, 2);
            requireMatches(buffer, model, std::format("erase columns {}", orientationName(orientation)));

            buffer.moveRows(1, 2, -1, Char{U'#'});
            model.moveRows(1, 2, -1, Char{U'#'});
            requireMatches(buffer, model, std::format("move rows {}", orientationName(orientation)));

            buffer.moveColumns(1, 2, 2, Char{U'!'});
            model.moveColumns(1, 2, 2, Char{U'!'});
            requireMatches(buffer, model, std::format("move columns {}", orientationName(orientation)));
        });
    }

    void testMoveOperationsHandleOverflow() {
        forEachOrientation([&](const Orientation orientation) -> void {
            auto buffer = createPatternBuffer(Size{5, 4}, orientation);
            auto model = createPatternModel(Size{5, 4});

            buffer.moveRows(1, 3, -2, Char{U'^'});
            model.moveRows(1, 3, -2, Char{U'^'});
            requireMatches(buffer, model, std::format("move rows overflow up {}", orientationName(orientation)));

            buffer.moveColumns(0, 3, 3, Char{U'v'});
            model.moveColumns(0, 3, 3, Char{U'v'});
            requireMatches(buffer, model, std::format("move columns overflow right {}", orientationName(orientation)));
        });
    }

    void testMoveOperationsRecycleFullSpanOverflow() {
        forEachOrientation([&](const Orientation orientation) -> void {
            {
                auto buffer = createPatternBuffer(Size{5, 4}, orientation);
                auto model = createPatternModel(Size{5, 4});
                buffer.moveRows(0, buffer.size().height(), -buffer.size().height(), Char{U'^'});
                model.moveRows(0, model.size().height(), -model.size().height(), Char{U'^'});
                requireMatches(buffer, model, std::format("move all rows above {}", orientationName(orientation)));
            }
            {
                auto buffer = createPatternBuffer(Size{5, 4}, orientation);
                auto model = createPatternModel(Size{5, 4});
                buffer.moveRows(0, buffer.size().height(), buffer.size().height(), Char{U'v'});
                model.moveRows(0, model.size().height(), model.size().height(), Char{U'v'});
                requireMatches(buffer, model, std::format("move all rows below {}", orientationName(orientation)));
            }
            {
                auto buffer = createPatternBuffer(Size{5, 4}, orientation);
                auto model = createPatternModel(Size{5, 4});
                buffer.moveColumns(0, buffer.size().width(), -buffer.size().width(), Char{U'<'});
                model.moveColumns(0, model.size().width(), -model.size().width(), Char{U'<'});
                requireMatches(buffer, model, std::format("move all columns left {}", orientationName(orientation)));
            }
            {
                auto buffer = createPatternBuffer(Size{5, 4}, orientation);
                auto model = createPatternModel(Size{5, 4});
                buffer.moveColumns(0, buffer.size().width(), buffer.size().width(), Char{U'>'});
                model.moveColumns(0, model.size().width(), model.size().width(), Char{U'>'});
                requireMatches(buffer, model, std::format("move all columns right {}", orientationName(orientation)));
            }
        });
    }

    void testInvalidArgumentsAreRejected() {
        auto buffer = RemappedBuffer{Size{4, 3}, Orientation::Vertical};

        REQUIRE_THROWS_AS(std::invalid_argument, buffer.resize(Size{0, 3}));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.shift(Direction::North, Char::space(), -1));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.rotate(Direction::East, 5));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.eraseRows(2, Char::space(), 2));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.eraseColumns(-1, Char::space(), 1));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.insertRows(2, Char::space(), 2));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.insertColumns(3, Char::space(), 2));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.moveRows(1, 3, 1, Char::space()));
        REQUIRE_THROWS_AS(std::invalid_argument, buffer.moveColumns(3, 2, -1, Char::space()));
    }

    void testStressOperationsAgainstReferenceModel() {
        forEachOrientation([&](const Orientation orientation) -> void {
            auto buffer = createPatternBuffer(Size{6, 5}, orientation);
            auto model = createPatternModel(Size{6, 5});
            auto rng = std::mt19937{0xC0DE1234U + static_cast<uint32_t>(orientation == Orientation::Horizontal)};

            for (int step = 0; step < 250; ++step) {
                const auto operation = randomInt(rng, 0, 8);
                switch (operation) {
                case 0: {
                    const auto direction = randomCardinalOrDiagonal(rng);
                    const auto count = maxDirectionalCount(model.size(), direction) == 0
                        ? 0
                        : randomInt(rng, 0, maxDirectionalCount(model.size(), direction));
                    const auto fillChar = Char{static_cast<char32_t>(U'a' + (step % 26))};
                    buffer.shift(direction, fillChar, count);
                    model.shift(direction, fillChar, count);
                    break;
                }
                case 1: {
                    const auto direction = randomCardinalOrDiagonal(rng);
                    const auto count = maxDirectionalCount(model.size(), direction) == 0
                        ? 0
                        : randomInt(rng, 0, maxDirectionalCount(model.size(), direction));
                    buffer.rotate(direction, count);
                    model.rotate(direction, count);
                    break;
                }
                case 2: {
                    const auto count = randomInt(rng, 0, model.size().height());
                    const auto start =
                        count == 0 ? model.size().height() : randomInt(rng, 0, model.size().height() - count);
                    const auto fillChar = Char{static_cast<char32_t>(U'A' + (step % 26))};
                    buffer.eraseRows(start, fillChar, count);
                    model.eraseRows(start, fillChar, count);
                    break;
                }
                case 3: {
                    const auto count = randomInt(rng, 0, model.size().width());
                    const auto start =
                        count == 0 ? model.size().width() : randomInt(rng, 0, model.size().width() - count);
                    const auto fillChar = Char{static_cast<char32_t>(U'0' + (step % 10))};
                    buffer.insertColumns(start, fillChar, count);
                    model.insertColumns(start, fillChar, count);
                    break;
                }
                case 4: {
                    const auto count = randomInt(rng, 0, model.size().height());
                    const auto start =
                        count == 0 ? model.size().height() : randomInt(rng, 0, model.size().height() - count);
                    const auto delta = randomInt(rng, -model.size().height(), model.size().height());
                    const auto fillChar = Char{static_cast<char32_t>(U'k' + (step % 10))};
                    buffer.moveRows(start, count, delta, fillChar);
                    model.moveRows(start, count, delta, fillChar);
                    break;
                }
                case 5: {
                    const auto count = randomInt(rng, 0, model.size().width());
                    const auto start =
                        count == 0 ? model.size().width() : randomInt(rng, 0, model.size().width() - count);
                    const auto delta = randomInt(rng, -model.size().width(), model.size().width());
                    const auto fillChar = Char{static_cast<char32_t>(U'p' + (step % 10))};
                    buffer.moveColumns(start, count, delta, fillChar);
                    model.moveColumns(start, count, delta, fillChar);
                    break;
                }
                case 6: {
                    const auto newSize = Size{
                        randomInt(rng, 1, std::max(1, model.size().width() + 1)),
                        randomInt(rng, 1, std::max(1, model.size().height() + 1))};
                    const auto fillChar = Char{static_cast<char32_t>(U'Z' - (step % 20))};
                    buffer.resize(newSize, BufferResizeMode::PreserveContent, fillChar);
                    model.resize(newSize, BufferResizeMode::PreserveContent, fillChar);
                    break;
                }
                case 7: {
                    const auto pos = Position{
                        randomInt(rng, 0, model.size().width() - 1), randomInt(rng, 0, model.size().height() - 1)};
                    const auto value = Char{static_cast<char32_t>(U'!' + (step % 60))};
                    buffer.set(pos, value);
                    model.set(pos, value);
                    break;
                }
                case 8: {
                    const auto fillChar = Char{static_cast<char32_t>(U'.' + (step % 20))};
                    buffer.fill(fillChar);
                    model.fill(fillChar);
                    break;
                }
                default:
                    REQUIRE(false);
                }
                requireMatches(buffer, model, std::format("stress step {} {}", step, orientationName(orientation)));
            }
        });
    }
};
