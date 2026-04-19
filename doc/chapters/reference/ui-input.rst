..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

********
UI Input
********

:cpp:any:`KeyBindings <erbsland::cterm::ui::KeyBindings>` map key input
to actions on pages and surfaces. Bindings are a lightweight way to add
behavior without subclassing every surface just to handle a few keys.

Usage
=====

Binding Special Keys and Characters
-----------------------------------

The canonical binding API accepts :cpp:any:`Key <erbsland::cterm::Key>`,
and convenience overloads let you bind special keys, Unicode
characters, or multi-character input directly.

.. code-block:: cpp

    page->keyBindings().bind(Key::Escape, [] {
        ui::getApplication().quit();
    });
    page->keyBindings().bind(U'q', [] {
        ui::getApplication().quit();
    });
    page->keyBindings().bind(U"gg", [documentView] {
        documentView->scrollToTop();
    });

``U"gg"`` binds a combined text input key. One code point becomes
``Key::Character`` internally, and longer text becomes
``Key::Combined``.

The :cpp:any:`Key::Type <erbsland::cterm::Key::Type>` overload is only
for real special keys such as ``Escape`` or ``PageDown``. The binding
API rejects ``None``, ``Character``, and ``Combined`` explicitly so
mistakes are caught early.

Binding Multiple Keys to the Same Action
----------------------------------------

Use initializer-list overloads when several keys should trigger the same
behavior.

.. code-block:: cpp

    page->keyBindings().bind({Key::Left, Key::Right}, [status] {
        status->setPaintOutdated();
    });
    page->keyBindings().bind({U'j', U'k'}, [status] {
        status->setPaintOutdated();
    });
    page->keyBindings().bind(
        {Key{Key::Up}, Key{Key::Character, U'k'}},
        [view] { view->scrollUp(); });

For mixed key kinds, use ``std::initializer_list<Key>`` so the intent is
explicit.

Removing Bindings
-----------------

Passing an empty action removes the affected bindings.

.. code-block:: cpp

    auto &bindings = page->keyBindings();
    bindings.bind(Key::Escape, {});
    bindings.bind({U'j', U'k'}, {});

Reference
=========

.. doxygenclass:: erbsland::cterm::ui::KeyBindings
    :members:
