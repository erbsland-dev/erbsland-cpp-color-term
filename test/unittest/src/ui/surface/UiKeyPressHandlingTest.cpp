// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "RecordingSurface.hpp"
#include "UiSchedulerManualClock.hpp"

#include "support/TerminalTestHelper.hpp"

#include <erbsland/cterm/theme/ThemeBuilder.hpp>
#include <erbsland/cterm/ui/event/impl/EventClockAccess.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <chrono>
#include <format>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class FocusProbeSurface final : public ui::Surface {
public:
    explicit FocusProbeSurface(std::shared_ptr<std::vector<std::string>> callLog, std::string name, ProtectedTag) :
        _callLog{std::move(callLog)}, _name{std::move(name)} {
        flags().setFocusable(true);
    }

    [[nodiscard]] static auto create(const std::shared_ptr<std::vector<std::string>> &callLog, std::string name)
        -> std::shared_ptr<FocusProbeSurface> {
        return std::make_shared<FocusProbeSurface>(callLog, std::move(name), ProtectedTag{});
    }

public: // implement Surface
    void onFocus(const ui::FocusChange focusChange) noexcept override {
        switch (focusChange) {
        case ui::FocusChange::In:
            _callLog->push_back(_name + ":in");
            break;
        case ui::FocusChange::InWithin:
            _callLog->push_back(_name + ":within");
            break;
        case ui::FocusChange::Out:
            _callLog->push_back(_name + ":out");
            break;
        }
    }

private:
    std::shared_ptr<std::vector<std::string>> _callLog;
    std::string _name;
};

TESTED_TARGETS(UiAction UiActions UiActionHelp UiFooterLine UiFocus UiKeyPress UiApplication)
class UiKeyPressHandlingTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testSurfaceActionsHandleMatchingKeysOnly() {
        auto panel = ui::Panel::create();
        auto actionCalls = std::size_t{0};
        auto action = ui::Action::create("toggle");
        action->setKeys(U'x');
        action->setTriggerFn([&](const ui::ActionTriggerContext &context) -> void {
            actionCalls += 1;
            REQUIRE(context.key() == Key{Key::Character, U'x'});
            REQUIRE(context.sourceSurface().lock() == panel);
        });
        panel->actions().add(action);

        auto unmatchedEvent = ui::KeyPressEvent{Key{Key::Character, U'y'}};
        panel->onKeyPress(unmatchedEvent);
        REQUIRE_EQUAL(actionCalls, std::size_t{0});
        REQUIRE_FALSE(unmatchedEvent.isHandled());

        auto matchedEvent = ui::KeyPressEvent{Key{Key::Character, U'x'}};
        panel->onKeyPress(matchedEvent);
        REQUIRE_EQUAL(actionCalls, std::size_t{1});
        REQUIRE(matchedEvent.isHandled());
    }

    void testActionsSupportKeyConvenienceAndCombinedKeys() {
        auto panel = ui::Panel::create();
        auto actionCalls = std::size_t{0};
        auto action = ui::Action::create("run");
        action->setKeys(
            std::vector<Key>{
                Key{Key::Enter},
                Key{Key::Character, U'ü'},
                Key{Key::Combined, U"ab"},
                Key{Key::Escape},
                Key{Key::Tab},
                Key{Key::Left},
                Key{Key::Right},
                Key{Key::Character, U'λ'},
                Key{Key::Character, U'ß'}});
        action->setTriggerFn([&actionCalls]() -> void { actionCalls += 1; });
        panel->actions().add(action);

        for (const auto &key :
             {Key{Key::Enter},
              Key{Key::Character, U'ü'},
              Key{Key::Combined, U"ab"},
              Key{Key::Escape},
              Key{Key::Tab},
              Key{Key::Left},
              Key{Key::Right},
              Key{Key::Character, U'λ'},
              Key{Key::Character, U'ß'}}) {
            auto keyPressEvent = ui::KeyPressEvent{key};
            panel->onKeyPress(keyPressEvent);
            REQUIRE(keyPressEvent.isHandled());
        }
        REQUIRE_EQUAL(actionCalls, std::size_t{9});

        action->setKeys({Key::PageUp, Key::PageDown});
        REQUIRE_EQUAL(renderKeyLabels(action->keys().mainKeys()), (std::vector<std::string>{"pgup", "pgdn"}));

        action->setKeys({U'a', U'b'});
        REQUIRE_EQUAL(renderKeyLabels(action->keys().mainKeys()), (std::vector<std::string>{"a", "b"}));
    }

    void testActionsRejectNullAndDuplicateAttachments() {
        auto panel = ui::Panel::create();
        auto otherPanel = ui::Panel::create();
        auto action = ui::Action::create("shared");

        REQUIRE_THROWS_AS(std::invalid_argument, panel->actions().add(ui::ActionPtr{}));
        panel->actions().add(action);
        REQUIRE_THROWS_AS(std::invalid_argument, panel->actions().add(action));

        otherPanel->actions().add(action);
        REQUIRE(otherPanel->actions().contains(action));
        REQUIRE_EQUAL(action.use_count(), 3);
    }

    void testSpecialKeyTypeOverloadsRejectUnsupportedKeyKinds() {
        auto action = ui::Action::create("invalid");

        REQUIRE_THROWS_AS(std::invalid_argument, action->setKeys({Key::None}));
        REQUIRE_THROWS_AS(std::invalid_argument, action->setKeys({Key::Character}));
        REQUIRE_THROWS_AS(std::invalid_argument, action->setKeys({Key::Combined}));
    }

    void testDuplicateKeysTriggerFirstEnabledActionAndFallThroughDisabledActions() {
        auto panel = ui::Panel::create();
        auto calls = std::vector<std::string>{};
        auto firstAction = createLoggingAction("first", Key{Key::Enter}, calls);
        auto secondAction = createLoggingAction("second", Key{Key::Enter}, calls);
        panel->actions().add(firstAction);
        panel->actions().add(secondAction);

        auto firstEvent = ui::KeyPressEvent{Key{Key::Enter}};
        panel->onKeyPress(firstEvent);
        REQUIRE(firstEvent.isHandled());
        REQUIRE_EQUAL(calls, (std::vector<std::string>{"first"}));

        firstAction->setEnabled(false);
        auto secondEvent = ui::KeyPressEvent{Key{Key::Enter}};
        panel->onKeyPress(secondEvent);
        REQUIRE(secondEvent.isHandled());
        REQUIRE_EQUAL(calls, (std::vector<std::string>{"first", "second"}));
    }

    void testFocusStateAndCallbacksFollowFocusedPath() {
        auto page = ui::Page::create();
        auto calls = std::make_shared<std::vector<std::string>>();
        auto parent = FocusProbeSurface::create(calls, "parent");
        auto child = FocusProbeSurface::create(calls, "child");
        parent->addSurface(child);
        page->addSurface(parent);

        REQUIRE(page->focusTo(child));

        REQUIRE_FALSE(parent->flags().hasFocus());
        REQUIRE(parent->flags().hasFocusWithin());
        REQUIRE(child->flags().hasFocus());
        REQUIRE(child->flags().hasFocusWithin());
        REQUIRE(child->flags().themeStates().contains(theme::State::Focused));
        REQUIRE(child->flags().themeStates().contains(theme::State::FocusWithin));
        REQUIRE(parent->flags().themeStates().contains(theme::State::FocusWithin));
        REQUIRE_EQUAL(*calls, (std::vector<std::string>{"parent:within", "child:in"}));

        REQUIRE(page->focusTo(child));
        REQUIRE_EQUAL(*calls, (std::vector<std::string>{"parent:within", "child:in"}));

        page->clearFocus();

        REQUIRE_FALSE(parent->flags().hasFocusWithin());
        REQUIRE_FALSE(child->flags().hasFocus());
        REQUIRE_EQUAL(*calls, (std::vector<std::string>{"parent:within", "child:in", "child:out", "parent:out"}));
    }

    void testFocusMoveEmitsLossesLeafToRootAndGainsRootToLeaf() {
        auto page = ui::Page::create();
        auto calls = std::make_shared<std::vector<std::string>>();
        auto parent = FocusProbeSurface::create(calls, "parent");
        auto first = FocusProbeSurface::create(calls, "first");
        auto second = FocusProbeSurface::create(calls, "second");
        parent->addSurface(first);
        parent->addSurface(second);
        page->addSurface(parent);

        page->focusTo(first);
        calls->clear();
        page->focusTo(second);

        REQUIRE_EQUAL(*calls, (std::vector<std::string>{"first:out", "second:in"}));
        REQUIRE(parent->flags().hasFocusWithin());
        REQUIRE_FALSE(first->flags().hasFocus());
        REQUIRE(second->flags().hasFocus());
    }

    void testFocusRejectsNonFocusableSurfaces() {
        auto page = ui::Page::create();
        auto panel = ui::Panel::create();
        page->addSurface(panel);

        REQUIRE_FALSE(page->focusTo(panel));

        REQUIRE_FALSE(page->hasFocusSurface());
        REQUIRE_FALSE(panel->flags().hasFocus());
    }

    void testFocusChainDispatchesFocusedChildBeforeParentAndPage() {
        auto page = ui::Page::create();
        auto callLog = std::make_shared<std::vector<std::string>>();
        auto parent = RecordingSurface::create(callLog, "parent");
        auto child = RecordingSurface::create(callLog, "child");
        parent->addSurface(child);
        page->addSurface(parent);
        page->focusTo(child);
        child->actions().add(createLoggingAction("child-action", Key{Key::Character, U'k'}, *callLog));
        parent->actions().add(createLoggingAction("parent-action", Key{Key::Character, U'k'}, *callLog));
        page->actions().add(createLoggingAction("page-action", Key{Key::Character, U'k'}, *callLog));

        auto keyPressEvent = ui::KeyPressEvent{Key{Key::Character, U'k'}};
        page->onKeyPress(keyPressEvent);

        REQUIRE(keyPressEvent.isHandled());
        REQUIRE_EQUAL(*callLog, (std::vector<std::string>{"child", "child-action"}));
    }

    void testDisabledActionsFallThroughTheFocusChain() {
        auto page = ui::Page::create();
        auto callLog = std::make_shared<std::vector<std::string>>();
        auto parent = RecordingSurface::create(callLog, "parent");
        auto child = RecordingSurface::create(callLog, "child");
        parent->addSurface(child);
        page->addSurface(parent);
        page->focusTo(child);
        auto childAction = createLoggingAction("child-action", Key{Key::Character, U'k'}, *callLog);
        auto parentAction = createLoggingAction("parent-action", Key{Key::Character, U'k'}, *callLog);
        childAction->setEnabled(false);
        parentAction->setEnabled(false);
        child->actions().add(childAction);
        parent->actions().add(parentAction);
        page->actions().add(createLoggingAction("page-action", Key{Key::Character, U'k'}, *callLog));

        auto keyPressEvent = ui::KeyPressEvent{Key{Key::Character, U'k'}};
        page->onKeyPress(keyPressEvent);

        REQUIRE(keyPressEvent.isHandled());
        REQUIRE_EQUAL(*callLog, (std::vector<std::string>{"child", "parent", "page-action"}));
    }

    void testSharedActionReuseKeepsSourceSurfaceContext() {
        auto page = ui::Page::create();
        auto child = ui::Panel::create();
        auto calls = std::vector<std::string>{};
        auto sharedAction = ui::Action::create("shared");
        sharedAction->setKeys(U's');
        sharedAction->setTriggerFn([&](const ui::ActionTriggerContext &context) -> void {
            calls.push_back(context.sourceSurface().lock() == child ? "child" : "page");
        });
        child->flags().setFocusable(true);
        child->actions().add(sharedAction);
        page->actions().add(sharedAction);
        page->addSurface(child);
        page->focusTo(child);

        auto keyPressEvent = ui::KeyPressEvent{Key{Key::Character, U's'}};
        page->onKeyPress(keyPressEvent);

        REQUIRE(keyPressEvent.isHandled());
        REQUIRE_EQUAL(calls, (std::vector<std::string>{"child"}));
    }

    void testFocusedCaptureSurfacePreventsPageActions() {
        auto page = ui::Page::create();
        auto callLog = std::make_shared<std::vector<std::string>>();
        auto inputSurface = RecordingSurface::create(callLog, "input", true);
        page->addSurface(inputSurface);
        page->focusTo(inputSurface);
        page->actions().add(createLoggingAction("quit", Key{Key::Character, U'q'}, *callLog));

        auto keyPressEvent = ui::KeyPressEvent{Key{Key::Character, U'q'}};
        page->onKeyPress(keyPressEvent);

        REQUIRE(keyPressEvent.isHandled());
        REQUIRE_EQUAL(*callLog, (std::vector<std::string>{"input"}));
    }

    void testFocusToRejectsForeignSurfaces() {
        auto page = ui::Page::create();
        auto foreignSurface = RecordingSurface::create(std::make_shared<std::vector<std::string>>(), "foreign");

        page->focusTo(foreignSurface);

        REQUIRE_FALSE(page->hasFocusSurface());
    }

    void testHiddenFocusSurfaceDoesNotReceiveKeyPresses() {
        auto page = ui::Page::create();
        auto callLog = std::make_shared<std::vector<std::string>>();
        auto child = RecordingSurface::create(callLog, "child");
        page->addSurface(child);
        page->focusTo(child);
        page->actions().add(createLoggingAction("page", Key{Key::Character, U'k'}, *callLog));

        child->flags().setVisible(false);
        auto keyPressEvent = ui::KeyPressEvent{Key{Key::Character, U'k'}};
        page->onKeyPress(keyPressEvent);

        REQUIRE(keyPressEvent.isHandled());
        REQUIRE_FALSE(page->hasFocusSurface());
        REQUIRE_EQUAL(*callLog, (std::vector<std::string>{"page"}));
    }

    void testForeignFocusFallsBackToThePageAndClearsTheStoredFocus() {
        auto page = ui::Page::create();
        auto callLog = std::make_shared<std::vector<std::string>>();
        auto child = RecordingSurface::create(callLog, "child");
        page->addSurface(child);
        page->focusTo(child);
        page->surfaces().remove(child);
        page->actions().add(createLoggingAction("page", Key{Key::Character, U'f'}, *callLog));

        auto keyPressEvent = ui::KeyPressEvent{Key{Key::Character, U'f'}};
        page->onKeyPress(keyPressEvent);

        REQUIRE(keyPressEvent.isHandled());
        REQUIRE_EQUAL(*callLog, (std::vector<std::string>{"page"}));
        REQUIRE_FALSE(page->hasFocusSurface());
    }

    void testReparentedFocusFallsBackToThePageAndClearsTheStoredFocus() {
        auto oldPage = ui::Page::create();
        auto newPage = ui::Page::create();
        auto callLog = std::make_shared<std::vector<std::string>>();
        auto child = RecordingSurface::create(callLog, "child");
        oldPage->addSurface(child);
        oldPage->focusTo(child);
        newPage->addSurface(child);
        oldPage->actions().add(createLoggingAction("old-page", Key{Key::Character, U'r'}, *callLog));

        auto keyPressEvent = ui::KeyPressEvent{Key{Key::Character, U'r'}};
        oldPage->onKeyPress(keyPressEvent);

        REQUIRE(keyPressEvent.isHandled());
        REQUIRE_EQUAL(*callLog, (std::vector<std::string>{"old-page"}));
        REQUIRE_FALSE(oldPage->hasFocusSurface());
        REQUIRE(oldPage->surfaces().empty());
        REQUIRE_EQUAL(child->parent().lock(), newPage);
    }

    void testExpiredFocusFallsBackToThePageAndClearsTheStoredFocus() {
        auto page = ui::Page::create();
        auto callLog = std::make_shared<std::vector<std::string>>();
        auto child = RecordingSurface::create(callLog, "child");
        page->addSurface(child);
        page->focusTo(child);
        page->surfaces().remove(child);
        child.reset();
        page->actions().add(createLoggingAction("page", Key{Key::Character, U'e'}, *callLog));

        auto keyPressEvent = ui::KeyPressEvent{Key{Key::Character, U'e'}};
        page->onKeyPress(keyPressEvent);

        REQUIRE(keyPressEvent.isHandled());
        REQUIRE_EQUAL(*callLog, (std::vector<std::string>{"page"}));
        REQUIRE_FALSE(page->hasFocusSurface());
    }

    void testManualTriggerHonorsEnabledStateAndCallbackContext() {
        auto panel = ui::Panel::create();
        auto action = ui::Action::create("manual");
        auto calls = std::size_t{0};
        action->setTriggerFn([&](const ui::ActionTriggerContext &context) -> void {
            calls += 1;
            REQUIRE(context.key() == Key{Key::Character, U'm'});
            REQUIRE(context.sourceSurface().lock() == panel);
        });

        action->setEnabled(false);
        REQUIRE_FALSE(action->trigger(ui::ActionTriggerContext{Key{Key::Character, U'm'}, panel}));
        REQUIRE_EQUAL(calls, std::size_t{0});

        action->setEnabled(true);
        REQUIRE(action->trigger(ui::ActionTriggerContext{Key{Key::Character, U'm'}, panel}));
        REQUIRE_EQUAL(calls, std::size_t{1});
    }

    void testMultiKeyActionBranchesOnTriggerKey() {
        auto panel = ui::Panel::create();
        auto directions = std::vector<Direction>{};
        auto action = ui::Action::create("pan");
        action->setKeys({Key::Left, Key::Up, Key::Down, Key::Right});
        action->setTriggerFn([&](const ui::ActionTriggerContext &context) -> void {
            switch (context.key().type()) {
            case Key::Left:
                directions.push_back(Direction::West);
                break;
            case Key::Up:
                directions.push_back(Direction::North);
                break;
            case Key::Down:
                directions.push_back(Direction::South);
                break;
            case Key::Right:
                directions.push_back(Direction::East);
                break;
            default:
                break;
            }
        });
        panel->actions().add(action);

        for (const auto keyType : {Key::Left, Key::Up, Key::Down, Key::Right}) {
            auto keyPressEvent = ui::KeyPressEvent{Key{keyType}};
            panel->onKeyPress(keyPressEvent);
            REQUIRE(keyPressEvent.isHandled());
        }

        REQUIRE_EQUAL(
            directions, (std::vector<Direction>{Direction::West, Direction::North, Direction::South, Direction::East}));
        REQUIRE_EQUAL(renderKeyLabels(action->keys().mainKeys()), (std::vector<std::string>{"←", "↑", "↓", "→"}));
    }

    void testBeforeTriggeredEnablementIsRefreshedBeforeDispatch() {
        auto panel = ui::Panel::create();
        auto enabled = false;
        auto callbackCalls = std::size_t{0};
        auto actionCalls = std::size_t{0};
        auto action = ui::Action::create("save");
        action->setKeys(U's');
        action->setEnabledFn(
            [&]() -> bool {
                callbackCalls += 1;
                return enabled;
            },
            ui::Action::EnabledUpdateMode::BeforeTriggered);
        action->setTriggerFn([&actionCalls]() -> void { actionCalls += 1; });
        panel->actions().add(action);

        auto disabledEvent = ui::KeyPressEvent{Key{Key::Character, U's'}};
        panel->onKeyPress(disabledEvent);
        REQUIRE_FALSE(disabledEvent.isHandled());
        REQUIRE_EQUAL(callbackCalls, std::size_t{1});
        REQUIRE_EQUAL(actionCalls, std::size_t{0});

        enabled = true;
        auto enabledEvent = ui::KeyPressEvent{Key{Key::Character, U's'}};
        panel->onKeyPress(enabledEvent);
        REQUIRE(enabledEvent.isHandled());
        REQUIRE_EQUAL(callbackCalls, std::size_t{2});
        REQUIRE_EQUAL(actionCalls, std::size_t{1});
    }

    void testBeforeRepaintEnablementRefreshesDuringActionCollection() {
        auto panel = ui::Panel::create();
        auto callbackCalls = std::size_t{0};
        auto action = ui::Action::create("refresh");
        action->setEnabledFn(
            [&]() -> bool {
                callbackCalls += 1;
                return true;
            },
            ui::Action::EnabledUpdateMode::BeforeRepaint);
        panel->actions().add(action);

        panel->actions().refreshForRepaint();

        REQUIRE_EQUAL(callbackCalls, std::size_t{1});
    }

    void testPolledEnablementRefreshesAtMostEveryHundredMilliseconds() {
        auto clock = std::make_shared<UiSchedulerManualClock>();
        auto clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }};
        auto panel = ui::Panel::create();
        auto callbackCalls = std::size_t{0};
        auto action = ui::Action::create("autosave");
        action->setEnabledFn(
            [&]() -> bool {
                callbackCalls += 1;
                return true;
            },
            ui::Action::EnabledUpdateMode::Polled100ms);
        panel->actions().add(action);

        panel->actions().refreshForRepaint();
        panel->actions().refreshForRepaint();
        clock->advance(std::chrono::milliseconds{99});
        panel->actions().refreshForRepaint();
        clock->advance(std::chrono::milliseconds{1});
        panel->actions().refreshForRepaint();

        REQUIRE_EQUAL(callbackCalls, std::size_t{2});
    }

    void testActionHelpCollectsFocusChainActionsAndDeduplicatesSharedActions() {
        auto page = ui::Page::create();
        auto child = ui::Panel::create();
        auto help = ui::ActionHelp::create();
        auto sharedAction = ui::Action::create("shared");
        auto childAction = ui::Action::create("child");
        auto pageAction = ui::Action::create("quit");
        sharedAction->setKeys(U's');
        childAction->setKeys(U'c');
        pageAction->setKeys(U'q');
        child->flags().setFocusable(true);
        child->actions().add(sharedAction);
        child->actions().add(childAction);
        page->actions().add(sharedAction);
        page->actions().add(pageAction);
        page->addSurface(child);
        page->addSurface(help);
        page->focusTo(child);

        const auto actions = help->collectActions();

        REQUIRE_EQUAL(actions.size(), std::size_t{3});
        REQUIRE_EQUAL(actions[0], sharedAction);
        REQUIRE_EQUAL(actions[1], childAction);
        REQUIRE_EQUAL(actions[2], pageAction);
    }

    void testActionHelpRendersMultipleKeysInOneCompactGroup() {
        auto page = ui::Page::create();
        auto help = ui::ActionHelp::create();
        auto action = ui::Action::create("open");
        action->setKeys({U'o', U'O', Key::Enter});
        page->actions().add(action);
        page->addSurface(help);
        const auto context = help->themeContextFrom(ui::ThemeContext{});

        const auto text = help->renderHelpText(
            ui::PaintContext{Rectangle{0, 0, 20, 1}, Rectangle{0, 0, 20, 1}, Rectangle{0, 0, 20, 1}, context}, 20);

        REQUIRE_EQUAL(render(text), "[o/O/↵] open");
    }

    void testActionHelpUsesCollapsedThemeMarginsBetweenParts() {
        auto builder = theme::ThemeBuilder::zero();
        builder.edit(theme::Selector{theme::Element::ActionHelp, theme::Part::ActionName})
            .setMargins(Margins{0, 3, 0, 2});
        builder.edit(theme::Selector{theme::Element::ActionHelp, theme::Part::KeyBracket})
            .setBlocks(U"    /    [ ]    ")
            .setPadding(Margins{0, 1, 0, 1});
        const auto activeTheme = builder.build();
        auto page = ui::Page::create();
        auto help = ui::ActionHelp::create();
        auto openAction = ui::Action::create("open");
        auto saveAction = ui::Action::create("save");
        openAction->setKeys(U'o');
        saveAction->setKeys(U's');
        page->actions().add(openAction);
        page->actions().add(saveAction);
        page->addSurface(help);
        const auto context = help->themeContextFrom(ui::ThemeContext{activeTheme});

        const auto text = help->renderHelpText(
            ui::PaintContext{Rectangle{0, 0, 40, 1}, Rectangle{0, 0, 40, 1}, Rectangle{0, 0, 40, 1}, context}, 40);

        REQUIRE_EQUAL(render(text), "[ o ]  open   [ s ]  save");
    }

    void testActionHelpRendersOnlyMainKeysInFooter() {
        auto page = ui::Page::create();
        auto help = ui::ActionHelp::create();
        auto keys = Keys{U'+', U']'};
        keys.setMainKeyCount(1);
        auto action = ui::Action::create("zoom in");
        action->setKeys(keys);
        page->actions().add(action);
        page->addSurface(help);
        const auto context = help->themeContextFrom(ui::ThemeContext{});

        const auto text = help->renderHelpText(
            ui::PaintContext{Rectangle{0, 0, 20, 1}, Rectangle{0, 0, 20, 1}, Rectangle{0, 0, 20, 1}, context}, 20);

        REQUIRE_EQUAL(render(text), "[+] zoom in");
    }

    void testActionHelpSkipsActionsHiddenFromFooter() {
        auto page = ui::Page::create();
        auto help = ui::ActionHelp::create();
        auto visibleAction = ui::Action::create("open");
        auto hiddenAction = ui::Action::create("hidden");
        auto helpPageAction = ui::Action::create("details");
        visibleAction->setKeys(U'o');
        hiddenAction->setKeys(U'h');
        helpPageAction->setKeys(U'd');
        hiddenAction->help().setVisibility(ui::HelpVisibility::Hidden);
        helpPageAction->help().setVisibility(ui::HelpVisibility::HelpPage);
        page->actions().add(hiddenAction);
        page->actions().add(helpPageAction);
        page->actions().add(visibleAction);
        page->addSurface(help);

        const auto actions = help->collectActions();
        REQUIRE_EQUAL(actions.size(), std::size_t{1});
        REQUIRE_EQUAL(actions[0], visibleAction);
        REQUIRE_FALSE(hiddenAction->help().isVisibleInFooter());
        REQUIRE(helpPageAction->help().isVisibleOnHelpPage());
    }

    void testActionHelpRendersPriorityAndOmittedMarker() {
        auto page = ui::Page::create();
        auto help = ui::ActionHelp::create();
        auto highPriority = ui::Action::create("high");
        auto lowPriority = ui::Action::create("low");
        highPriority->setKeys(U'h');
        highPriority->help().setPriority(100);
        lowPriority->setKeys(U'l');
        lowPriority->help().setPriority(-100);
        page->actions().add(lowPriority);
        page->actions().add(highPriority);
        page->addSurface(help);
        const auto context = help->themeContextFrom(ui::ThemeContext{});

        const auto text = help->renderHelpText(
            ui::PaintContext{Rectangle{0, 0, 11, 1}, Rectangle{0, 0, 11, 1}, Rectangle{0, 0, 11, 1}, context}, 11);

        REQUIRE_EQUAL(render(text), "[h] high  …");
    }

    void testHeaderFooterAndDynamicTextRenderThemedLines() {
        auto header = ui::HeaderLine::create();
        auto dynamicText = ui::DynamicText::create();
        auto footer = ui::FooterLine::create();

        REQUIRE_EQUAL(header->themeAttributes().element(), theme::Element::HeaderLine);
        REQUIRE_EQUAL(footer->themeAttributes().element(), theme::Element::FooterLine);
        REQUIRE_EQUAL(footer->actionHelp()->themeAttributes().element(), theme::Element::ActionHelp);

        auto updateCount = 0;
        dynamicText->setUpdateFn([&](String &text, const Coordinate width) -> void {
            updateCount += 1;
            text = String{std::format("D{}-{}", updateCount, width)};
        });
        dynamicText->setRectangle(Rectangle{0, 0, 8, 1});
        auto firstBuffer = Buffer{Size{8, 1}, Char{U' '}};
        auto secondBuffer = Buffer{Size{8, 1}, Char{U' '}};
        dynamicText->layout(Size{8, 1}, ui::LayoutContext{});
        dynamicText->updateText();
        dynamicText->onPaint(
            firstBuffer,
            ui::PaintContext{firstBuffer.rect(), firstBuffer.rect(), firstBuffer.rect(), ui::ThemeContext{}});
        dynamicText->layout(Size{8, 1}, ui::LayoutContext{});
        dynamicText->updateText();
        dynamicText->onPaint(
            secondBuffer,
            ui::PaintContext{secondBuffer.rect(), secondBuffer.rect(), secondBuffer.rect(), ui::ThemeContext{}});

        REQUIRE_EQUAL(updateCount, 2);
        requireRowsEqual(firstBuffer, {"D1-8    "});
        requireRowsEqual(secondBuffer, {"D2-8    "});
    }

    void testFooterLineMessageOverlayQueuesAndHidesMessages() {
        auto footer = ui::FooterLine::create();
        footer->setText("left");
        footer->setRectangle(Rectangle{0, 0, 20, 1});
        footer->displayMessage("First", {}, ui::FooterLine::milliseconds{0});
        footer->displayMessage("Second", {}, ui::FooterLine::milliseconds{0});

        auto firstBuffer = renderFooter(*footer, 20);
        requireRowsEqual(firstBuffer, {"left   First        "});

        footer->hideMessage();
        auto secondBuffer = renderFooter(*footer, 20);
        requireRowsEqual(secondBuffer, {"left   Second       "});
    }

    void testFooterLineRejectsExternalChildren() {
        auto footer = ui::FooterLine::create();

        REQUIRE_THROWS_AS(std::logic_error, footer->surfaces().add(ui::Panel::create()));
    }

    void testApplicationQueuesTerminalKeyPressesAndRoutesThemToTheUi() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{20, 4});
        auto application = ui::Application{terminal};
        auto page = ui::Page::create();
        auto actionCalls = std::size_t{0};
        auto quitAction = ui::Action::create("quit");
        quitAction->setKeys(U'q');
        quitAction->setTriggerFn([&actionCalls]() -> void {
            actionCalls += 1;
            ui::getApplication().quit();
        });
        page->actions().add(quitAction);
        application.setMainPage(page);
        backend->_readKeyResults.push(Key{Key::Character, U'q'});

        const auto exitCode = application.run();

        REQUIRE_EQUAL(exitCode, 0);
        REQUIRE_EQUAL(actionCalls, std::size_t{1});
        REQUIRE_EQUAL(backend->_inputMode, Input::Mode::Key);
        REQUIRE(backend->_readKeyCallCount >= 1);
    }

