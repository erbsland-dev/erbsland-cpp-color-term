// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ThemeBuilder.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace erbsland::cterm::theme {

auto ThemeBuilder::registerTag() -> Tag {
    if (_nextTagBit >= 64) {
        throw std::logic_error{"Too many theme tags."};
    }
    const auto result = Tag{_nextTagBit};
    _nextTagBit += 1;
    return result;
}

auto ThemeBuilder::edit(const Selector selector) -> PropertyEditor {
    return PropertyEditor{definitionFor(selector).properties};
}

auto ThemeBuilder::build() const -> ThemeConstPtr {
    return std::make_shared<Theme>(_definitions, _nextTagBit);
}

auto ThemeBuilder::from(const ThemeConstPtr &theme) -> ThemeBuilder {
    auto result = ThemeBuilder{};
    if (theme != nullptr) {
        result._definitions = theme->definitions();
        result._nextTagBit = theme->registeredTagCount();
        for (const auto &[selector, definition] : result._definitions) {
            result._nextOrder = std::max(result._nextOrder, definition.order + 1);
        }
    }
    return result;
}

auto ThemeBuilder::dark() -> ThemeBuilder {
    auto builder = ThemeBuilder{};
    builder.addClassicBlocks();
    builder.edit(Selector{Element::Base}).setStyle(CharStyle::reset());
    builder.edit(Selector{Element::Surface, Part::Background}).setStyle(CharStyle{Color::reset()});
    builder.edit(Selector{Element::StatusLine, Part::Background}).setColor(Color{fg::White, bg::BrightBlack});
    builder.edit(Selector{Element::HeaderLine, Part::Background}).setColor(Color{fg::White, bg::Blue});
    builder.edit(Selector{Element::FooterLine, Part::Background}).setColor(Color{fg::White, bg::BrightBlack});
    builder.edit(Selector{Element::ActionHelp, Part::Text}).setColor(Color{fg::BrightWhite, bg::Inherited});
    builder.edit(Selector{Element::ActionHelp, Part::Key}).setColor(Color{fg::BrightYellow, bg::Inherited});
    builder.edit(Selector{Element::ActionHelp, Part::KeyBracket}).setColor(Color{fg::Black, bg::Inherited});
    builder.edit(Selector{Element::ScrollCorner, Part::Background}).setColor(Color{fg::Black, bg::BrightBlack});
    builder.edit(Selector{Element::HorizontalScrollBar, Part::Track}).setColor(Color{fg::Black, bg::BrightBlack});
    builder.edit(Selector{Element::HorizontalScrollBar, Part::Thumb}).setColor(Color{fg::Black, bg::White});
    builder.edit(Selector{Element::HorizontalScrollBar, Part::Decrease}).setColor(Color{fg::Black, bg::BrightBlack});
    builder.edit(Selector{Element::HorizontalScrollBar, Part::Increase}).setColor(Color{fg::Black, bg::BrightBlack});
    builder.edit(Selector{Element::VerticalScrollBar, Part::Track}).setColor(Color{fg::Black, bg::BrightBlack});
    builder.edit(Selector{Element::VerticalScrollBar, Part::Thumb}).setColor(Color{fg::Black, bg::White});
    builder.edit(Selector{Element::VerticalScrollBar, Part::Decrease}).setColor(Color{fg::Black, bg::BrightBlack});
    builder.edit(Selector{Element::VerticalScrollBar, Part::Increase}).setColor(Color{fg::Black, bg::BrightBlack});
    builder.edit(Selector{Element::Sections, Part::Border}).setColor(Color{fg::BrightBlack, bg::Inherited});
    builder.edit(Selector{Element::Sections, Part::Text}).setColor(Color{fg::BrightWhite, bg::Inherited});
    builder.edit(Selector{Element::Sections, Part::Title}).setColor(Color{fg::BrightWhite, bg::Inherited});
    builder.edit(Selector{Element::Sections, Part::TitleBracket}).setColor(Color{fg::BrightYellow, bg::Inherited});
    builder.edit(Selector{Element::Sections, Part::Border}.requireState(State::FocusWithin))
        .setColor(Color{fg::BrightYellow, bg::Inherited});
    builder.edit(Selector{Element::Button, Part::Border}).setColor(Color{fg::BrightBlue, bg::Blue});
    builder.edit(Selector{Element::Button, Part::Text}).setColor(Color{fg::BrightWhite, bg::Blue});
    builder.edit(Selector{Element::Button, Part::Key}).setColor(Color{fg::BrightYellow, bg::Blue});
    builder.edit(Selector{Element::Button, Part::KeyBracket}).setColor(Color{fg::Black, bg::Blue});
    builder.edit(Selector{Element::Button, Part::Border}.requireState(State::Focused))
        .setColor(Color{fg::BrightYellow, bg::Blue});
    builder.edit(Selector{Element::Button, Part::Text}.requireState(State::Disabled))
        .setColor(Color{fg::BrightBlack, bg::Inherited});
    builder.edit(Selector{Element::Frame, Part::Background}).setColor(Color::reset());
    builder.edit(Selector{Element::Frame, Part::Border}).setColor(Color{fg::BrightWhite, bg::Inherited});
    builder.edit(Selector{Element::Frame, Part::Text}).setColor(Color{fg::BrightWhite, bg::Inherited});
    builder.edit(Selector{Element::Frame, Part::Indicator}).setColor(Color{fg::BrightCyan, bg::Inherited});
    builder.edit(Selector{Element::Choice, Part::Background}).setColor(Color{fg::BrightBlack, bg::Default});
    builder.edit(Selector{Element::Choice, Part::Border}).setColor(Color{fg::BrightCyan, bg::Inherited});
    builder.edit(Selector{Element::Choice, Part::Text}).setColor(Color{fg::BrightWhite, bg::Inherited});
    builder.edit(Selector{Element::Choice, Part::Indicator}).setColor(Color{fg::BrightCyan, bg::Inherited});
    builder.edit(Selector{Element::TextBox, Part::Text}).setColor(Color{fg::BrightWhite, bg::Inherited});
    builder.edit(Selector{Element::HelpViewer, Part::Background}).setColor(Color{fg::BrightWhite, bg::Green});
    return builder;
}

auto ThemeBuilder::light() -> ThemeBuilder {
    auto builder = ThemeBuilder{};
    builder.addClassicBlocks();
    builder.edit(Selector{Element::Base}).setStyle(CharStyle::reset());
    builder.edit(Selector{Element::Surface, Part::Background}).setStyle(CharStyle{Color::reset()});
    builder.edit(Selector{Element::StatusLine, Part::Background}).setColor(Color{fg::Black, bg::White});
    builder.edit(Selector{Element::HeaderLine, Part::Background}).setColor(Color{fg::White, bg::Blue});
    builder.edit(Selector{Element::FooterLine, Part::Background}).setColor(Color{fg::Black, bg::White});
    builder.edit(Selector{Element::ActionHelp, Part::Text}).setColor(Color{fg::Black, bg::Inherited});
    builder.edit(Selector{Element::ActionHelp, Part::Key}).setColor(Color{fg::Blue, bg::Inherited});
    builder.edit(Selector{Element::ActionHelp, Part::KeyBracket}).setColor(Color{fg::BrightBlack, bg::Inherited});
    builder.edit(Selector{Element::ScrollCorner, Part::Background}).setColor(Color{fg::Black, bg::White});
    builder.edit(Selector{Element::HorizontalScrollBar, Part::Track}).setColor(Color{fg::Black, bg::White});
    builder.edit(Selector{Element::HorizontalScrollBar, Part::Thumb}).setColor(Color{fg::White, bg::BrightBlack});
    builder.edit(Selector{Element::VerticalScrollBar, Part::Track}).setColor(Color{fg::Black, bg::White});
    builder.edit(Selector{Element::VerticalScrollBar, Part::Thumb}).setColor(Color{fg::White, bg::BrightBlack});
    builder.edit(Selector{Element::Sections, Part::Border}).setColor(Color{fg::BrightBlack, bg::Inherited});
    builder.edit(Selector{Element::Sections, Part::Text}).setColor(Color{fg::Black, bg::Inherited});
    builder.edit(Selector{Element::Sections, Part::Title}).setColor(Color{fg::Black, bg::Inherited});
    builder.edit(Selector{Element::Sections, Part::TitleBracket}).setColor(Color{fg::Blue, bg::Inherited});
    builder.edit(Selector{Element::Sections, Part::Border}.requireState(State::FocusWithin))
        .setColor(Color{fg::Blue, bg::Inherited});
    builder.edit(Selector{Element::Button, Part::Border}).setColor(Color{fg::BrightBlack, bg::Inherited});
    builder.edit(Selector{Element::Button, Part::Text}).setColor(Color{fg::Black, bg::Inherited});
    builder.edit(Selector{Element::Button, Part::Key}).setColor(Color{fg::Blue, bg::Inherited});
    builder.edit(Selector{Element::Button, Part::KeyBracket}).setColor(Color{fg::BrightBlack, bg::Inherited});
    builder.edit(Selector{Element::Button, Part::Border}.requireState(State::Focused))
        .setColor(Color{fg::Blue, bg::Inherited});
    builder.edit(Selector{Element::Button, Part::Text}.requireState(State::Disabled))
        .setColor(Color{fg::BrightBlack, bg::Inherited});
    builder.edit(Selector{Element::Frame, Part::Background}).setColor(Color::reset());
    builder.edit(Selector{Element::Frame, Part::Border}).setColor(Color{fg::Black, bg::Inherited});
    builder.edit(Selector{Element::Frame, Part::Text}).setColor(Color{fg::Black, bg::Inherited});
    builder.edit(Selector{Element::Frame, Part::Indicator}).setColor(Color{fg::Blue, bg::Inherited});
    builder.edit(Selector{Element::Choice, Part::Background}).setColor(Color::reset());
    builder.edit(Selector{Element::Choice, Part::Border}).setColor(Color{fg::Black, bg::Inherited});
    builder.edit(Selector{Element::Choice, Part::Text}).setColor(Color{fg::Black, bg::Inherited});
    builder.edit(Selector{Element::Choice, Part::Indicator}).setColor(Color{fg::Blue, bg::Inherited});
    return builder;
}

auto ThemeBuilder::monochrome() -> ThemeBuilder {
    auto builder = ThemeBuilder{};
    builder.addPlainBlocks();
    builder.edit(Selector{Element::Base}).setStyle(CharStyle::reset());
    builder.edit(Selector{Element::HeaderLine, Part::Background}).setAttributes(CharAttributes::Bold);
    builder.edit(Selector{Element::ActionHelp, Part::Key}).setAttributes(CharAttributes::Bold);
    builder.edit(Selector{Element::Sections, Part::Title}).setAttributes(CharAttributes::Bold);
    builder.edit(Selector{Element::Sections, Part::TitleBracket}).setAttributes(CharAttributes::Bold);
    builder.edit(Selector{Element::Sections, Part::Border}.requireState(State::FocusWithin))
        .setAttributes(CharAttributes::Bold);
    builder.edit(Selector{Element::Button, Part::Key}).setAttributes(CharAttributes::Bold);
    builder.edit(Selector{Element::Frame, Part::Indicator}).setAttributes(CharAttributes::Bold);
    builder.edit(Selector{Element::Choice, Part::Indicator}).setAttributes(CharAttributes::Bold);
    return builder;
}

auto ThemeBuilder::definitionFor(Selector selector) -> Theme::Definition & {
    if (!selector.element().isValid()) {
        selector = selector.withElement(Element::Base);
    }
    const auto [definitionIt, inserted] = _definitions.try_emplace(selector);
    if (inserted) {
        definitionIt->second.order = _nextOrder;
        _nextOrder += 1;
    }
    return definitionIt->second;
}

