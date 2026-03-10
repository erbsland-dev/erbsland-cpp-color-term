..
    Copyright (c) 2026 Tobias Erbsland - Erbsland DEV. https://erbsland.dev
    SPDX-License-Identifier: Apache-2.0

************************
Add Input and Animation
************************

In this final step, the static dashboard becomes a small interactive
application. The program switches the input backend into key-reading
mode, redraws the screen continuously, reacts to terminal resize
events, and updates the animated title on every frame.

Replace :file:`signal-board/src/main.cpp`
=========================================

Replace the source file with the following final tutorial version:

.. code-block:: cpp
    :caption: <project>/signal-board/src/main.cpp

    #include <array>
    #include <chrono>
    #include <erbsland/cterm/all.hpp>

    using namespace erbsland::cterm;

    auto buildFooter() -> String {
        auto footer = String{};
        for (const auto &character : String{"[Q]"}) {
            footer.append(character.withColorOverlay({fg::BrightYellow, bg::BrightBlack}));
        }
        for (const auto &character : String{" quit  [C] recolor"}) {
            footer.append(character.withColorOverlay({fg::BrightWhite, bg::BrightBlack}));
        }
        return footer;
    }

    auto main() -> int {
        auto terminal = Terminal{Size{96, 28}};
        terminal.initializeScreen();
        terminal.setRefreshMode(Terminal::RefreshMode::Overwrite);
        terminal.input().setMode(Input::Mode::Key);

        auto paletteIndex = std::size_t{0};
        auto animationCycle = std::size_t{0};
        auto quitRequested = false;
        auto palettes = std::array{
            ColorSequence{
                Color{fg::BrightBlue, bg::Black},
                Color{fg::BrightCyan, bg::Black},
                Color{fg::BrightMagenta, bg::Black},
                Color{fg::BrightYellow, bg::Black},
            },
            ColorSequence{
                Color{fg::BrightRed, bg::Black},
                Color{fg::BrightYellow, bg::Black},
                Color{fg::BrightGreen, bg::Black},
                Color{fg::BrightWhite, bg::Black},
            },
        };

        while (!quitRequested) {
            terminal.testScreenSize();
            if (const auto key = terminal.input().read(std::chrono::milliseconds{90}); key.valid()) {
                if (key == Key{Key::Character, 'q'}) {
                    quitRequested = true;
                } else if (key == Key{Key::Character, 'c'}) {
                    paletteIndex = (paletteIndex + 1) % palettes.size();
                }
            }

            auto buffer = Buffer{terminal.size() - Size{1, 1}};
            buffer.fill(Char{" ", {fg::BrightWhite, bg::Black}});

            const auto canvas = Rectangle{0, 0, buffer.size().width(), buffer.size().height()};
            const auto titleRect = canvas.subRectangle(Anchor::TopCenter, Size{0, 6}, Margins{1, 2, 0, 2});
            const auto contentRect = canvas.insetBy(Margins{7, 2, 2, 2});
            const auto footerRect = canvas.subRectangle(Anchor::BottomCenter, Size{0, 1}, Margins{0, 0, 0, 0});

            auto title = Text{String{"SIGNAL BOARD"}, titleRect, Alignment::Center};
            title.setFont(Font::defaultAscii());
            title.setColorSequence(palettes[paletteIndex]);
            title.setAnimation(TextAnimation::ColorDiagonal);
            buffer.drawText(title, animationCycle);

            buffer.drawFilledFrame(
                contentRect,
                FrameStyle::LightWithRoundedCorners,
                Char{" ", bg::BrightBlack});
            buffer.drawText(
                "Resize the terminal, press C to switch the title colors, or Q to quit.",
                contentRect.insetBy(Margins{2, 2, 2, 2}),
                Alignment::Center,
                Color{fg::BrightWhite, bg::BrightBlack});

            buffer.fill(footerRect, Char{" ", bg::BrightBlack});
            buffer.drawText(Text{buildFooter(), footerRect, Alignment::CenterLeft});

            terminal.updateScreen(buffer);
            terminal.flush();
            ++animationCycle;
        }

        terminal.restoreScreen();
        return 0;
    }

What This Final Version Adds
============================

This version introduces the core workflow for interactive terminal
applications:

* ``Input::Mode::Key`` switches from line-based input to immediate
  key handling.
* ``read(timeout)`` allows the redraw loop to remain responsive
  without busy waiting.
* ``testScreenSize()`` updates the detected terminal size between frames.
* ``RefreshMode::Overwrite`` redraws the screen cleanly without leaving
  old content behind.
* ``String`` is useful for constructing colored prompts or status bars
  character by character.

With these pieces in place, you can already build many interactive
terminal dashboards, games, and command-line tools.

.. button-ref:: 08-the-next-steps
    :ref-type: doc
    :color: success
    :align: center
    :expand:
    :class: sd-fs-5 sd-font-weight-bold sd-p-2 sd-my-4

    Continue with the Next Steps →
