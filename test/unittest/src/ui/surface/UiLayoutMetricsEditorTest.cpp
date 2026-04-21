// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/ColorTermIncludes.hpp"

#include <erbsland/unittest/UnitTest.hpp>

TESTED_TARGETS(UiLayoutMetricsEditor UiSurface)
class UiLayoutMetricsEditorTest final : public el::UnitTest {
public:
    void testEditorUpdatesMetricsAndInvalidatesLayout() {
        auto surface = ui::Panel::create();
        surface->layout(Size{10, 5}, ui::LayoutContext{});

        surface->editLayoutMetrics()
            .setMinimumSize(Size{2, 1})
            .setMaximumSize(Size{10, 4})
            .setPreferredSize(Size{5, 3})
            .setSizePolicy(ui::SizePolicy{ui::DimensionPolicy::Grow});

        REQUIRE_EQUAL(surface->layoutMetrics().minimum(), Size(2, 1));
        REQUIRE_EQUAL(surface->layoutMetrics().maximum(), Size(10, 4));
        REQUIRE_EQUAL(surface->layoutMetrics().preferred(), Size(5, 3));
        REQUIRE_EQUAL(surface->layoutMetrics().sizePolicy().width().type(), ui::DimensionPolicy::Grow);
        REQUIRE_EQUAL(surface->layoutMetrics().sizePolicy().height().type(), ui::DimensionPolicy::Grow);
        REQUIRE(surface->flags().isLayoutOutdated());
    }

    void testEditorDoesNotInvalidateForUnchangedMetrics() {
        auto surface = ui::Panel::create();
        surface->editLayoutMetrics().setFixedSize(Size{6, 2});
        surface->layout(Size{10, 5}, ui::LayoutContext{});

        surface->editLayoutMetrics().setFixedSize(Size{6, 2});

        REQUIRE_FALSE(surface->flags().isLayoutOutdated());
        REQUIRE_EQUAL(surface->layoutMetrics().minimum(), Size(6, 2));
        REQUIRE_EQUAL(surface->layoutMetrics().maximum(), Size(6, 2));
        REQUIRE_EQUAL(surface->layoutMetrics().preferred(), Size(6, 2));
    }
};
