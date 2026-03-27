// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TerminalTestSupport.hpp"

#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <memory>
#include <string>
#include <vector>

namespace ui = erbsland::cterm::ui;

namespace ui_key_press_handling_test {

class RecordingSurface final : public ui::Surface {
public:
    explicit RecordingSurface(
        std::shared_ptr<std::vector<std::string>> callLog, std::string name, bool handleKeyPress, ProtectedTag) :
        _callLog(std::move(callLog)), _name(std::move(name)), _handleKeyPress(handleKeyPress) {}

    [[nodiscard]] static auto
    create(const std::shared_ptr<std::vector<std::string>> &callLog, std::string name, bool handleKeyPress = false)
        -> std::shared_ptr<RecordingSurface> {
        return std::make_shared<RecordingSurface>(callLog, std::move(name), handleKeyPress, ProtectedTag{});
    }

public:
    void onKeyPress(ui::KeyPressEvent &keyPressEvent) noexcept override {
        _callLog->push_back(_name);
        Surface::onKeyPress(keyPressEvent);
        if (!keyPressEvent.isHandled() && _handleKeyPress) {
            keyPressEvent.setHandled();
        }
    }

private:
    std::shared_ptr<std::vector<std::string>> _callLog;
    std::string _name;
    bool _handleKeyPress{false};
};

}


TESTED_TARGETS(UiKeyBindings UiKeyPress UiApplication)
class UiKeyPressHandlingTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testSurfaceKeyBindingsHandleMatchingKeysOnly() {
        auto panel = ui::Panel::create();
        auto bindingCalls = std::size_t{0};
        panel->keyBindings().bind(Key{Key::Character, U'x'}, [&bindingCalls] { bindingCalls += 1; });

        auto unmatchedEvent = ui::KeyPressEvent{Key{Key::Character, U'y'}};
        panel->onKeyPress(unmatchedEvent);
        REQUIRE_EQUAL(bindingCalls, std::size_t{0});
        REQUIRE_FALSE(unmatchedEvent.isHandled());

        auto matchedEvent = ui::KeyPressEvent{Key{Key::Character, U'x'}};
        panel->onKeyPress(matchedEvent);
        REQUIRE_EQUAL(bindingCalls, std::size_t{1});
        REQUIRE(matchedEvent.isHandled());
    }

    void testFocusToRejectsForeignSurfaces() {
        auto page = ui::Page::create();
        auto foreignSurface = ui_key_press_handling_test::RecordingSurface::create(
            std::make_shared<std::vector<std::string>>(), "foreign");

        page->focusTo(foreignSurface);

        REQUIRE_FALSE(page->hasFocusSurface());
    }

    void testPageDispatchesKeyPressAlongTheFocusChain() {
        auto page = ui::Page::create();
        auto callLog = std::make_shared<std::vector<std::string>>();
        auto parent = ui_key_press_handling_test::RecordingSurface::create(callLog, "parent");
        auto child = ui_key_press_handling_test::RecordingSurface::create(callLog, "child");
        parent->addChild(child);
        page->addChild(parent);
        page->focusTo(child);
        page->keyBindings().bind(Key{Key::Character, U'k'}, [callLog] { callLog->emplace_back("page"); });

        auto keyPressEvent = ui::KeyPressEvent{Key{Key::Character, U'k'}};
        page->onKeyPress(keyPressEvent);

        REQUIRE(keyPressEvent.isHandled());
        REQUIRE_EQUAL(*callLog, (std::vector<std::string>{"child", "parent", "page"}));
    }

    void testHandledKeyStopsFurtherFocusChainDispatch() {
        auto page = ui::Page::create();
        auto callLog = std::make_shared<std::vector<std::string>>();
        auto parent = ui_key_press_handling_test::RecordingSurface::create(callLog, "parent");
        auto child = ui_key_press_handling_test::RecordingSurface::create(callLog, "child", true);
        parent->addChild(child);
        page->addChild(parent);
        page->focusTo(child);
        page->keyBindings().bind(Key{Key::Character, U'h'}, [callLog] { callLog->emplace_back("page"); });

        auto keyPressEvent = ui::KeyPressEvent{Key{Key::Character, U'h'}};
        page->onKeyPress(keyPressEvent);

        REQUIRE(keyPressEvent.isHandled());
        REQUIRE_EQUAL(*callLog, (std::vector<std::string>{"child"}));
    }

    void testForeignFocusFallsBackToThePageAndClearsTheStoredFocus() {
        auto page = ui::Page::create();
        auto callLog = std::make_shared<std::vector<std::string>>();
        auto child = ui_key_press_handling_test::RecordingSurface::create(callLog, "child");
        page->addChild(child);
        page->focusTo(child);
        page->removeChild(child);
        page->keyBindings().bind(Key{Key::Character, U'f'}, [callLog] { callLog->emplace_back("page"); });

        auto keyPressEvent = ui::KeyPressEvent{Key{Key::Character, U'f'}};
        page->onKeyPress(keyPressEvent);

        REQUIRE(keyPressEvent.isHandled());
        REQUIRE_EQUAL(*callLog, (std::vector<std::string>{"page"}));
        REQUIRE_FALSE(page->hasFocusSurface());
    }

    void testExpiredFocusFallsBackToThePageAndClearsTheStoredFocus() {
        auto page = ui::Page::create();
        auto callLog = std::make_shared<std::vector<std::string>>();
        auto child = ui_key_press_handling_test::RecordingSurface::create(callLog, "child");
        page->addChild(child);
        page->focusTo(child);
        page->removeChild(child);
        child.reset();
        page->keyBindings().bind(Key{Key::Character, U'e'}, [callLog] { callLog->emplace_back("page"); });

        auto keyPressEvent = ui::KeyPressEvent{Key{Key::Character, U'e'}};
        page->onKeyPress(keyPressEvent);

        REQUIRE(keyPressEvent.isHandled());
        REQUIRE_EQUAL(*callLog, (std::vector<std::string>{"page"}));
        REQUIRE_FALSE(page->hasFocusSurface());
    }

    void testApplicationQueuesTerminalKeyPressesAndRoutesThemToTheUi() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{20, 4});
        auto application = ui::Application{terminal};
        auto page = ui::Page::create();
        auto bindingCalls = std::size_t{0};
        page->keyBindings().bind(Key{Key::Character, U'q'}, [&bindingCalls] {
            bindingCalls += 1;
            ui::getApplication().quit();
        });
        application.setMainPage(page);
        backend->_readKeyResults.push(Key{Key::Character, U'q'});

        const auto exitCode = application.run();

        REQUIRE_EQUAL(exitCode, 0);
        REQUIRE_EQUAL(bindingCalls, std::size_t{1});
        REQUIRE_EQUAL(backend->_inputMode, Input::Mode::Key);
        REQUIRE(backend->_readKeyCallCount >= 1);
    }
};
