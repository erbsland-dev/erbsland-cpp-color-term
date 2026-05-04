..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

********
UI Input
********

The UI framework routes key presses through shared
:cpp:any:`Action <erbsland::cterm::ui::Action>` objects. An action owns
trigger keys, enablement rules, and the callback that runs when the
action is accepted. Its user-facing name, detailed description,
visibility, and display priority live in
:cpp:any:`HelpData <erbsland::cterm::ui::HelpData>`, which you access
through ``action->help()``.

Actions are attached to surfaces and pages through
:cpp:any:`Surface::actions() <erbsland::cterm::ui::Surface::actions()>`.
The same ``ActionPtr`` can be attached to many surfaces, so application
commands such as quit, save, or toggle-help can be implemented once and
reused wherever they make sense.

Button surfaces use :cpp:any:`ButtonAction
<erbsland::cterm::ui::ButtonAction>` instead. A ``ButtonAction`` is owned
by one live button and synchronizes its enabled state into that button's
``SurfaceFlags``. Create a separate ``ButtonAction`` for each button,
even when several buttons call the same callback.

.. important::

    The UI Framework is currently in beta, and parts of the API may
    change in future releases.

Usage
=====

Creating Shared Actions
-----------------------

Create an action, assign one or more keys, and attach it to a page or
surface.

.. code-block:: cpp

    auto quitAction = ui::Action::create("quit");
    quitAction->setKeys({Key::Escape, U'q'});
    quitAction->help().setDescription("Quits the application.").setPriority(100);
    quitAction->setTriggerFn([] {
        ui::getApplication().quit();
    });

    page->actions().add(quitAction);

When a key press reaches a surface, the default
``Surface::onKeyPress()`` asks the local action container to trigger the
first enabled action that matches the key. If the matching action is
disabled, dispatch continues so an action on a parent surface or the
page may still handle the same key.

Focused Dispatch
----------------

The active page is the keyboard boundary. It sends a key press to the
focused surface first, then each parent surface, and finally to the page
itself. This gives focused controls the first chance to interpret keys
without letting a transparent page accidentally leak input into the page
below it.

.. code-block:: cpp

    auto saveAction = ui::Action::create("save");
    saveAction->addKey(U's');
    saveAction->setEnabledFn(
        [document] { return document->hasUnsavedChanges(); },
        ui::Action::EnabledUpdateMode::BeforeRepaint);
    saveAction->setTriggerFn([document] {
        document->save();
    });

    editorPage->actions().add(quitAction);
    editorPage->actions().add(saveAction);
    detailsPanel->actions().add(saveAction);

The enabled callback is always refreshed immediately before triggering,
even when the regular update mode is repaint-driven or polled. This
avoids stale enabled state when data changes between two paint passes.

Main and Alternative Keys
-------------------------

Use :cpp:any:`Keys <erbsland::cterm::Keys>` when an action has several
accepted keys. The keys are stored once, in priority order. By default
all keys are shown in compact footer help. When some bindings are only
alternatives for detailed help, set the number of leading main keys.

.. code-block:: cpp

    auto zoomKeys = Keys{U'+', U']'};
    zoomKeys.setMainKeyCount(1);

    auto zoomInAction = ui::Action::create("zoom in");
    zoomInAction->setKeys(zoomKeys);

The action still accepts both ``+`` and ``]``. The footer displays only
``[+] zoom in`` while a future detailed help view can show the
alternative too.

Capturing Text Input
--------------------

Text-entry controls should override ``onKeyPress()`` and mark ordinary
text keys as handled. Page actions such as ``quit`` then do not run
while the focused control is editing text.

.. code-block:: cpp

    void SearchField::onKeyPress(ui::KeyPressEvent &event) noexcept {
        if (event.key() == Key::Enter) {
            submit();
            event.setHandled();
            return;
        }
        if (event.key() == Key::Escape) {
            cancel();
            event.setHandled();
            return;
        }
        if (event.key().type() == Key::Character || event.key().type() == Key::Combined) {
            insertText(event.key().combined());
            event.setHandled();
            return;
        }
        Surface::onKeyPress(event);
    }

Multi-Key Actions
-----------------

When several keys perform related work, use one action and branch on the
trigger key in the callback. This keeps dispatch simple and keeps the
footer help focused on the one command the reader can perform.

.. code-block:: cpp

    auto panAction = ui::Action::create("pan");
    panAction->setKeys({Key::Left, Key::Up, Key::Down, Key::Right});
    panAction->setTriggerFn([view](const ui::ActionTriggerContext &context) {
        switch (context.key().type()) {
        case Key::Left:
            view->panLeft();
            break;
        case Key::Right:
            view->panRight();
            break;
        case Key::Up:
            view->panUp();
            break;
        case Key::Down:
            view->panDown();
            break;
        default:
            break;
        }
    });

    mapPage->actions().add(panAction);

Footer Help
-----------

:cpp:any:`ActionHelp <erbsland::cterm::ui::surface::ActionHelp>` renders
the currently available actions for the focused surface chain.
:cpp:any:`FooterLine <erbsland::cterm::ui::surface::FooterLine>` embeds
it on the right side and leaves a dynamic text area on the left.

.. code-block:: cpp

    auto footer = ui::FooterLine::create();
    footer->setText("Ready");
    rootStack->addSurface(footer);

The help surface deduplicates shared actions, skips disabled actions and
actions hidden from the footer, sorts by help priority, and shows as many
items as fit. Formatting such as key brackets, separators, and the
omitted-action marker comes from the active theme.

Reference
=========

.. doxygenclass:: erbsland::cterm::ui::Action
    :members:

.. doxygenclass:: erbsland::cterm::ui::ButtonAction
    :members:

.. doxygenclass:: erbsland::cterm::ui::HelpData
    :members:

.. doxygenenum:: erbsland::cterm::ui::HelpVisibility

.. doxygenclass:: erbsland::cterm::ui::Actions
    :members:

.. doxygenclass:: erbsland::cterm::ui::ActionTriggerContext
    :members:
