// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "UiSchedulerManualClock.hpp"

#include "support/TerminalTestHelper.hpp"

#include <erbsland/cterm/theme/Theme.hpp>
#include <erbsland/cterm/theme/ThemeBuilder.hpp>
#include <erbsland/cterm/ui/event/impl/EventClockAccess.hpp>
#include <erbsland/cterm/ui/surface/Panel.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <memory>
#include <utility>

class ThemeProbeSurface final : public ui::Surface {
public:
    explicit ThemeProbeSurface(std::shared_ptr<Color> color, std::shared_ptr<theme::States> state, ProtectedTag) :
        Surface{theme::Element::TextBox}, _color{std::move(color)}, _state{std::move(state)} {}

    [[nodiscard]] static auto create(const std::shared_ptr<Color> &color, const std::shared_ptr<theme::States> &state)
        -> std::shared_ptr<ThemeProbeSurface> {
        return std::make_shared<ThemeProbeSurface>(color, state, ProtectedTag{});
    }

public: // implement Surface
    [[nodiscard]] auto isOpaque() const noexcept -> bool override { return true; }

    void onPaint(WritableBuffer &buffer, const ui::PaintContext &context) noexcept override {
        *_color = context.theme().forPart(theme::Part::Background).color();
        *_state = context.themeContext().state();
        buffer.fill(context.surfaceRect(), Char{U'P', *_color, CharAttributes{}});
    }

private:
    std::shared_ptr<Color> _color;
    std::shared_ptr<theme::States> _state;
};

class ThemeLayoutProbeSurface final : public ui::Surface {
public:
    explicit ThemeLayoutProbeSurface(Rectangle *contentRect, ProtectedTag) :
        Surface{theme::Element::HorizontalScrollBar}, _contentRect{contentRect} {}

    [[nodiscard]] static auto create(Rectangle &contentRect) -> std::shared_ptr<ThemeLayoutProbeSurface> {
        return std::make_shared<ThemeLayoutProbeSurface>(&contentRect, ProtectedTag{});
    }

public: // implement Surface
    void onLayout(ui::LayoutScope &scope) noexcept override {
        *_contentRect = scope.theme().forPart(theme::Part::Track).contentRect(Rectangle{Position{}, scope.size()});
    }

private:
    Rectangle *_contentRect;
};

TESTED_TARGETS(UiApplication UiDisplay UiPaintContext UiLayoutContext Theme)
class UiThemeTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testDisplayThemeReachesPaintContext() {
        auto setup = createSetup(applicationTheme(Color{fg::Red, bg::Blue}));
        renderInitialFrame(setup);

