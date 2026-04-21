// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/BufferTestHelper.hpp"

#include <erbsland/cterm/theme/Theme.hpp>
#include <erbsland/cterm/theme/ThemeBuilder.hpp>
#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <string>
#include <vector>

TESTED_TARGETS(UiButtons UiButton UiFocus)
class UiButtonsTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testButtonsAcceptGenericButtonMutationAndRejectOtherSurfaces() {
        auto buttons = ui::Buttons::create();
        auto button = ui::Button::create(action("Generic", U'g'));

        buttons->surfaces().add(button);

        REQUIRE_EQUAL(buttons->buttonCount(), std::size_t{1});
        REQUIRE_EQUAL(buttons->button(0), button);
        REQUIRE(buttons->actionsPtr() != nullptr);
        REQUIRE(buttons->actionsPtr()->contains(button->action()));
        REQUIRE_THROWS(buttons->surfaces().add(ui::Panel::create()));

        buttons->surfaces().remove(button);

        REQUIRE_EQUAL(buttons->buttonCount(), std::size_t{0});
        REQUIRE_FALSE(buttons->actionsPtr()->contains(button->action()));
    }

    void testButtonsCenterOneRow() {
        auto buttons = ui::Buttons::create();
        const auto yes = buttons->addAction(action("Yes", U'y'));
        const auto no = buttons->addAction(action("No", U'n'));

        buttons->setRectangle(Rectangle{0, 0, 30, 1});
        buttons->layout(Size{30, 1}, ui::LayoutContext{buttons->themeContextFrom(ui::ThemeContext{})});

        REQUIRE_EQUAL(yes->rectangle(), Rectangle(1, 0, 14, 1));
        REQUIRE_EQUAL(no->rectangle(), Rectangle(16, 0, 13, 1));
    }

    void testButtonsWrapToMultipleCenteredRows() {
        auto buttons = ui::Buttons::create();
        const auto yes = buttons->addAction(action("Yes", U'y'));
        const auto no = buttons->addAction(action("No", U'n'));

        buttons->setRectangle(Rectangle{0, 0, 15, 2});
        buttons->layout(Size{15, 2}, ui::LayoutContext{buttons->themeContextFrom(ui::ThemeContext{})});

        REQUIRE_EQUAL(yes->rectangle(), Rectangle(0, 0, 14, 1));
        REQUIRE_EQUAL(no->rectangle(), Rectangle(1, 1, 13, 1));
    }

    void testButtonsUseThemeSpacing() {
        auto builder = theme::ThemeBuilder::from(theme::Theme::dark());
        builder.edit(theme::Selector{theme::Element::Buttons, theme::Part::Spacing}).setMargins(Margins{3, 1});
        const auto activeTheme = builder.build();
        auto buttons = ui::Buttons::create();
        auto context = buttons->themeContextFrom(ui::ThemeContext{activeTheme});
        const auto yes = buttons->addAction(action("Yes", U'y'));
        const auto no = buttons->addAction(action("No", U'n'));

        buttons->setRectangle(Rectangle{0, 0, 20, 3});
        buttons->layout(Size{20, 3}, ui::LayoutContext{context});

        REQUIRE_EQUAL(yes->rectangle(), Rectangle(3, 0, 14, 1));
        REQUIRE_EQUAL(no->rectangle(), Rectangle(3, 2, 13, 1));
    }

    void testButtonsMinimumHeightIncludesAllWrappedRows() {
        auto buttons = ui::Buttons::create();
        buttons->addAction(action("Yes", U'y'));
        buttons->addAction(action("No", U'n'));
        auto scope = ui::MeasureScope{};

        const auto metrics = buttons->onMeasure(scope, ui::LayoutProposal::atMost(Size{15, 10}));

        REQUIRE_EQUAL(metrics.minimum().height(), Coordinate{2});
        REQUIRE_EQUAL(metrics.preferred().height(), Coordinate{2});
    }

    void testButtonsMoveFocusAndSkipDisabledButtons() {
        auto page = ui::Page::create();
        auto buttons = ui::Buttons::create();
        const auto first = buttons->addAction(action("First", U'f'));
        const auto disabled = buttons->addAction(action("Second", U's'));
        const auto third = buttons->addAction(action("Third", U't'));
        disabled->action()->setEnabled(false);
        page->addSurface(buttons);
        buttons->setRectangle(Rectangle{0, 0, 60, 1});
        buttons->layout(Size{60, 1}, ui::LayoutContext{});
        page->focusTo(first);

        auto event = ui::KeyPressEvent{Key{Key::Right}};
        page->onKeyPress(event);

        REQUIRE(event.isHandled());
        REQUIRE_FALSE(first->flags().hasFocus());
        REQUIRE_FALSE(disabled->flags().hasFocus());
        REQUIRE(third->flags().hasFocus());
    }

    void testButtonsDoNotMoveFocusToClippedButtons() {
        auto page = ui::Page::create();
        auto buttons = ui::Buttons::create();
        const auto first = buttons->addAction(action("Yes", U'y'));
        const auto second = buttons->addAction(action("No", U'n'));
        page->addSurface(buttons);
        buttons->setRectangle(Rectangle{0, 0, 15, 1});
        buttons->layout(Size{15, 1}, ui::LayoutContext{});
        page->focusTo(first);

        auto event = ui::KeyPressEvent{Key{Key::Right}};
        page->onKeyPress(event);

        REQUIRE_FALSE(event.isHandled());
        REQUIRE(first->flags().hasFocus());
        REQUIRE_FALSE(second->flags().hasFocus());
    }

    void testButtonsDispatchActionKeysFromTheFocusedLayout() {
        auto page = ui::Page::create();
        auto buttons = ui::Buttons::create();
        const auto firstAction = action("First", U'f');
        const auto secondAction = action("Second", U's');
        auto calls = std::vector<std::string>{};
        secondAction->setTriggerFn([&]() { calls.emplace_back("second"); });
        const auto first = buttons->addAction(firstAction);
        buttons->addAction(secondAction);
        page->addSurface(buttons);
        page->focusTo(first);

        auto event = ui::KeyPressEvent{Key{Key::Character, U's'}};
        page->onKeyPress(event);

        REQUIRE(event.isHandled());
        REQUIRE_EQUAL(calls, (std::vector<std::string>{"second"}));
    }

private:
    [[nodiscard]] static auto action(const std::string &name, const char32_t key) -> ui::ButtonActionPtr {
        auto result = ui::ButtonAction::create(name);
        result->setKeys(key);
        return result;
    }
};
