// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/ColorTermIncludes.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <format>
#include <memory>
#include <string>
#include <vector>

TESTED_TARGETS(UiSurfaceContainer UiSurfaceFlags)
class UiSurfaceContainerTest final : public el::UnitTest {
private:
    class TestLayoutData final : public ui::LayoutData {};

    class RecordingManager final : public ui::SurfaceManager {
    public:
        [[nodiscard]] auto maximumSurfaceCount() const noexcept -> std::size_t override { return 1; }

        [[nodiscard]] auto defaultLayoutData([[maybe_unused]] const ui::SurfacePtr &surface) const
            -> ui::LayoutDataPtr override {
            return std::make_shared<TestLayoutData>();
        }

        void didAdd(
            [[maybe_unused]] const ui::SurfacePtr &surface,
            const std::size_t index,
            [[maybe_unused]] const ui::LayoutDataPtr &data) noexcept override {
            log.emplace_back(std::format("add:{}", index));
        }

        void didRemove(
            [[maybe_unused]] const ui::SurfacePtr &surface,
            const std::size_t index,
            [[maybe_unused]] const ui::LayoutDataPtr &data) noexcept override {
            log.emplace_back(std::format("remove:{}", index));
        }

    public:
        std::vector<std::string> log;
    };

    class ManagedSurface final : public ui::Surface {
    public:
        explicit ManagedSurface(ProtectedTag) noexcept {}

        [[nodiscard]] static auto create() -> std::shared_ptr<ManagedSurface> {
            return std::make_shared<ManagedSurface>(ProtectedTag{});
        }

        void setManager(ui::SurfaceManager &manager) noexcept { childStorage().setManager(manager); }
    };

public:
    void testManagerAddsDefaultLayoutDataAndRejectsMaximumCount() {
        auto surface = ManagedSurface::create();
        auto child = ui::Panel::create();
        auto extra = ui::Panel::create();
        auto manager = RecordingManager{};
        surface->setManager(manager);

        surface->surfaces().add(child);

        REQUIRE(std::dynamic_pointer_cast<TestLayoutData>(surface->surfaces().layoutData(child)) != nullptr);
        REQUIRE_EQUAL(manager.log, (std::vector<std::string>{"add:0"}));
        REQUIRE_THROWS(surface->surfaces().add(extra));

        surface->surfaces().remove(child);

        REQUIRE_EQUAL(manager.log, (std::vector<std::string>{"add:0", "remove:0"}));
    }

    void testAddRejectsNullSurfacesAndCycles() {
        auto page = ui::Page::create();
        auto child = ui::Panel::create();

        REQUIRE_THROWS(page->addSurface(nullptr));
        page->addSurface(child);

        REQUIRE_THROWS(child->addSurface(page));
    }

    void testDuplicateSurfacesInTheSameContainerAreRejected() {
        auto page = ui::Page::create();
        auto child = ui::Panel::create();
        auto otherChild = ui::Panel::create();

        page->addSurface(child);
        page->addSurface(otherChild);

        REQUIRE_THROWS(page->addSurface(child));
        REQUIRE_THROWS(page->surfaces().insert(0, child));
        REQUIRE_THROWS(page->surfaces().replace(0, otherChild));
    }

    void testInsertAndIterationPreserveOrderAndParentLinks() {
        auto page = ui::Page::create();
        auto a = ui::Panel::create();
        auto b = ui::Panel::create();
        auto c = ui::Panel::create();
        auto d = ui::Panel::create();

        page->addSurface(a);
        page->addSurface(c);
        page->surfaces().insertBefore(c, b);
        page->surfaces().insertAfter(c, d);

        REQUIRE_EQUAL(orderedSurfaces(page), (std::vector<ui::SurfacePtr>{a, b, c, d}));
        REQUIRE_EQUAL(a->parent().lock(), page);
        REQUIRE_EQUAL(b->parent().lock(), page);
        REQUIRE_EQUAL(c->parent().lock(), page);
        REQUIRE_EQUAL(d->parent().lock(), page);
    }

