..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

******************
Rich Text and HTML
******************

The :cpp:any:`erbsland::cterm::text` helpers let you render a focused
subset of HTML as styled terminal output. They are designed to give you
predictable, document-like formatting without the complexity of a full
HTML engine.

The system is structured into three layers:

* :cpp:any:`HtmlRenderer <erbsland::cterm::text::HtmlRenderer>` parses
  HTML fragments or full documents and renders them either into a
  :cpp:any:`String <erbsland::cterm::String>` or directly to a
  :cpp:any:`CursorWriter <erbsland::cterm::CursorWriter>`.
* :cpp:any:`Style <erbsland::cterm::text::Style>` defines the visual
  appearance of headings, paragraphs, lists, blockquotes, code blocks,
  and inline elements such as emphasis or links.
* :cpp:any:`TextNode <erbsland::cterm::text::TextNode>` represents the
  intermediate document tree. You can inspect or construct this tree
  when you need more control than direct rendering provides.

If you only need plain terminal text, see :doc:`text`. For details on
paragraph wrapping and layout behavior used by block rendering, refer to
:doc:`paragraph-options`.

Usage
=====

Rendering HTML Fragments to Terminal Text
-----------------------------------------

Use :cpp:any:`HtmlRenderer <erbsland::cterm::text::HtmlRenderer>` when
you want to convert HTML into terminal-friendly output.

The renderer supports a practical subset of HTML, including:

* paragraphs and headings
* links
* unordered and ordered lists
* definition lists
* blockquotes
* code blocks
* horizontal rules
* common inline formatting (``<strong>``, ``<em>``, etc.)

.. code-block:: cpp

    using namespace erbsland::cterm;
    namespace rich = erbsland::cterm::text;

    const auto html = std::string_view{
        "<h2>Status</h2>"
        "<p>Hello <strong>world</strong>.</p>"
        "<ul><li>One</li><li>Two</li></ul>"
        "<p><a href=\"/docs\">Documentation</a></p>"};

    const auto summary = rich::HtmlRenderer{html}.renderString();
    terminal.print(summary);

Use ``renderString()`` when you want a self-contained result that you can
store, combine, or print later.

Use ``renderTo()`` when you are writing directly to a
:cpp:any:`CursorWriter <erbsland::cterm::CursorWriter>` and want to
preserve block spacing, margins, and cursor positioning across multiple
render operations.

Customizing Heading, List, and Code Styles
------------------------------------------

With :cpp:any:`Style <erbsland::cterm::text::Style>`, you define how your
rendered content looks through selector-based rules. Rules can target a
semantic role, an optional heading or list level, a list kind, and
named style tokens parsed from ``TextNode::style()``.

.. code-block:: cpp

    namespace rich = erbsland::cterm::text;

    auto style = std::make_shared<rich::Style>();
    style->setBaseTextStyle(CharStyle{Color{fg::BrightWhite, bg::Inherited}});
    style->edit(rich::StyleSelector::strong()).setTextStyle(CharStyle{Color{fg::BrightYellow, bg::Inherited}});
    style->edit(rich::StyleSelector::code()).setTextStyle(CharStyle{Color{fg::BrightCyan, bg::Black}});
    style->edit(rich::StyleSelector::heading(1))
        .setTextStyle(CharStyle{Color{fg::BrightWhite, bg::Blue}})
        .setSuffix(U"  ")
        .setLineFill(U'=');
    style->edit(rich::StyleSelector::listItem(rich::StyleListKind::Bullet, 0))
        .setLiteralMarker(U"• ", CharStyle{Color{fg::BrightMagenta, bg::Inherited}});

    rich::HtmlRenderer{
        "<h1>Title</h1><ul><li>Entry</li></ul>",
        style}.renderTo(buffer);

Block-level layout is controlled via
:cpp:any:`ParagraphIndents <erbsland::cterm::ParagraphIndents>`. The
renderer automatically collapses adjacent vertical margins where it
makes sense, so headings and paragraphs behave like cohesive document
blocks rather than isolated ``printParagraph()`` calls.

Inspecting and Building TextNode Trees
--------------------------------------

:cpp:any:`TextNode <erbsland::cterm::text::TextNode>` gives you access to
the parsed document structure.

This is useful when you want to:

* debug or inspect parsed HTML
* transform content before rendering
* build structured terminal output programmatically

.. code-block:: cpp

    namespace rich = erbsland::cterm::text;

    auto document = rich::TextNode::createDocument();
    auto paragraph = rich::TextNode::createParagraph();
    auto link = rich::TextNode::createLink("/docs");

    link->addChild(rich::TextNode::createText(U"Open docs"));
    paragraph->addChild(link);
    document->addChild(paragraph);

    for (const auto &line : document->toDiagnosticTree(2)) {
        terminal.printLine(line);
    }

When the HTML parser encounters unsupported block-level elements (for
example tables or SVG), it inserts
:cpp:any:`TextNode::Type::Unsupported <erbsland::cterm::text::TextNode::Type::Unsupported>`
nodes instead of silently dropping content. This makes missing support
explicit and easier to handle in your application logic.

Interface
=========

.. doxygenclass:: erbsland::cterm::text::HtmlRenderer
    :members:

.. doxygentypedef:: erbsland::cterm::text::StylePtr

.. doxygentypedef:: erbsland::cterm::text::StyleConstPtr

.. doxygenclass:: erbsland::cterm::text::Style
    :members:

.. doxygentypedef:: erbsland::cterm::text::TextNodePtr

.. doxygentypedef:: erbsland::cterm::text::TextNodeConstPtr

.. doxygentypedef:: erbsland::cterm::text::TextNodeWeakPtr

.. doxygenclass:: erbsland::cterm::text::TextNode
    :members:
