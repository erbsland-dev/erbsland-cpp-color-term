// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "UiSchedulerManualClock.hpp"

#include "support/TerminalTestHelper.hpp"

#include <erbsland/cterm/ui/event/impl/EventClockAccess.hpp>
#include <erbsland/cterm/ui/surface/DynamicTextLine.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <memory>
#include <string>
#include <vector>

struct PaintCall final {
    std::string name;
    Rectangle surfaceRect;
    Rectangle visibleRect;
    Rectangle dirtyRect;
};

class PaintProbeSurface final : public ui::Surface {
public:
    explicit PaintProbeSurface(
        std::shared_ptr<std::vector<PaintCall>> calls,
        std::string name,
        Char paintChar,
        const bool opaque,
        ProtectedTag) :
        _calls{std::move(calls)}, _name{std::move(name)}, _paintChar{paintChar}, _opaque{opaque} {}

    [[nodiscard]] static auto create(
        const std::shared_ptr<std::vector<PaintCall>> &calls,
        std::string name,
        Char paintChar,
        const bool opaque = false) -> std::shared_ptr<PaintProbeSurface> {
        return std::make_shared<PaintProbeSurface>(calls, std::move(name), paintChar, opaque, ProtectedTag{});
    }

public: // implement Surface
    [[nodiscard]] auto isOpaque() const noexcept -> bool override { return _opaque; }

    void onPaint(WritableBuffer &buffer, const ui::PaintContext &context) noexcept override {
        _calls->push_back(PaintCall{_name, context.surfaceRect(), context.visibleRect(), context.dirtyRect()});
        buffer.fill(context.surfaceRect(), _paintChar);
        if (_invalidateDuringNextPaint) {
            _invalidateDuringNextPaint = false;
            flags().setPaintOutdated();
        }
    }

public:
    void invalidateDuringNextPaint() noexcept { _invalidateDuringNextPaint = true; }

private:
    std::shared_ptr<std::vector<PaintCall>> _calls;
    std::string _name;
    Char _paintChar;
    bool _opaque{false};
    bool _invalidateDuringNextPaint{false};
};

TESTED_TARGETS(UiDisplay UiSurface UiPaintContext)
class UiPaintSystemTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testOpaqueDirtyChildSkipsParentPaint() {
        auto setup = createSetup(Size{8, 2});
        auto child = PaintProbeSurface::create(setup.calls, "child", Char{U'C'}, true);
        setup.root->addSurface(child);
        child->setRectangle(Rectangle{1, 0, 2, 1});
        renderInitialFrame(setup);

        child->flags().setPaintOutdated();
        renderNextFrame(setup);

        requireCallNames(setup, {"child"});
        REQUIRE_FALSE(child->flags().isPaintOutdated());
    }

    void testTransparentDirtyChildRepaintsParentWithLocalDirtyRect() {
        auto setup = createSetup(Size{8, 2});
        auto child = PaintProbeSurface::create(setup.calls, "child", Char{U'T'}, false);
        setup.root->addSurface(child);
        child->setRectangle(Rectangle{2, 1, 3, 1});
        renderInitialFrame(setup);

        child->flags().setPaintOutdated();
        renderNextFrame(setup);

        requireCallNames(setup, {"root", "child"});
        REQUIRE_EQUAL(setup.calls->at(0).dirtyRect, Rectangle(2, 1, 3, 1));
        REQUIRE_EQUAL(setup.calls->at(1).dirtyRect, Rectangle(0, 0, 3, 1));
    }

    void testUnrelatedSurfaceOutsideDamageIsNotPainted() {
        auto setup = createSetup(Size{10, 2});
        auto dirtyChild = PaintProbeSurface::create(setup.calls, "dirty", Char{U'D'}, false);
        auto unrelated = PaintProbeSurface::create(setup.calls, "unrelated", Char{U'U'}, true);
        setup.root->addSurface(dirtyChild);
        setup.root->addSurface(unrelated);
        dirtyChild->setRectangle(Rectangle{0, 0, 2, 1});
        unrelated->setRectangle(Rectangle{6, 0, 2, 1});
        renderInitialFrame(setup);

        dirtyChild->flags().setPaintOutdated();
        renderNextFrame(setup);

        requireCallNames(setup, {"root", "dirty"});
    }

    void testPartiallyClippedChildReceivesLocalVisibleRect() {
        auto setup = createSetup(Size{5, 3});
        auto child = PaintProbeSurface::create(setup.calls, "child", Char{U'C'}, true);
        setup.root->addSurface(child);
        child->setRectangle(Rectangle{-2, 1, 5, 1});
        renderInitialFrame(setup);

        child->flags().setPaintOutdated();
        renderNextFrame(setup);

        requireCallNames(setup, {"child"});
        REQUIRE_EQUAL(setup.calls->at(0).surfaceRect, Rectangle(0, 0, 5, 1));
        REQUIRE_EQUAL(setup.calls->at(0).visibleRect, Rectangle(2, 0, 3, 1));
        REQUIRE_EQUAL(setup.calls->at(0).dirtyRect, Rectangle(2, 0, 3, 1));
    }

    void testFullyInvisibleDirtyChildStaysDirtyUntilVisible() {
        auto setup = createSetup(Size{5, 2});
        auto child = PaintProbeSurface::create(setup.calls, "child", Char{U'C'}, true);
        setup.root->addSurface(child);
        child->setRectangle(Rectangle{6, 0, 2, 1});
        renderInitialFrame(setup);

        child->flags().setPaintOutdated();
        renderNextFrame(setup);

        REQUIRE(setup.calls->empty());
        REQUIRE(child->flags().isPaintOutdated());

        child->setRectangle(Rectangle{1, 1, 2, 1});
        renderNextFrame(setup);

        requireCallNames(setup, {"root", "child"});
        REQUIRE_FALSE(child->flags().isPaintOutdated());
    }

    void testMovingChildDamagesOldAndNewRegions() {
        auto setup = createSetup(Size{6, 1});
        auto child = PaintProbeSurface::create(setup.calls, "child", Char{U'C'}, true);
        setup.root->addSurface(child);
        child->setRectangle(Rectangle{0, 0, 2, 1});
        renderInitialFrame(setup);

        child->setRectangle(Rectangle{3, 0, 2, 1});
        renderNextFrame(setup);

        requireCallNames(setup, {"root", "child"});
        REQUIRE_EQUAL(setup.calls->at(0).dirtyRect, Rectangle(0, 0, 5, 1));
        REQUIRE_EQUAL(setup.backend->output(), std::string{"RRRCCR\n"});
    }

    void testHiddenChildIsNotPaintedAndKeepsItsRectangle() {
        auto setup = createSetup(Size{6, 1});
        auto child = PaintProbeSurface::create(setup.calls, "child", Char{U'C'}, true);
        setup.root->addSurface(child);
        child->setRectangle(Rectangle{1, 0, 3, 1});
        renderInitialFrame(setup);

        child->flags().setVisible(false);
        renderNextFrame(setup);

        requireCallNames(setup, {"root"});
        REQUIRE_EQUAL(child->rectangle(), Rectangle(1, 0, 3, 1));
        REQUIRE_EQUAL(setup.backend->output(), std::string{"RRRRRR\n"});
    }

    void testOverlappingLaterSiblingRepaintsAboveDirtySurface() {
        auto setup = createSetup(Size{6, 1});
        auto first = PaintProbeSurface::create(setup.calls, "first", Char{U'A'}, true);
        auto second = PaintProbeSurface::create(setup.calls, "second", Char{U'B'}, true);
        setup.root->addSurface(first);
        setup.root->addSurface(second);
        first->setRectangle(Rectangle{0, 0, 4, 1});
        second->setRectangle(Rectangle{2, 0, 4, 1});
        renderInitialFrame(setup);

        first->flags().setPaintOutdated();
        renderNextFrame(setup);

        requireCallNames(setup, {"first", "second"});
        REQUIRE_EQUAL(setup.backend->output(), std::string{"AABBBB\n"});
    }

    void testPrePaintDirtyClearPreservesInvalidationDuringPaint() {
        auto setup = createSetup(Size{4, 1});
        auto child = PaintProbeSurface::create(setup.calls, "child", Char{U'C'}, true);
        setup.root->addSurface(child);
        child->setRectangle(Rectangle{0, 0, 2, 1});
        renderInitialFrame(setup);

        child->invalidateDuringNextPaint();
        child->flags().setPaintOutdated();
        renderNextFrame(setup);

        requireCallNames(setup, {"child"});
        REQUIRE(child->flags().isPaintOutdated());
        setup.calls->clear();

        renderNextFrame(setup);

        requireCallNames(setup, {"child"});
        REQUIRE_FALSE(child->flags().isPaintOutdated());
    }

    void testDirtyDynamicTextLineRepaintsUpdatedChildText() {
        auto setup = createSetup(Size{12, 2});
        auto textLine = ui::DynamicTextLine::create();
        textLine->setText(ui::DynamicTextLine::Section::Left, "line 1");
        setup.root->addSurface(textLine);
        textLine->setRectangle(Rectangle{0, 1, 12, 1});
        renderInitialFrame(setup);

        textLine->setText(ui::DynamicTextLine::Section::Left, "line 2");
        textLine->flags().setPaintOutdated();
        renderNextFrame(setup);

        REQUIRE_NOT_EQUAL(setup.backend->output().find("line 2"), std::string::npos);
    }

private:
    struct Setup final {
        std::shared_ptr<UiSchedulerManualClock> clock;
        ui::impl::ScopedEventClockOverride clockOverride;
        std::shared_ptr<TerminalTestBackend> backend;
        std::shared_ptr<Terminal> terminal;
        ui::PagePtr page;
        std::shared_ptr<PaintProbeSurface> root;
        std::shared_ptr<std::vector<PaintCall>> calls;
        ui::Display display;
    };

    [[nodiscard]] auto createSetup(Size size) -> Setup {
        auto calls = std::make_shared<std::vector<PaintCall>>();
        calls->reserve(32);
        auto clock = std::make_shared<UiSchedulerManualClock>();
        auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, size);
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto page = ui::Page::create();
        auto root = PaintProbeSurface::create(calls, "root", Char{U'R'}, true);
        root->editLayoutMetrics().setFixedSize(size);
        page->addSurface(root);
        return Setup{
            .clock = clock,
            .clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }},
            .backend = backend,
            .terminal = terminal,
            .page = page,
            .root = root,
            .calls = calls,
            .display = ui::Display{terminal, page, exactTerminalSizeLimits()},
        };
    }

    static void renderInitialFrame(Setup &setup) {
        setup.display.pollRender();
        setup.calls->clear();
        setup.backend->clearOutput();
    }

    static void renderNextFrame(Setup &setup) {
        setup.clock->advance(std::chrono::milliseconds{100});
        setup.display.pollRender();
    }

    void requireCallNames(const Setup &setup, const std::vector<std::string> &expectedNames) {
        REQUIRE_EQUAL(setup.calls->size(), expectedNames.size());
        for (auto index = std::size_t{0}; index < expectedNames.size(); ++index) {
            REQUIRE_EQUAL(setup.calls->at(index).name, expectedNames[index]);
        }
    }
};
