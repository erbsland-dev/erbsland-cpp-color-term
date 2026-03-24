// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>


TESTED_TARGETS(BufferView)
class BufferViewTest final : public el::UnitTest {
public:
    void testCloneCopiesTheVisibleAreaAndTracksTheAssignedContent() {
        auto content = createBuffer({
            "ABC",
            "DEF",
            "GHI",
        });
        auto view = BufferView{content, Rectangle{Position{1, 1}, Size{2, 2}}};
        const auto expectedSize = Size{2, 2};
        const auto expectedRect = Rectangle{Position{0, 0}, expectedSize};
        const auto expectedViewRect = Rectangle{Position{1, 1}, expectedSize};

        REQUIRE(view.content() == content);
        REQUIRE_EQUAL(view.size(), expectedSize);
        REQUIRE_EQUAL(view.rect(), expectedRect);
        REQUIRE_EQUAL(view.viewRect(), expectedViewRect);
        REQUIRE_EQUAL(view.get(Position{0, 0}), U'E');
        REQUIRE_EQUAL(view.get(Position{1, 0}), U'F');
        REQUIRE_EQUAL(view.get(Position{0, 1}), U'H');
        REQUIRE_EQUAL(view.get(Position{1, 1}), U'I');

        const auto clone = view.clone();
        REQUIRE_EQUAL(clone->size(), expectedSize);
        REQUIRE_EQUAL(clone->get(Position{0, 0}), U'E');
        REQUIRE_EQUAL(clone->get(Position{1, 0}), U'F');
        REQUIRE_EQUAL(clone->get(Position{0, 1}), U'H');
        REQUIRE_EQUAL(clone->get(Position{1, 1}), U'I');

        auto replacement = createBuffer({"Z"});
        view.setContent(replacement);
        view.setViewRect(Rectangle{Position{0, 0}, Size{1, 1}});
        const auto expectedReplacementRect = Rectangle{Position{0, 0}, Size{1, 1}};

        REQUIRE(view.content() == replacement);
        REQUIRE_EQUAL(view.viewRect(), expectedReplacementRect);
        REQUIRE_EQUAL(view.get(Position{0, 0}), U'Z');
    }

    void testCropCharactersCanBeConfiguredAndAreIgnoredForInvalidDirections() {
        auto content = createBuffer({"A"});
        auto view = BufferView{content, Size{0, 0}};

        REQUIRE_FALSE(view.showCropCharacters());
        REQUIRE_EQUAL(view.get(Position{0, 0}), U' ');

        view.setShowCropCharacters(true);
        view.setCropCharacter(Direction::None, Char{U'.'});
        view.setCropCharacter(Direction::East, Char{U'>'});

        REQUIRE(view.showCropCharacters());
        REQUIRE_EQUAL(view.cropCharacter(Direction::None), U'.');
        REQUIRE_EQUAL(view.cropCharacter(Direction::East), U'>');

        const auto invalidDirection = Direction{static_cast<Direction::Enum>(99)};
        REQUIRE_EQUAL(view.cropCharacter(invalidDirection), Char{});

        view.setCropCharacter(invalidDirection, Char{U'!'});
        REQUIRE_EQUAL(view.cropCharacter(Direction::East), U'>');
    }

    void testViewsRenderConfiguredCropMarksForSharedAndReferencedContent() {
        auto content = createBuffer({
            "ABC",
            "DEF",
            "GHI",
        });
        auto view = BufferView{content, Rectangle{Position{0, 0}, Size{2, 2}}};
        view.setShowCropCharacters(true);
        view.setCropCharacter(Direction::East, Char{U'>'});
        view.setCropCharacter(Direction::South, Char{U'v'});
        view.setCropCharacter(Direction::SouthEast, Char{U'x'});

        REQUIRE_EQUAL(view.get(Position{0, 0}), U'A');
        REQUIRE_EQUAL(view.get(Position{1, 0}), U'>');
        REQUIRE_EQUAL(view.get(Position{0, 1}), U'v');
        REQUIRE_EQUAL(view.get(Position{1, 1}), U'x');

        auto refView = BufferConstRefView{*content, Rectangle{Position{0, 0}, Size{2, 2}}};
        refView.setShowCropCharacters(true);
        refView.setCropCharacter(Direction::East, Char{U']'});
        refView.setCropCharacter(Direction::South, Char{U'_'});
        refView.setCropCharacter(Direction::SouthEast, Char{U'+'});

        REQUIRE_EQUAL(refView.get(Position{0, 0}), U'A');
        REQUIRE_EQUAL(refView.get(Position{1, 0}), U']');
        REQUIRE_EQUAL(refView.get(Position{0, 1}), U'_');
        REQUIRE_EQUAL(refView.get(Position{1, 1}), U'+');
    }

private:
    [[nodiscard]] static auto createBuffer(const std::initializer_list<std::string_view> rows)
        -> std::shared_ptr<Buffer> {
        auto width = Coordinate{0};
        for (const auto row : rows) {
            if (width == 0) {
                width = static_cast<Coordinate>(row.size());
            }
        }
        auto buffer = std::make_shared<Buffer>(Size{width, static_cast<Coordinate>(rows.size())});
        auto y = Coordinate{0};
        for (const auto row : rows) {
            auto x = Coordinate{0};
            for (const auto value : row) {
                buffer->set(Position{x, y}, Char{static_cast<char32_t>(static_cast<unsigned char>(value))});
                x += 1;
            }
            y += 1;
        }
        return buffer;
    }
};
