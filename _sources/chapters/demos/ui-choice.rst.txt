..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

UI Choice
=========

``ui-choice`` demonstrates the composed modal choice page.

Run it from the build directory with:

.. code-block:: console

    $ ./demo-apps/ui-choice

The main page uses regular layout surfaces and opens a
:cpp:any:`Choice <erbsland::cterm::ui::page::Choice>` dialog. The dialog
is built from :cpp:any:`Centered <erbsland::cterm::ui::layout::Centered>`,
:cpp:any:`Frame <erbsland::cterm::ui::layout::Frame>`,
:cpp:any:`Stack <erbsland::cterm::ui::layout::Stack>`,
:cpp:any:`TextBox <erbsland::cterm::ui::surface::TextBox>`, and
:cpp:any:`Buttons <erbsland::cterm::ui::layout::Buttons>`.