    void testRemoveAndRemoveAllUpdateParentsAndInvalidateLayout() {
        auto page = ui::Page::create();
        auto a = ui::Panel::create();
        auto b = ui::Panel::create();
        auto c = ui::Panel::create();
        page->addSurface(a);
        page->addSurface(b);
        page->addSurface(c);
        page->layout(Size{10, 5}, ui::LayoutContext{});

        page->surfaces().remove(std::size_t{1});

        REQUIRE_EQUAL(orderedSurfaces(page), (std::vector<ui::SurfacePtr>{a, c}));
        REQUIRE(b->parent().expired());
        REQUIRE(page->flags().isLayoutOutdated());
        REQUIRE(page->flags().isPaintOutdated());

        page->layout(Size{10, 5}, ui::LayoutContext{});
        page->surfaces().remove(b);

        REQUIRE_FALSE(page->flags().isLayoutOutdated());

        page->surfaces().removeAll();

        REQUIRE(page->surfaces().empty());
        REQUIRE(a->parent().expired());
        REQUIRE(c->parent().expired());
        REQUIRE(page->flags().isLayoutOutdated());
    }

    void testReplaceUpdatesParentsAndReparentsFromOldParent() {
        auto oldParent = ui::Page::create();
        auto newParent = ui::Page::create();
        auto oldSurface = ui::Panel::create();
        auto replacement = ui::Panel::create();
        oldParent->addSurface(replacement);
        newParent->addSurface(oldSurface);
        oldParent->layout(Size{10, 5}, ui::LayoutContext{});
        newParent->layout(Size{10, 5}, ui::LayoutContext{});

        newParent->surfaces().replace(oldSurface, replacement);

        REQUIRE(oldParent->surfaces().empty());
        REQUIRE_EQUAL(orderedSurfaces(newParent), (std::vector<ui::SurfacePtr>{replacement}));
        REQUIRE(oldSurface->parent().expired());
        REQUIRE_EQUAL(replacement->parent().lock(), newParent);
        REQUIRE(oldParent->flags().isLayoutOutdated());
        REQUIRE(newParent->flags().isLayoutOutdated());
    }

    void testMoveOperationsKeepParentLinksAndUpdateOrder() {
        auto page = ui::Page::create();
        auto a = ui::Panel::create();
        auto b = ui::Panel::create();
        auto c = ui::Panel::create();
        auto d = ui::Panel::create();
        page->addSurface(a);
        page->addSurface(b);
        page->addSurface(c);
        page->addSurface(d);
        page->layout(Size{10, 5}, ui::LayoutContext{});

        page->surfaces().move(0, 2);
        REQUIRE_EQUAL(orderedSurfaces(page), (std::vector<ui::SurfacePtr>{b, c, a, d}));

        page->surfaces().moveBefore(d, b);
        REQUIRE_EQUAL(orderedSurfaces(page), (std::vector<ui::SurfacePtr>{d, b, c, a}));

        page->surfaces().moveAfter(d, a);
        REQUIRE_EQUAL(orderedSurfaces(page), (std::vector<ui::SurfacePtr>{b, c, a, d}));
        REQUIRE_EQUAL(d->parent().lock(), page);
        REQUIRE(page->flags().isLayoutOutdated());
    }

    void testInvalidIndicesAndAnchorsThrow() {
        auto page = ui::Page::create();
        auto a = ui::Panel::create();
        auto b = ui::Panel::create();
        auto foreign = ui::Panel::create();
        page->addSurface(a);
        page->addSurface(b);

        REQUIRE_THROWS(page->surfaces().remove(std::size_t{2}));
        REQUIRE_THROWS(page->surfaces().insert(3, ui::Panel::create()));
        REQUIRE_THROWS(page->surfaces().insertBefore(foreign, ui::Panel::create()));
        REQUIRE_THROWS(page->surfaces().insertAfter(foreign, ui::Panel::create()));
        REQUIRE_THROWS(page->surfaces().replace(foreign, ui::Panel::create()));
        REQUIRE_THROWS(page->surfaces().replace(2, ui::Panel::create()));
        REQUIRE_THROWS(page->surfaces().move(0, 2));
        REQUIRE_THROWS(page->surfaces().move(foreign, 0));
        REQUIRE_THROWS(page->surfaces().moveBefore(a, foreign));
        REQUIRE_THROWS(page->surfaces().moveAfter(a, foreign));
    }

private:
    [[nodiscard]] static auto orderedSurfaces(const ui::SurfacePtr &surface) -> std::vector<ui::SurfacePtr> {
        auto result = std::vector<ui::SurfacePtr>{};
        for (const auto &child : surface->surfaces()) {
            result.push_back(child);
        }
        return result;
    }
};
