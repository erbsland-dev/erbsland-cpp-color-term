// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "BufferTestHelper.hpp"

#include <erbsland/cterm/ui/surface/AbstractStatusLine.hpp>
#include <erbsland/cterm/ui/surface/StatusLine.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <array>
#include <format>
#include <memory>

namespace ui = erbsland::cterm::ui;

namespace ui_status_line_test {

class TestStatusLine final : public ui::AbstractStatusLine {
public:
    explicit TestStatusLine(ProtectedTag) noexcept : AbstractStatusLine{ProtectedTag{}} {}

    [[nodiscard]] static auto create() noexcept -> std::shared_ptr<TestStatusLine> {
        return std::make_shared<TestStatusLine>(ProtectedTag{});
    }

public:
    [[nodiscard]] auto text(const Section section) const -> const String & override {
        return _texts[static_cast<std::size_t>(section)];
    }

    [[nodiscard]] auto collapseBehavior(const Section section) const noexcept -> CollapseBehavior override {
        return _behaviors[static_cast<std::size_t>(section)];
    }

    [[nodiscard]] auto margins(const Section section) const -> Margins override {
        return _margins[static_cast<std::size_t>(section)];
    }

    void setText(const Section section, String text) { _texts[static_cast<std::size_t>(section)] = std::move(text); }

    void setCollapseBehavior(const Section section, const CollapseBehavior behavior) noexcept {
        _behaviors[static_cast<std::size_t>(section)] = behavior;
    }

    void setMargins(const Section section, const Margins margins) noexcept {
        _margins[static_cast<std::size_t>(section)] = margins;
    }

private:
    std::array<String, 3> _texts;
    std::array<CollapseBehavior, 3> _behaviors{
        CollapseBehavior::Ellipsis,
        CollapseBehavior::Ellipsis,
        CollapseBehavior::Ellipsis,
    };
    std::array<Margins, 3> _margins{};
};

}

TESTED_TARGETS(UiStatusLine)
class UiStatusLineTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testAbstractStatusLineAnchorsLeftMiddleAndRightSections() {
        auto statusLine = ui_status_line_test::TestStatusLine::create();
        auto buffer = Buffer{Size{20, 1}, Char{U'.'}};

        statusLine->setText(ui::AbstractStatusLine::Section::Left, String{"LEFT"});
        statusLine->setText(ui::AbstractStatusLine::Section::Middle, String{"MID"});
        statusLine->setText(ui::AbstractStatusLine::Section::Right, String{"RIGHT"});
        statusLine->setRectangle(Rectangle{0, 0, 20, 1});
        statusLine->onLayout(Size{20, 1});
        statusLine->onPaint(buffer, ui::PaintContext{buffer.rect()});

        requireRowsEqual(buffer, {"LEFT....MID....RIGHT"});
    }

    void testAbstractStatusLineAppliesEllipsisAndHideCollapseBehavior() {
        auto statusLine = ui_status_line_test::TestStatusLine::create();
        auto ellipsisBuffer = Buffer{Size{6, 1}, Char{U'.'}};

        statusLine->setText(ui::AbstractStatusLine::Section::Left, String{"ABCDEFG"});
        statusLine->setRectangle(Rectangle{0, 0, 6, 1});
        statusLine->onLayout(Size{6, 1});
        statusLine->onPaint(ellipsisBuffer, ui::PaintContext{ellipsisBuffer.rect()});

        requireRowsEqual(ellipsisBuffer, {"ABCDE…"});

        auto hideBuffer = Buffer{Size{4, 1}, Char{U'.'}};
        statusLine->setText(ui::AbstractStatusLine::Section::Left, String{});
        statusLine->setText(ui::AbstractStatusLine::Section::Right, String{"RIGHT"});
        statusLine->setCollapseBehavior(
            ui::AbstractStatusLine::Section::Right, ui::AbstractStatusLine::CollapseBehavior::Hide);
        statusLine->setRectangle(Rectangle{0, 0, 4, 1});
        statusLine->onLayout(Size{4, 1});
        statusLine->onPaint(hideBuffer, ui::PaintContext{hideBuffer.rect()});

        requireRowsEqual(hideBuffer, {"...."});
    }

    void testAbstractStatusLineNeverCollapseAllowsOverlappingSections() {
        auto statusLine = ui_status_line_test::TestStatusLine::create();
        auto buffer = Buffer{Size{10, 1}, Char{U'.'}};

        statusLine->setText(ui::AbstractStatusLine::Section::Left, String{"LEFTSECTION"});
        statusLine->setText(ui::AbstractStatusLine::Section::Right, String{"RIGHTSIDE"});
        statusLine->setCollapseBehavior(
            ui::AbstractStatusLine::Section::Left, ui::AbstractStatusLine::CollapseBehavior::Never);
        statusLine->setCollapseBehavior(
            ui::AbstractStatusLine::Section::Right, ui::AbstractStatusLine::CollapseBehavior::Never);
        statusLine->setRectangle(Rectangle{0, 0, 10, 1});
        statusLine->onLayout(Size{10, 1});
        statusLine->onPaint(buffer, ui::PaintContext{buffer.rect()});

        requireRowsEqual(buffer, {"LRIGHTSIDE"});
    }

    void testAbstractStatusLineMarginsInsetVisibleSectionsAndExposeSectionWidths() {
        auto statusLine = ui_status_line_test::TestStatusLine::create();
        auto buffer = Buffer{Size{10, 1}, Char{U'.'}};

        statusLine->setText(ui::AbstractStatusLine::Section::Left, String{"LEFT"});
        statusLine->setMargins(ui::AbstractStatusLine::Section::Left, Margins{0, 1, 0, 1});
        statusLine->setRectangle(Rectangle{0, 0, 10, 1});
        statusLine->onLayout(Size{10, 1});
        statusLine->onPaint(buffer, ui::PaintContext{buffer.rect()});

        REQUIRE_EQUAL(statusLine->sectionWidth(ui::AbstractStatusLine::Section::Left), 8);
        REQUIRE_EQUAL(statusLine->sectionWidth(ui::AbstractStatusLine::Section::Middle), 0);
        REQUIRE_EQUAL(statusLine->sectionWidth(ui::AbstractStatusLine::Section::Right), 0);
        requireRowsEqual(buffer, {".LEFT....."});
    }

    void testConcreteStatusLineStaticModeNeverCallsUpdater() {
        auto statusLine = ui::StatusLine::create();
        auto buffer = Buffer{Size{10, 1}, Char{U'.'}};
        auto updateCount = 0;

        statusLine->setText(ui::StatusLine::Section::Left, "STATIC");
        statusLine->setUpdateFn(
            ui::StatusLine::Section::Left, [&updateCount](String &, const Coordinate) { ++updateCount; });
        statusLine->setUpdateMode(ui::StatusLine::Section::Left, ui::StatusLine::UpdateMode::Static);
        statusLine->setRectangle(Rectangle{0, 0, 10, 1});
        statusLine->onLayout(Size{10, 1});
        statusLine->onPaint(buffer, ui::PaintContext{buffer.rect()});

        REQUIRE_EQUAL(updateCount, 0);
        requireRowsEqual(buffer, {"STATIC...."});
    }

    void testConcreteStatusLineWithoutUpdaterDoesNotRefreshAutomatically() {
        auto statusLine = ui::StatusLine::create();
        auto buffer = Buffer{Size{10, 1}, Char{U'.'}};

        statusLine->setText(ui::StatusLine::Section::Left, "STATIC");
        statusLine->setUpdateMode(ui::StatusLine::Section::Left, ui::StatusLine::UpdateMode::OnRefresh);
        statusLine->setRectangle(Rectangle{0, 0, 10, 1});
        statusLine->onLayout(Size{10, 1});
        statusLine->onPaint(buffer, ui::PaintContext{buffer.rect()});

        requireRowsEqual(buffer, {"STATIC...."});
    }

    void testConcreteStatusLineRefreshModeUpdatesBeforeEveryPaint() {
        auto statusLine = ui::StatusLine::create();
        auto firstBuffer = Buffer{Size{10, 1}, Char{U'.'}};
        auto secondBuffer = Buffer{Size{10, 1}, Char{U'.'}};
        auto updateCount = 0;

        statusLine->setUpdateMode(ui::StatusLine::Section::Left, ui::StatusLine::UpdateMode::OnRefresh);
        statusLine->setUpdateFn(ui::StatusLine::Section::Left, [&updateCount](String &text, const Coordinate width) {
            ++updateCount;
            text = String{std::format("R{}-{}", updateCount, width)};
        });
        statusLine->setRectangle(Rectangle{0, 0, 10, 1});
        statusLine->onLayout(Size{10, 1});
        statusLine->onPaint(firstBuffer, ui::PaintContext{firstBuffer.rect()});
        statusLine->onPaint(secondBuffer, ui::PaintContext{secondBuffer.rect()});

        REQUIRE_EQUAL(updateCount, 2);
        requireRowsEqual(firstBuffer, {"R1-10....."});
        requireRowsEqual(secondBuffer, {"R2-10....."});
    }

    void testConcreteStatusLineResizeModeUpdatesOnlyWhenWidthChanges() {
        auto statusLine = ui::StatusLine::create();
        auto firstBuffer = Buffer{Size{10, 1}, Char{U'.'}};
        auto secondBuffer = Buffer{Size{10, 1}, Char{U'.'}};
        auto thirdBuffer = Buffer{Size{8, 1}, Char{U'.'}};
        auto updateCount = 0;

        statusLine->setUpdateMode(ui::StatusLine::Section::Left, ui::StatusLine::UpdateMode::OnResize);
        statusLine->setUpdateFn(ui::StatusLine::Section::Left, [&updateCount](String &text, const Coordinate width) {
            ++updateCount;
            text = String{std::format("W{}", width)};
        });

        statusLine->setRectangle(Rectangle{0, 0, 10, 1});
        statusLine->onLayout(Size{10, 1});
        statusLine->onPaint(firstBuffer, ui::PaintContext{firstBuffer.rect()});

        statusLine->setRectangle(Rectangle{0, 0, 10, 1});
        statusLine->onLayout(Size{10, 1});
        statusLine->onPaint(secondBuffer, ui::PaintContext{secondBuffer.rect()});

        statusLine->setRectangle(Rectangle{0, 0, 8, 1});
        statusLine->onLayout(Size{8, 1});
        statusLine->onPaint(thirdBuffer, ui::PaintContext{thirdBuffer.rect()});

        REQUIRE_EQUAL(updateCount, 2);
        requireRowsEqual(firstBuffer, {"W10......."});
        requireRowsEqual(secondBuffer, {"W10......."});
        requireRowsEqual(thirdBuffer, {"W8......"});
    }

    void testConcreteStatusLineMarginsAffectResizeUpdatesAndWidthArguments() {
        auto statusLine = ui::StatusLine::create();
        auto firstBuffer = Buffer{Size{20, 1}, Char{U'.'}};
        auto secondBuffer = Buffer{Size{20, 1}, Char{U'.'}};
        auto lastWidth = Coordinate{-1};
        auto updateCount = 0;

        statusLine->setText(ui::StatusLine::Section::Left, "LEFT");
        statusLine->setMargins(ui::StatusLine::Section::Right, Margins{0, 1, 0, 1});
        statusLine->setUpdateMode(ui::StatusLine::Section::Right, ui::StatusLine::UpdateMode::OnResize);
        statusLine->setUpdateFn(
            ui::StatusLine::Section::Right, [&lastWidth, &updateCount](String &text, const Coordinate width) {
                ++updateCount;
                lastWidth = width;
                text = String{std::format("R{}", width)};
            });

        statusLine->setRectangle(Rectangle{0, 0, 20, 1});
        statusLine->onLayout(Size{20, 1});
        statusLine->onPaint(firstBuffer, ui::PaintContext{firstBuffer.rect()});

        REQUIRE_EQUAL(lastWidth, 14);
        REQUIRE_EQUAL(updateCount, 1);

        statusLine->setMargins(ui::StatusLine::Section::Right, Margins{0, 2, 0, 2});
        statusLine->onPaint(secondBuffer, ui::PaintContext{secondBuffer.rect()});

        REQUIRE_EQUAL(lastWidth, 12);
        REQUIRE_EQUAL(updateCount, 2);
        requireRowsEqual(firstBuffer, {"LEFT............R14."});
        requireRowsEqual(secondBuffer, {"LEFT...........R12.."});
    }
};