        REQUIRE_EQUAL(*setup.probeColor, Color(fg::Red, bg::Blue));
    }

    void testPageThemeOverrideWinsOverApplicationTheme() {
        auto setup = createSetup(applicationTheme(Color{fg::Red, bg::Blue}));
        setup.page->setTheme(applicationTheme(Color{fg::Green, bg::Yellow}));
        renderInitialFrame(setup);

        REQUIRE_EQUAL(*setup.probeColor, Color(fg::Green, bg::Yellow));
    }

    void testThemeChangesInvalidateLayoutAndPaint() {
        auto setup = createSetup(applicationTheme(Color{fg::Red, bg::Blue}));
        renderInitialFrame(setup);

        REQUIRE_FALSE(setup.page->flags().isLayoutOutdated());
        REQUIRE_FALSE(setup.page->flags().isPaintOutdated());

        setup.display.setTheme(applicationTheme(Color{fg::Green, bg::Yellow}));

        REQUIRE(setup.page->flags().isLayoutOutdated());
        REQUIRE(setup.page->flags().isPaintOutdated());
    }

    void testPageThemeChangesInvalidateLayoutAndPaint() {
        auto setup = createSetup(applicationTheme(Color{fg::Red, bg::Blue}));
        renderInitialFrame(setup);

        setup.page->setTheme(applicationTheme(Color{fg::Green, bg::Yellow}));

        REQUIRE(setup.page->flags().isLayoutOutdated());
        REQUIRE(setup.page->flags().isPaintOutdated());
    }

    void testFocusUpdatesAutomaticThemeStateAndInvalidatesSurfaces() {
        auto setup = createSetup(applicationTheme(Color{fg::Red, bg::Blue}));
        renderInitialFrame(setup);

        setup.page->focusTo(setup.probe);

        REQUIRE(setup.probe->flags().themeStates().contains(theme::State::Focused));
        REQUIRE(setup.root->flags().themeStates().contains(theme::State::FocusWithin));
        REQUIRE(setup.probe->flags().isLayoutOutdated());
        REQUIRE(setup.root->flags().isPaintOutdated());
    }

    void testSurfaceFlagsCreateThemeStatesAndInvalidateTheme() {
        auto surface = ui::Panel::create();
        surface->layout(Size{10, 5}, ui::LayoutContext{});

        surface->flags().setEnabled(false);
        surface->flags().setSelected(true);
        surface->flags().setChecked(true);

        const auto states = surface->flags().themeStates();
        REQUIRE(states.contains(theme::State::Disabled));
        REQUIRE(states.contains(theme::State::Selected));
        REQUIRE(states.contains(theme::State::Checked));
        REQUIRE(surface->flags().isLayoutOutdated());
        REQUIRE(surface->flags().isPaintOutdated());
    }

    void testThemeAttributesArePassiveAndUsedInThemeContext() {
        auto surface = ui::Panel::create();
        const auto tag = theme::Tag{1};
        surface->layout(Size{10, 5}, ui::LayoutContext{});

        surface->themeAttributes().setElement(theme::Element::TextBox);
        surface->themeAttributes().setTags(theme::Tags{tag});

        const auto context = surface->themeContextFrom(ui::ThemeContext{});

        REQUIRE_EQUAL(context.element(), theme::Element::TextBox);
        REQUIRE(context.tags().contains(tag));
        REQUIRE_FALSE(surface->flags().isLayoutOutdated());

        surface->flags().setThemeOutdated();

        REQUIRE(surface->flags().isLayoutOutdated());
        REQUIRE(surface->flags().isPaintOutdated());
    }

    void testThemeInvalidationRecursesIntoChildren() {
        auto parent = ui::Panel::create();
        auto child = ui::Panel::create();
        parent->addSurface(child);
        parent->layout(Size{10, 5}, ui::LayoutContext{});
        child->layout(Size{5, 3}, ui::LayoutContext{});

        parent->flags().setThemeOutdated();

        REQUIRE(parent->flags().isLayoutOutdated());
        REQUIRE(parent->flags().isPaintOutdated());
        REQUIRE(child->flags().isLayoutOutdated());
        REQUIRE(child->flags().isPaintOutdated());
    }

    void testLayoutContextCarriesTheThemeContext() {
        auto contentRect = Rectangle{};
        auto probe = ThemeLayoutProbeSurface::create(contentRect);
        const auto context = ui::LayoutContext{theme::Theme::dark(), theme::Element::HorizontalScrollBar};

        probe->layout(Size{10, 1}, context);

        REQUIRE_EQUAL(contentRect, Rectangle(1, 0, 8, 1));
    }

private:
    struct Setup final {
        std::shared_ptr<UiSchedulerManualClock> clock;
        ui::impl::ScopedEventClockOverride clockOverride;
        std::shared_ptr<TerminalTestBackend> backend;
        std::shared_ptr<Terminal> terminal;
        ui::PagePtr page;
        ui::surface::PanelPtr root;
        std::shared_ptr<ThemeProbeSurface> probe;
        std::shared_ptr<theme::States> probeState;
        std::shared_ptr<Color> probeColor;
        ui::Display display;
    };

    [[nodiscard]] auto createSetup(theme::ThemeConstPtr theme) -> Setup {
        auto clock = std::make_shared<UiSchedulerManualClock>();
        auto backend = std::make_shared<TerminalTestBackend>();
        auto terminal = std::make_shared<Terminal>(backend, Size{6, 1});
        terminal->setOutputMode(Terminal::OutputMode::Text);
        auto page = ui::Page::create();
        auto root = ui::surface::Panel::create();
        auto probeState = std::make_shared<theme::States>();
        auto probeColor = std::make_shared<Color>();
        auto probe = ThemeProbeSurface::create(probeColor, probeState);
        probe->setRectangle(Rectangle{0, 0, 6, 1});
        root->addSurface(probe);
        probe->flags().setFocusable(true);
        probe->setRectangle(Rectangle{0, 0, 6, 1});
        page->addSurface(root);
        root->setRectangle(Rectangle{0, 0, 6, 1});
        return Setup{
            .clock = clock,
            .clockOverride = ui::impl::ScopedEventClockOverride{[clock]() -> ui::EventTime { return clock->now(); }},
            .backend = backend,
            .terminal = terminal,
            .page = page,
            .root = root,
            .probe = probe,
            .probeState = probeState,
            .probeColor = probeColor,
            .display = ui::Display{terminal, page, std::move(theme), exactTerminalSizeLimits()},
        };
    }

    static auto applicationTheme(Color textBoxBackground) -> theme::ThemeConstPtr {
        auto builder = theme::ThemeBuilder::from(theme::Theme::dark());
        builder.edit(theme::Selector{theme::Element::TextBox, theme::Part::Background}).setColor(textBoxBackground);
        return builder.build();
    }

    static void renderInitialFrame(Setup &setup) {
        setup.display.pollRender();
        setup.backend->clearOutput();
    }
};
