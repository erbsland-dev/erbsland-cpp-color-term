// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/BufferTestHelper.hpp"

#include <erbsland/cterm/WriteClippedBuffer.hpp>
#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(WriteClippedBuffer)
class WriteClippedBufferTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testReferenceWrapperClipsAndOffsetsReadsAndWrites() {
        auto buffer = createBuffer({
            "......",
            "......",
            "......",
            "......",
        });
        auto view = WriteClippedBufferRef{buffer, Position{10, 20}, Rectangle{2, 1, 3, 2}};

        REQUIRE_EQUAL(view.size(), (Size{3, 2}));
        REQUIRE_EQUAL(view.rect(), (Rectangle{10, 20, 3, 2}));
        REQUIRE_EQUAL(view.sourceRect(), (Rectangle{10, 20, 3, 2}));
        REQUIRE_EQUAL(view.targetRect(), (Rectangle{2, 1, 3, 2}));

        view.fill(Rectangle{11, 20, 4, 2}, Char{U'+'});
        view.set(Position{10, 20}, Char{U'A'});
        view.set(Position{12, 21}, Char{U'B'});
        view.set(Position{9, 20}, Char{U'X'});
        view.set(Position{13, 21}, Char{U'Y'});
        view.set(Position{12, 20}, Char{U'界'});
        view.set(Position{9, 21}, String{"ZCD"});

        requireRowsEqual(
            buffer,
            {
                "......",
                "..A++.",
                "..CDB.",
                "......",
            });
        REQUIRE_EQUAL(view.get(Position{10, 20}), U'A');
        REQUIRE_EQUAL(view.get(Position{12, 20}), U'+');
        REQUIRE_EQUAL(view.get(Position{13, 20}), U'.');
    }

    void testSharedWrapperHandlesNullContentAndReplacement() {
        auto view = WriteClippedBuffer{Size{2, 1}};

        REQUIRE(view.content() == nullptr);
        REQUIRE_EQUAL(view.get(Position{0, 0}), U' ');
        view.set(Position{0, 0}, Char{U'X'});

        auto firstBuffer = createSharedBuffer({".."});
        view.setContent(firstBuffer);
        view.set(Position{0, 0}, Char{U'A'});

        REQUIRE(view.content() == firstBuffer);
        requireRowsEqual(*firstBuffer, {"A."});

        auto secondBuffer = createSharedBuffer({".."});
        view.setContent(secondBuffer);
        view.set(Position{1, 0}, Char{U'B'});

        requireRowsEqual(*firstBuffer, {"A."});
        requireRowsEqual(*secondBuffer, {".B"});
    }

    void testResizeOnlyChangesTheWrapperAndCloneIsZeroBased() {
        auto buffer = createBuffer({
            "ABCDE",
            "FGHIJ",
            "KLMNO",
        });
        auto view = WriteClippedBufferRef{buffer, Position{5, 7}, Rectangle{1, 1, 3, 2}};

        const auto clone = view.clone();
        requireRowsEqual(
            *clone,
            {
                "GHI",
                "LMN",
            });

        view.resize(Size{2, 1}, BufferResizeMode::PreserveContent, Char{U'?'});

        REQUIRE_EQUAL(buffer.size(), (Size{5, 3}));
        REQUIRE_EQUAL(view.size(), (Size{2, 1}));
        REQUIRE_EQUAL(view.rect(), (Rectangle{5, 7, 2, 1}));
        REQUIRE_EQUAL(view.targetRect(), (Rectangle{1, 1, 2, 1}));
    }

    void testInheritedDrawingPathsStayInsideTheTargetRectangle() {
        auto buffer = createBuffer({
            ".......",
            ".......",
            ".......",
            ".......",
            ".......",
        });
        auto view = WriteClippedBufferRef{buffer, Position{100, 200}, Rectangle{1, 1, 5, 3}};

        view.drawFrame(Rectangle{100, 200, 5, 3}, Char{U'#'});
        auto source = createBuffer({"abcdef"});
        view.drawBuffer(source, BufferDrawOptions{Rectangle{98, 201, 6, 1}, Rectangle{0, 0, 6, 1}});
        view.drawText(Position{99, 201}, String{"WXY"});

        requireRowsEqual(
            buffer,
            {
                ".......",
                ".#####.",
                ".XYef#.",
                ".#####.",
                ".......",
            });
    }

    void testTargetRectanglePartiallyOutsideWrappedBufferIsClipped() {
        auto buffer = createBuffer({
            "....",
            "....",
        });
        auto view = WriteClippedBufferRef{buffer, Position{0, 0}, Rectangle{2, 0, 4, 2}};

        view.fill(Char{U'*'});

        requireRowsEqual(
            buffer,
            {
                "..**",
                "..**",
            });
        REQUIRE_EQUAL(view.get(Position{0, 0}), U'*');
        REQUIRE_EQUAL(view.get(Position{1, 0}), U'*');
        REQUIRE_EQUAL(view.get(Position{2, 0}), U' ');
    }
};
