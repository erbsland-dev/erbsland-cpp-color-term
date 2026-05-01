// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TerminalTestHelper.hpp"

#include <erbsland/cterm/theme/Theme.hpp>
#include <erbsland/cterm/theme/ThemeBuilder.hpp>
#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

TESTED_TARGETS(UiChoice UiButton UiButtons UiDisplay)
class UiChoiceTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testChoiceIsNonOpaqueAndPaintsOnlyThePrompt() {
        auto choice = ui::Choice::create("Quit?", "Are you sure to quit the application?");
        choice->addChoice("cancel", "Cancel", Key::Escape);
        choice->addChoice("quit", "Quit", U'q');

        const auto output = renderChoiceOnDottedPage(choice, Size{50, 12});
        const auto frame = findSurface<ui::Frame>(choice);
        REQUIRE(frame != nullptr);
        const auto frameRect = frame->localToScreen(frame->localSurfaceRect());

        REQUIRE_FALSE(choice->isOpaque());
        REQUIRE_EQUAL(outputCharAt(output, Size{50, 12}, Position{0, 0}), U'.');
        REQUIRE_FALSE(outputCharAt(output, Size{50, 12}, frameRect.topLeft()) == U'.');
        REQUIRE(output.find("Are you sure") != std::string::npos);
        REQUIRE(output.find("Cancel") != std::string::npos);
        REQUIRE(output.find("Quit") != std::string::npos);
    }

    void testChoiceOrdersChoicesByPriorityThenInsertionOrder() {
        auto choice = ui::Choice::create("Order");
        choice->addChoice("low", "Low", U'l', 0);
        choice->addChoice("high", "High", U'h', 10);
        choice->addChoice("second-high", "Second High", U's', 10);
        const auto buttons = findSurface<ui::Buttons>(choice);

        REQUIRE(buttons != nullptr);
        REQUIRE_EQUAL(buttons->button(0)->action()->help().name(), std::string{"High"});
        REQUIRE_EQUAL(buttons->button(1)->action()->help().name(), std::string{"Second High"});
        REQUIRE_EQUAL(buttons->button(2)->action()->help().name(), std::string{"Low"});
    }

    void testChoiceConfiguresReadableDescriptionWidth() {
        auto choice = ui::Choice::create("Prompt", "A longer description");
        const auto textBox = findSurface<ui::TextBox>(choice);
        REQUIRE(textBox != nullptr);

        REQUIRE_EQUAL(choice->preferredLineWidth(), std::optional<Coordinate>{78});
        REQUIRE_EQUAL(textBox->preferredLineWidth(), std::optional<Coordinate>{78});

        choice->setPreferredLineWidth(40);

        REQUIRE_EQUAL(choice->preferredLineWidth(), std::optional<Coordinate>{40});
        REQUIRE_EQUAL(textBox->preferredLineWidth(), std::optional<Coordinate>{40});
    }

    void testChoiceDefaultPaddingMatchesTheFrame() {
        auto choice = ui::Choice::create("Prompt", "Description");
        const auto frame = findSurface<ui::Frame>(choice);
        REQUIRE(frame != nullptr);

        REQUIRE_EQUAL(choice->padding(), (Margins{1, 2, 0, 2}));
        REQUIRE_EQUAL(frame->padding(), (Margins{1, 2, 0, 2}));
    }

    void testChoiceDefaultPaddingLeavesNoEmptyRowBelowButtons() {
        auto choice = ui::Choice::create("Prompt", "Description");
        choice->addChoice("ok", "OK", Key::Enter);

        layoutChoice(choice, Size{80, 12});

        const auto frame = findSurface<ui::Frame>(choice);
        const auto buttons = findSurface<ui::Buttons>(choice);
        REQUIRE(frame != nullptr);
        REQUIRE(buttons != nullptr);

        const auto frameRect = frame->localToScreen(frame->localSurfaceRect());
        const auto buttonsRect = buttons->localToScreen(buttons->localSurfaceRect());
        REQUIRE_EQUAL(buttonsRect.y2(), frameRect.y2() - Coordinate{1});
    }

    void testChoiceClampsLargeMarginsToKeepPromptUsable() {
        auto choice = ui::Choice::create("Small", "Description");
        choice->addChoice("ok", "OK", Key::Enter);
        choice->setMargins(Margins{100});

        layoutChoice(choice, Size{20, 8});

        const auto buttons = findSurface<ui::Buttons>(choice);
        REQUIRE(buttons != nullptr);
        REQUIRE(buttons->rectangle().width() > 0);
        REQUIRE(buttons->rectangle().height() > 0);
        REQUIRE(buttons->localToScreen(buttons->localSurfaceRect()).x1() >= 0);
        REQUIRE(buttons->localToScreen(buttons->localSurfaceRect()).x2() <= 20);
    }

    void testChoiceUsesThemeButtonSpacingInPromptHeight() {
        auto builder = theme::ThemeBuilder::zero();
        builder.edit(theme::Selector{theme::Element::Buttons, theme::Part::Spacing}).setMargins(Margins{3, 1});
        builder.edit(theme::Selector{theme::Element::Button, theme::Part::Border})
            .setBlock(theme::BlockRole::LeftBracket, U'▌')
            .setBlock(theme::BlockRole::RightBracket, U'▐')
            .setMargins(Margins{1})
            .setPadding(Margins{2, 0});
        builder.edit(theme::Selector{theme::Element::Button, theme::Part::Text}).setPadding(Margins{2, 0});
        builder.edit(theme::Selector{theme::Element::Button, theme::Part::KeyBracket}).setBlocks(U"    /    [ ]    ");
        const auto activeTheme = builder.build();
        auto choice = ui::Choice::create("Wrap");
        choice->addChoice("one", "One", U'1');
        choice->addChoice("two", "Two", U'2');
        choice->setMargins(Margins{1});

        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{30, 10});
        auto display = ui::Display{terminal, choice, activeTheme, exactTerminalSizeLimits()};
        display.pollRender();

        const auto buttons = findSurface<ui::Buttons>(choice);
        REQUIRE(buttons != nullptr);
        REQUIRE_EQUAL(buttons->rectangle().height(), Coordinate{3});
        REQUIRE_EQUAL(buttons->button(1)->rectangle().y1(), Coordinate{2});
    }

    void testChoiceDescriptionUsesChoiceTextTheme() {
        auto builder = theme::ThemeBuilder::zero();
        builder.edit(theme::Selector{theme::Element::Choice, theme::Part::Text})
            .setColor(fg::BrightWhite)
            .setMargins(Margins{0, 0, 1, 0});
        const auto activeTheme = builder.build();
        auto choice = ui::Choice::create("Prompt", "Description");
        choice->addChoice("ok", "OK", Key::Enter);

        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{30, 10});
        auto display = ui::Display{terminal, choice, activeTheme, exactTerminalSizeLimits()};
        display.pollRender();

        const auto descriptionText = findSurface<ui::TextBox>(choice);
        const auto buttons = findSurface<ui::Buttons>(choice);
        REQUIRE(descriptionText != nullptr);
        REQUIRE(buttons != nullptr);
        REQUIRE_EQUAL(descriptionText->themeAttributes().element(), theme::Element::Choice);
        REQUIRE_EQUAL(buttons->rectangle().y1(), descriptionText->rectangle().y2() + Coordinate{1});

        auto buffer = Buffer{descriptionText->rectangle().size(), Char{U'.'}};
        const auto themeContext = descriptionText->themeContextFrom(ui::ThemeContext{activeTheme});
        descriptionText->onPaint(buffer, ui::PaintContext{buffer.rect(), buffer.rect(), buffer.rect(), themeContext});

        REQUIRE_EQUAL(buffer.get(Position{0, 0}).color().fg(), fg::BrightWhite);
    }

    void testChoiceLongDescriptionKeepsTextMarginAndFullHeight() {
        auto choice = ui::Choice::create(
            "Pick a Direction",
            "Please pick one of the directions below, or cancel with escape. "
            "Tea is at once a plant, a drink, a ritual, a commodity, and a language of hospitality. In many homes it "
            "appears so often that it almost escapes notice, yet its story is unusually rich. The leaves that become "
            "green tea, black tea, white tea, oolong, and many other styles all come from one remarkable evergreen "
            "species, <em>Camellia sinensis</em>. From mountain gardens wrapped in mist to busy city kitchens where "
            "kettles begin to sing, tea connects agriculture, craft, trade, memory, and daily comfort.");
        choice->addChoice("north", "North", U'n', 30);
        choice->addChoice("east", "East", U'e', 20);
        choice->addChoice("south", "South", U's', 10);
        choice->addChoice("cancel", "Cancel", Key::Escape, -10);

        const auto output = renderChoiceOnDottedPage(choice, Size{150, 27}, choiceTextMarginTheme());
        const auto descriptionText = findSurface<ui::TextBox>(choice);
        const auto buttons = findSurface<ui::Buttons>(choice);
        REQUIRE(descriptionText != nullptr);
        REQUIRE(buttons != nullptr);

        const auto descriptionRect = descriptionText->localToScreen(descriptionText->localSurfaceRect());
        const auto buttonsRect = buttons->localToScreen(buttons->localSurfaceRect());
        REQUIRE_EQUAL(buttonsRect.y1(), descriptionRect.y2() + Coordinate{1});
        REQUIRE(output.find("daily comfort.") != std::string::npos);
    }

    void testChoiceClipsLongDescriptionInsidePromptFrame() {
        auto choice =
            ui::Choice::create("Small", "This description is intentionally too long for the compact prompt height.");
        choice->addChoice("ok", "OK", Key::Enter);
        choice->setMargins(Margins{1});
        const auto output = renderChoiceOnDottedPage(choice, Size{26, 7});
        const auto frame = findSurface<ui::Frame>(choice);
        REQUIRE(frame != nullptr);

        const auto frameRect = frame->localToScreen(frame->localSurfaceRect());
        for (auto y = Coordinate{0}; y < 7; ++y) {
            for (auto x = Coordinate{0}; x < 26; ++x) {
                const auto pos = Position{x, y};
                if (!frameRect.contains(pos)) {
                    REQUIRE_EQUAL(outputCharAt(output, Size{26, 7}, pos), U'.');
                }
            }
        }
    }

    void testChoicePopsBeforeCallingSelectionCallback() {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{50, 12});
        auto application = ui::Application{terminal};
        auto mainPage = ui::Page::create();
        application.setMainPage(mainPage);
        REQUIRE_EQUAL(application.manualInitialize(), ui::Application::cExitCodeContinue);

        auto choice = ui::Choice::create("Quit?", "Are you sure?");
        auto callbackLog = std::vector<std::string>{};
        choice->setCallback([&](const ui::Choice::Selection &selection) {
            callbackLog.emplace_back(selection.id);
            try {
                ui::getApplication().display().popPage();
                callbackLog.emplace_back("still-on-stack");
            } catch (const std::logic_error &) {
                callbackLog.emplace_back("already-popped");
            }
        });
        choice->addChoice("cancel", "Cancel", Key::Escape);
        choice->addChoice("quit", "Quit", U'q');
        choice->show();

        auto keyPressEvent = ui::KeyPressEvent{Key{Key::Character, U'q'}};
        application.display().onKeyPress(keyPressEvent);
        application.manualShutdown();

        REQUIRE(keyPressEvent.isHandled());
        REQUIRE_EQUAL(callbackLog, (std::vector<std::string>{"quit", "already-popped"}));
    }