void ThemeBuilder::addClassicBlocks() {
    edit(Selector{Element::Base}).setBlock(BlockRole::Background, U' ');
    edit(Selector{Element::FooterLine, Part::Background}).setMargins(Margins{1, 0});
    edit(Selector{Element::ActionHelp, Part::KeyBracket}).setBlocks(U"   [/]          ");
    edit(Selector{Element::HorizontalScrollBar, Part::Track}).setBlocks(U"░░░░░░░░░←░→░░░←").setMargins(Margins{1, 0});
    edit(Selector{Element::HorizontalScrollBar, Part::Thumb}).setBlocks(U"         ▏ ▕    ");
    edit(Selector{Element::HorizontalScrollBar, Part::Decrease}).setBlock(BlockRole::Background, U'←');
    edit(Selector{Element::HorizontalScrollBar, Part::Increase}).setBlock(BlockRole::Background, U'→');
    edit(Selector{Element::VerticalScrollBar, Part::Track}).setBlocks(U"░░░░░░░░░░░░↑░↓↑").setMargins(Margins{0, 1});
    edit(Selector{Element::VerticalScrollBar, Part::Thumb}).setBlocks(U"            ▔ ▁ ");
    edit(Selector{Element::VerticalScrollBar, Part::Decrease}).setBlock(BlockRole::Background, U'↑');
    edit(Selector{Element::VerticalScrollBar, Part::Increase}).setBlock(BlockRole::Background, U'↓');
    edit(Selector{Element::Sections, Part::Border})
        .setBlock(BlockRole::Background, U'─')
        .setMargins(Margins{0, 1, 0, 2});
    edit(Selector{Element::Sections, Part::Title})
        .setBlock(BlockRole::Background, U' ')
        .setMargins(Margins{0, 1, 0, 1});
    edit(Selector{Element::Sections, Part::TitleBracket}).setBlocks(U"         ⟨ ⟩    ");
    edit(Selector{Element::Buttons, Part::Spacing}).setBlock(BlockRole::Background, U' ').setMargins(Margins{1, 0});
    edit(Selector{Element::Button, Part::Background}).setBlock(BlockRole::Background, U' ');
    edit(Selector{Element::Button, Part::Border}).setBlocks(U"         ▌ ▐    ");
    edit(Selector{Element::Button, Part::KeyBracket}).setBlocks(U"         [ ]    ");
    edit(Selector{Element::Frame, Part::Background}).setBlock(BlockRole::Background, U' ');
    edit(Selector{Element::Frame, Part::Border}).setBlocks(U"┌─┐│ │└─┘├─┤┬│┴┼").setMargins(Margins{2});
    edit(Selector{Element::Frame, Part::Indicator}).setBlocks(U"         ⟨ ⟩    ").setMargins(Margins{0, 1});
    edit(Selector{Element::Choice, Part::Text}).setMargins(Margins{0, 0, 1, 0});
    edit(Selector{Element::Choice, Part::Background}).setBlock(BlockRole::Background, U' ');
    edit(Selector{Element::Choice, Part::Border}).setBlocks(U"┌─┐│ │└─┘├─┤┬│┴┼").setMargins(Margins{2});
    edit(Selector{Element::Choice, Part::Indicator}).setBlocks(U"         ⟨ ⟩    ").setMargins(Margins{0, 1});
}

void ThemeBuilder::addPlainBlocks() {
    addClassicBlocks();
    edit(Selector{Element::FooterLine, Part::Background}).setMargins(Margins{});
    edit(Selector{Element::HorizontalScrollBar, Part::Track}).setBlocks(U"                ").setMargins(Margins{});
    edit(Selector{Element::HorizontalScrollBar, Part::Thumb}).setBlocks(U"         ███████");
    edit(Selector{Element::VerticalScrollBar, Part::Track}).setBlocks(U"                ").setMargins(Margins{});
    edit(Selector{Element::VerticalScrollBar, Part::Thumb}).setBlocks(U"         ███████");
    edit(Selector{Element::Sections, Part::TitleBracket}).setBlocks(U"         < >    ");
    edit(Selector{Element::Button, Part::Border}).setBlocks(U"         { ▕}   ");
    edit(Selector{Element::Frame, Part::Indicator}).setBlocks(U"         < >    ");
    edit(Selector{Element::Choice, Part::Text}).setMargins(Margins{0, 0, 1, 0});
    edit(Selector{Element::Choice, Part::Indicator}).setBlocks(U"         < >    ");
}

}