private:
    [[nodiscard]] static auto createLoggingAction(std::string name, Key key, std::vector<std::string> &calls)
        -> ui::ActionPtr {
        auto action = ui::Action::create(std::move(name));
        const auto actionName = action->help().name();
        action->setKeys(std::move(key));
        action->setTriggerFn([&calls, actionName]() -> void { calls.push_back(actionName); });
        return action;
    }

    [[nodiscard]] static auto renderKeyLabels(const std::vector<Key> &keys) -> std::vector<std::string> {
        auto result = std::vector<std::string>{};
        result.reserve(keys.size());
        for (const auto &key : keys) {
            result.emplace_back(key.toDisplayText(false));
        }
        return result;
    }

    [[nodiscard]] static auto renderFooter(ui::FooterLine &footer, const Coordinate width) -> Buffer {
        auto buffer = Buffer{Size{width, 1}, Char{U'.'}};
        footer.layout(Size{width, 1}, ui::LayoutContext{});
        footer.onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), ui::ThemeContext{}});
        for (const auto &child : footer.surfaces()) {
            if (!child->flags().isVisible()) {
                continue;
            }
            child->onPaint(
                buffer,
                ui::PaintContext{child->rectangle(), child->rectangle(), child->rectangle(), ui::ThemeContext{}});
        }
        return buffer;
    }
};