private:
    template <typename SurfaceType>
    [[nodiscard]] static auto findSurface(const ui::SurfacePtr &root) -> std::shared_ptr<SurfaceType> {
        if (root == nullptr) {
            return {};
        }
        if (auto surface = std::dynamic_pointer_cast<SurfaceType>(root); surface != nullptr) {
            return surface;
        }
        for (const auto &child : root->surfaces()) {
            if (auto surface = findSurface<SurfaceType>(child); surface != nullptr) {
                return surface;
            }
        }
        return {};
    }

    static void layoutChoice(const ui::ChoicePtr &choice, const Size size) {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, size);
        auto display = ui::Display{terminal, choice, theme::Theme::zero(), exactTerminalSizeLimits()};
        display.pollRender();
    }

    [[nodiscard]] static auto renderChoiceOnDottedPage(const ui::ChoicePtr &choice, const Size size) -> std::string {
        return renderChoiceOnDottedPage(choice, size, theme::Theme::zero());
    }

    [[nodiscard]] static auto
    renderChoiceOnDottedPage(const ui::ChoicePtr &choice, const Size size, theme::ThemeConstPtr activeTheme)
        -> std::string {
        const auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, size);
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto page = ui::Page::create();
        const auto renderTheme = pageTheme(std::move(activeTheme), U'.');
        page->setTheme(renderTheme);
        auto display = ui::Display{terminal, page, renderTheme, exactTerminalSizeLimits()};
        display.pushPage(choice);
        display.pollRender();
        return backend->output();
    }

    [[nodiscard]] static auto outputCharAt(const std::string &output, const Size size, const Position pos) -> char32_t {
        if (!size.contains(pos)) {
            return U'\0';
        }
        auto lineStart = std::size_t{0};
        for (auto y = Coordinate{0}; y < pos.y(); ++y) {
            lineStart = output.find('\n', lineStart);
            if (lineStart == std::string::npos) {
                return U'\0';
            }
            lineStart += 1;
        }
        auto lineEnd = output.find('\n', lineStart);
        if (lineEnd == std::string::npos) {
            lineEnd = output.size();
        }
        const auto line = String{std::string_view{output}.substr(lineStart, lineEnd - lineStart)};
        const auto index = static_cast<std::size_t>(pos.x());
        if (index >= line.size()) {
            return U'\0';
        }
        return line[index].mainCodePoint();
    }

    [[nodiscard]] static auto pageTheme(theme::ThemeConstPtr activeTheme, const char32_t backgroundBlock)
        -> theme::ThemeConstPtr {
        auto builder = theme::ThemeBuilder::from(activeTheme);
        builder.edit(theme::Selector{theme::Element::Page, theme::Part::Background}).setBlocks(backgroundBlock);
        return builder.build();
    }

    [[nodiscard]] static auto choiceTextMarginTheme() -> theme::ThemeConstPtr {
        auto builder = theme::ThemeBuilder::zero();
        builder.edit(theme::Selector{theme::Element::Choice, theme::Part::Text}).setMargins(Margins{0, 0, 1, 0});
        return builder.build();
    }
};
