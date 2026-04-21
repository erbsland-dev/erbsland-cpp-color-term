..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

************
UI Framework
************

The :cpp:any:`erbsland::cterm::ui` module lets you build full-screen
terminal applications from a structured surface tree instead of manually
repainting one big buffer. Pages, layouts, and surfaces describe the UI
declaratively, while :cpp:any:`Application <erbsland::cterm::ui::Application>`
takes care of layout passes, paint invalidation, key routing, and timed
actions.

At a glance, the UI framework gives you:

* :cpp:any:`Application <erbsland::cterm::ui::Application>` as the
  runtime and terminal owner
* :cpp:any:`Page <erbsland::cterm::ui::Page>` and
  :cpp:any:`Surface <erbsland::cterm::ui::Surface>` as the tree model
* :cpp:any:`Stack <erbsland::cterm::ui::layout::Stack>` for common
  vertical and horizontal compositions
* :cpp:any:`Centered <erbsland::cterm::ui::layout::Centered>` and
  :cpp:any:`Frame <erbsland::cterm::ui::layout::Frame>` for compact
  one-child composition
* :cpp:any:`ScrollArea <erbsland::cterm::ui::layout::ScrollArea>` and
  :cpp:any:`Viewport <erbsland::cterm::ui::layout::Viewport>` for
  clipped, scrollable content
* :cpp:any:`Sections <erbsland::cterm::ui::layout::Sections>` and
  :cpp:any:`Buttons <erbsland::cterm::ui::layout::Buttons>` for common
  structured interaction areas
* built-in surfaces such as
  :cpp:any:`Panel <erbsland::cterm::ui::surface::Panel>`,
  :cpp:any:`TextBox <erbsland::cterm::ui::surface::TextBox>`,
  :cpp:any:`Button <erbsland::cterm::ui::surface::Button>`,
  :cpp:any:`ScrollingBufferView <erbsland::cterm::ui::surface::ScrollingBufferView>`,
  :cpp:any:`HeaderLine <erbsland::cterm::ui::surface::HeaderLine>`,
  and :cpp:any:`FooterLine <erbsland::cterm::ui::surface::FooterLine>`
* :cpp:any:`Choice <erbsland::cterm::ui::page::Choice>` for small
  non-opaque modal prompts
* :cpp:any:`Action <erbsland::cterm::ui::Action>` objects and surface
  schedulers for responsive interaction
* :doc:`themes <theme>` for shared colors, block shapes, and padding
  rules across built-in and custom controls
* local surface visibility, so layouts and painting can keep inactive
  surfaces in the tree without allocating space for them

.. important::

    The UI Framework is currently in beta, and parts of the API may
    change in future releases.

Quick Start
===========

The umbrella include ``<erbsland/cterm/ui/all.hpp>`` re-exports the
``layout`` and ``surface`` namespaces into ``ui``. This lets you write
compact code such as ``ui::Stack`` or ``ui::TextBox`` directly.

For very small programs, create an :cpp:any:`Application <erbsland::cterm::ui::Application>` instance directly, build
one page, assign it with :cpp:any:`Application::setMainPage() <erbsland::cterm::ui::Application::setMainPage()>`, and
call :cpp:any:`Application::run() <erbsland::cterm::ui::Application::run()>`.

.. code-block:: cpp

    using namespace erbsland::cterm;
    using namespace erbsland::cterm::ui;

    auto page = Page::create();
    auto root = Stack::create(Orientation::Vertical);
    auto title = TextBox::create("Library Status", Alignment::Center);
    auto body = Panel::create();

    title->editLayoutMetrics().setFixedHeight(1);
    body->addSurface(TextBox::create("Everything is running.", Alignment::Center));

    root->addSurface(title);
    root->addSurface(body);
    page->addSurface(root);

    auto app = Application{};
    app.setTheme(theme::Theme::dark());
    app.setMainPage(page);
    return app.run();

Applications with command line handling, data loading, or reusable UI setup usually derive from
:cpp:any:`Application <erbsland::cterm::ui::Application>`. Override
:cpp:any:`Application::setupUi() <erbsland::cterm::ui::Application::setupUi()>` to build the surface tree, and override
:cpp:any:`Application::processCommandLineArguments()
<erbsland::cterm::ui::Application::processCommandLineArguments()>` when arguments select data or options for that tree.

.. code-block:: cpp

    class DemoApp final : public Application {
    public:
        DemoApp(const int argc, char *argv[]) : Application(argc, argv) {}

    protected:
        void setupUi() override {
            _page = Page::create();
            _body = TextBox::create("", Alignment::Center);
            _page->addSurface(_body);
            setMainPage(_page);
        }

        auto processCommandLineArguments(const CommandLineArgs &args) -> ExitCode override {
            _body->setText(args.size() > 1 ? String{args[1]} : String{"Ready"});
            return cExitCodeContinue;
        }

    private:
        PagePtr _page;
        TextBoxPtr _body;
    };

    auto main(const int argc, char *argv[]) -> int {
        return DemoApp{argc, argv}.run();
    }

The startup sequence is:

* ``setupUi()`` builds the page and surface structure.
* ``initializeTerminal()`` prepares the terminal.
* ``processCommandLineArguments()`` parses arguments and can assign data to the prepared UI surfaces.
* ``initialize()`` creates the display, event driver, and scheduler infrastructure.
* ``runEventLoop()`` processes input, scheduled actions, layout, and painting until the app quits.

Painting Model
==============

Each surface paints only its own content in local coordinates.
The display walks the surface tree, skips hidden surfaces, clips child
surfaces to their parents, and repaints only visible dirty regions.
Custom surfaces should use ``context.surfaceRect()`` for their full local bounds, ``context.visibleRect()`` for the
ancestor-clipped area, and ``context.dirtyRect()`` for the current repaint region.
For themed painting, use ``context.theme()`` to get a
:cpp:any:`ThemeAccessor <erbsland::cterm::theme::ThemeAccessor>`.
From there, ``forPart()``, ``style()``, ``color()``, ``block()``, and
``contentRect()`` resolve values for the current surface. These helpers
use the current
:cpp:any:`ThemeContext <erbsland::cterm::ui::ThemeContext>` for the
surface being painted.

Hidden surfaces remain part of the tree, keep their last assigned
rectangle, and still own their actions and scheduled callbacks.
They are ignored by layout, paint traversal, and focus routing until
shown again.

Scrollable Content
==================

The UI framework has two scroll layers:

* :cpp:any:`AbstractScrollArea <erbsland::cterm::ui::surface::AbstractScrollArea>`
  is a base for custom-painted scrollable surfaces. It owns scroll
  state, common scroll methods, and optional scroll bars, but subclasses
  paint their own content in ``onPaintArea()``.
* :cpp:any:`ScrollArea <erbsland::cterm::ui::layout::ScrollArea>` is a
  ready-to-use layout surface. It owns a
  :cpp:any:`Viewport <erbsland::cterm::ui::layout::Viewport>`, a
  horizontal scroll bar, a vertical scroll bar, and the scroll corner.
  Applications assign one content surface with ``setContentSurface()``.

Both layers use ``scrollOffset`` terminology and provide directional,
page, edge, and ``scrollIntoView()`` helpers. Scroll bars are configured
per orientation with :cpp:any:`ScrollBarMode <erbsland::cterm::ui::ScrollBarMode>`.
