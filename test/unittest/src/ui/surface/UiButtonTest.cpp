// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/BufferTestHelper.hpp"

#include <erbsland/cterm/theme/State.hpp>
#include <erbsland/cterm/theme/ThemeBuilder.hpp>
#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <vector>

TESTED_TARGETS(UiButton UiAction UiFocus)
class UiButtonTest final : public UNITTEST_SUBCLASS(BufferTestHelper) {
public:
    void testButtonRendersActionNameAndMainKey() {
        auto action = ui::ButtonAction::create("Yes");
        action->setKeys(U'y');
        auto button = ui::Button::create(action);
        const auto context = button->themeContextFrom(zeroThemeContext());
        auto scope = ui::MeasureScope{{}, context};

        const auto metrics = button->onMeasure(scope, ui::LayoutProposal::unconstrained());
        auto buffer = Buffer{metrics.preferred(), Char{U'.'}};
        button->setRectangle(buffer.rect());
        button->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), context});

        REQUIRE_EQUAL(metrics.preferred(), Size(8, 1));
        REQUIRE_EQUAL(metrics.margins(), Margins{});
        REQUIRE(button->flags().isFocusable());
        requireRowsEqual(buffer, {" Yes y  "});
    }

    void testButtonTriggersActionWithEnterSpaceAndActionKey() {
        auto action = ui::ButtonAction::create("Run");
        action->setKeys(U'r');
        auto button = ui::Button::create(action);
        auto calls = std::vector<Key>{};
        action->setTriggerFn([&](const ui::ActionTriggerContext &context) { calls.push_back(context.key()); });

        auto enterEvent = ui::KeyPressEvent{Key{Key::Enter}};
        button->onKeyPress(enterEvent);
        auto spaceEvent = ui::KeyPressEvent{Key{Key::Space}};
        button->onKeyPress(spaceEvent);
        auto keyEvent = ui::KeyPressEvent{Key{Key::Character, U'r'}};
        button->onKeyPress(keyEvent);

        REQUIRE(enterEvent.isHandled());
        REQUIRE(spaceEvent.isHandled());
        REQUIRE(keyEvent.isHandled());
        REQUIRE_EQUAL(calls, (std::vector<Key>{Key{Key::Enter}, Key{Key::Space}, Key{Key::Character, U'r'}}));
    }

    void testButtonUsesThemeTextPaddingForMeasurementAndRendering() {
        auto builder = theme::ThemeBuilder::zero();
        builder.edit(theme::Selector{theme::Element::Button, theme::Part::Border})
            .setBlock(theme::BlockRole::LeftBracket, U'▌')
            .setBlock(theme::BlockRole::RightBracket, U'▐')
            .setMargins(Margins{1})
            .setPadding(Margins{2, 0});
        builder.edit(theme::Selector{theme::Element::Button, theme::Part::Text}).setPadding(Margins{1, 0});
        const auto activeTheme = builder.build();
        auto action = ui::ButtonAction::create("Go");
        auto button = ui::Button::create(action);
        const auto context = button->themeContextFrom(ui::ThemeContext{activeTheme});
        auto scope = ui::MeasureScope{{}, context};

        const auto metrics = button->onMeasure(scope, ui::LayoutProposal::unconstrained());
        REQUIRE_EQUAL(metrics.preferred(), (Size{10, 1}));

        auto buffer = Buffer{metrics.preferred(), Char{U'.'}};
        button->setRectangle(buffer.rect());
        button->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), context});

        requireRowsEqual(buffer, {"▌   Go   ▐"});
    }

    void testDisabledButtonUsesDisabledStateAndDoesNotTrigger() {
        auto action = ui::ButtonAction::create("No");
        action->setKeys(U'n');
        action->setEnabled(false);
        auto button = ui::Button::create(action);
        auto calls = std::size_t{0};
        action->setTriggerFn([&]() { calls += 1; });

        auto keyEvent = ui::KeyPressEvent{Key{Key::Character, U'n'}};
        button->onKeyPress(keyEvent);

        REQUIRE_FALSE(button->isEnabled());
        REQUIRE(button->flags().themeStates().contains(theme::State::Disabled));
        REQUIRE_FALSE(keyEvent.isHandled());
        REQUIRE_EQUAL(calls, std::size_t{0});
    }

    void testButtonActionSynchronizesEnabledStateWithButton() {
        auto action = ui::ButtonAction::create("Apply");
        auto button = ui::Button::create(action);

        REQUIRE(button->flags().isEnabled());

        action->setEnabled(false);

        REQUIRE_FALSE(button->flags().isEnabled());
        REQUIRE(button->flags().themeStates().contains(theme::State::Disabled));

        action->setEnabled(true);

        REQUIRE(button->flags().isEnabled());
        REQUIRE_FALSE(button->flags().themeStates().contains(theme::State::Disabled));
    }

    void testButtonActionRefreshSynchronizesEnabledCallback() {
        auto action = ui::ButtonAction::create("Save");
        auto button = ui::Button::create(action);
        auto enabled = false;
        action->setEnabledFn([&]() { return enabled; }, ui::Action::EnabledUpdateMode::BeforeRepaint);

        action->refreshEnabled(ui::Action::EnabledRefreshReason::BeforeRepaint);

        REQUIRE_FALSE(button->flags().isEnabled());

        enabled = true;
        action->refreshEnabled(ui::Action::EnabledRefreshReason::BeforeRepaint);

        REQUIRE(button->flags().isEnabled());
    }

    void testButtonActionCanOnlyBindToOneLiveButton() {
        auto action = ui::ButtonAction::create("Shared");
        auto button = ui::Button::create(action);
        REQUIRE(button != nullptr);

        REQUIRE_THROWS(ui::Button::create(action));
    }
};
