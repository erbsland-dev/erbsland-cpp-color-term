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
    builder.edit(Selector{Element::StatusLine, Part::Background}).setColor(fg::White, bg::BrightBlack);
    builder.edit(Selector{Element::HeaderLine, Part::Background}).setColor(fg::White, bg::Blue);
    builder.edit(Selector{Element::FooterLine, Part::Background}).setColor(fg::White, bg::BrightBlack);
    builder.edit(Selector{Element::ActionHelp, Part::ActionName}).setColor(fg::BrightWhite);
    builder.edit(Selector{Element::ActionHelp, Part::Key}).setColor(fg::BrightYellow);
    builder.edit(Selector{Element::ActionHelp, Part::KeyBracket}).setColor(fg::Black);
    builder.edit(Selector{Element::ScrollCorner, Part::Background}).setColor(fg::Black, bg::BrightBlack);
    builder.edit(Selector{Element::HorizontalScrollBar, Part::Track}).setColor(fg::Black, bg::BrightBlack);
    builder.edit(Selector{Element::HorizontalScrollBar, Part::Thumb}).setColor(fg::Black, bg::White);
    builder.edit(Selector{Element::HorizontalScrollBar, Part::Decrease}).setColor(fg::Black, bg::BrightBlack);
    builder.edit(Selector{Element::HorizontalScrollBar, Part::Increase}).setColor(fg::Black, bg::BrightBlack);
    builder.edit(Selector{Element::VerticalScrollBar, Part::Track}).setColor(fg::Black, bg::BrightBlack);
    builder.edit(Selector{Element::VerticalScrollBar, Part::Thumb}).setColor(fg::Black, bg::White);
    builder.edit(Selector{Element::VerticalScrollBar, Part::Decrease}).setColor(fg::Black, bg::BrightBlack);
    builder.edit(Selector{Element::VerticalScrollBar, Part::Increase}).setColor(fg::Black, bg::BrightBlack);
    builder.edit(Selector{Element::Sections, Part::Border}).setColor(fg::BrightBlack);
    builder.edit(Selector{Element::Sections, Part::Text}).setColor(fg::BrightWhite);
    builder.edit(Selector{Element::Sections, Part::Title}).setColor(fg::BrightWhite);
    builder.edit(Selector{Element::Sections, Part::TitleBracket}).setColor(fg::BrightYellow);
    builder.edit(Selector{Element::Sections, Part::Border}.requireState(State::FocusWithin)).setColor(fg::BrightYellow);
    builder.edit(Selector{Element::Button, Part::Border}).setColor(fg::BrightBlue, bg::Blue);
    builder.edit(Selector{Element::Button, Part::Text}).setColor(fg::BrightWhite, bg::Blue);
    builder.edit(Selector{Element::Button, Part::Key}).setColor(fg::BrightYellow, bg::Blue);
    builder.edit(Selector{Element::Button, Part::KeyBracket}).setColor(fg::Black, bg::Blue);
    builder.edit(Selector{Element::Button, Part::Border}.requireState(State::Focused))
        .setColor(fg::BrightYellow, bg::Blue);
    builder.edit(Selector{Element::Button, Part::Text}.requireState(State::Disabled)).setColor(fg::BrightBlack);
    builder.edit(Selector{Element::Frame, Part::Background}).setColor(Color::reset());
    builder.edit(Selector{Element::Frame, Part::Border}).setColor(fg::White);
    builder.edit(Selector{Element::Frame, Part::Text}).setColor(fg::BrightWhite);
    builder.edit(Selector{Element::Frame, Part::Title}).setColor(fg::BrightWhite);
    builder.edit(Selector{Element::Frame, Part::TitleBracket}).setColor(fg::BrightCyan);
    builder.edit(Selector{Element::Choice, Part::Background}).setColor(fg::BrightBlack, bg::Default);
    builder.edit(Selector{Element::Choice, Part::Border}).setColor(fg::BrightCyan);
    builder.edit(Selector{Element::Choice, Part::Text}).setColor(fg::BrightWhite);
    builder.edit(Selector{Element::Choice, Part::Title}).setColor(fg::BrightWhite);
    builder.edit(Selector{Element::Choice, Part::TitleBracket}).setColor(fg::BrightCyan);
    builder.edit(Selector{Element::StaticText, Part::Text}).setColor(fg::BrightWhite);
    builder.edit(Selector{Element::TextBox, Part::Text}).setColor(fg::BrightWhite);
    builder.edit(Selector{Element::HelpViewer, Part::Background}).setColor(fg::BrightWhite, bg::Green);
    return builder;
}

auto ThemeBuilder::light() -> ThemeBuilder {
    auto builder = ThemeBuilder{};
    builder.addClassicBlocks();
    builder.edit(Selector{Element::StatusLine, Part::Background}).setColor(fg::Black, bg::White);
    builder.edit(Selector{Element::HeaderLine, Part::Background}).setColor(fg::White, bg::Blue);
    builder.edit(Selector{Element::FooterLine, Part::Background}).setColor(fg::Black, bg::White);
    builder.edit(Selector{Element::ActionHelp, Part::ActionName}).setColor(fg::Black);
    builder.edit(Selector{Element::ActionHelp, Part::Key}).setColor(fg::Blue);
    builder.edit(Selector{Element::ActionHelp, Part::KeyBracket}).setColor(fg::BrightBlack);
    builder.edit(Selector{Element::ScrollCorner, Part::Background}).setColor(fg::Black, bg::White);
    builder.edit(Selector{Element::HorizontalScrollBar, Part::Track}).setColor(fg::Black, bg::White);
    builder.edit(Selector{Element::HorizontalScrollBar, Part::Thumb}).setColor(fg::White, bg::BrightBlack);
    builder.edit(Selector{Element::VerticalScrollBar, Part::Track}).setColor(fg::Black, bg::White);
    builder.edit(Selector{Element::VerticalScrollBar, Part::Thumb}).setColor(fg::White, bg::BrightBlack);
    builder.edit(Selector{Element::Sections, Part::Border}).setColor(fg::BrightBlack);
    builder.edit(Selector{Element::Sections, Part::Text}).setColor(fg::Black);
    builder.edit(Selector{Element::Sections, Part::Title}).setColor(fg::Black);
    builder.edit(Selector{Element::Sections, Part::TitleBracket}).setColor(fg::Blue);
    builder.edit(Selector{Element::Sections, Part::Border}.requireState(State::FocusWithin)).setColor(fg::Blue);
    builder.edit(Selector{Element::Button, Part::Border}).setColor(fg::BrightBlack);
    builder.edit(Selector{Element::Button, Part::Text}).setColor(fg::Black);
    builder.edit(Selector{Element::Button, Part::Key}).setColor(fg::Blue);
    builder.edit(Selector{Element::Button, Part::KeyBracket}).setColor(fg::BrightBlack);
    builder.edit(Selector{Element::Button, Part::Border}.requireState(State::Focused)).setColor(fg::Blue);
    builder.edit(Selector{Element::Button, Part::Text}.requireState(State::Disabled)).setColor(fg::BrightBlack);
    builder.edit(Selector{Element::Frame, Part::Background}).setColor(Color::reset());
    builder.edit(Selector{Element::Frame, Part::Border}).setColor(fg::Black);
    builder.edit(Selector{Element::Frame, Part::Text}).setColor(fg::Black);
    builder.edit(Selector{Element::Frame, Part::Indicator}).setColor(fg::Blue);
    builder.edit(Selector{Element::Choice, Part::Background}).setColor(Color::reset());
    builder.edit(Selector{Element::Choice, Part::Border}).setColor(fg::Black);
    builder.edit(Selector{Element::Choice, Part::Text}).setColor(fg::Black);
    builder.edit(Selector{Element::Choice, Part::Indicator}).setColor(fg::Blue);
    builder.edit(Selector{Element::StaticText, Part::Text}).setColor(fg::Black);
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

auto ThemeBuilder::zero() -> ThemeBuilder {
    return ThemeBuilder{};
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
    edit(Selector{Element::Base, Part::Ellipsis}).setBlock(BlockRole::Main, U'…');
    edit(Selector{Element::Base, Part::KeyBracket}).setBracketBlocks(U'[', U']', U'/');
    edit(Selector{Element::Base, Part::TitleBracket}).setBracketBlocks(U'⟨', U'⟩').setPadding(Margins{1, 0});
    edit(Selector{Element::FooterLine, Part::Text}).setMargins(Margins{1, 0});
    edit(Selector{Element::ActionHelp, Part::ActionName}).setMargins(Margins{0, 2, 0, 1});
    edit(Selector{Element::ActionHelp, Part::Ellipsis}).setBlock(BlockRole::Single, U'…');
    edit(Selector{Element::HorizontalScrollBar, Part::Track}).setBlocks(U"░░░░░░░░░←░→░░░←").setMargins(Margins{1, 0});
    edit(Selector{Element::HorizontalScrollBar, Part::Thumb}).setBlocks(U"         ▏ ▕    ");
    edit(Selector{Element::HorizontalScrollBar, Part::Decrease}).setBlock(BlockRole::Main, U'←');
    edit(Selector{Element::HorizontalScrollBar, Part::Increase}).setBlock(BlockRole::Main, U'→');
    edit(Selector{Element::VerticalScrollBar, Part::Track}).setBlocks(U"░░░░░░░░░░░░↑░↓↑").setMargins(Margins{0, 1});
    edit(Selector{Element::VerticalScrollBar, Part::Thumb}).setBlocks(U"            ▔ ▁ ");
    edit(Selector{Element::VerticalScrollBar, Part::Decrease}).setBlock(BlockRole::Main, U'↑');
    edit(Selector{Element::VerticalScrollBar, Part::Increase}).setBlock(BlockRole::Main, U'↓');
    edit(Selector{Element::Sections, Part::Border}).setBlocks(U'─');
    edit(Selector{Element::Sections, Part::Title}).setPadding(Margins{1, 0}).setMargins(Margins{2, 0});
    edit(Selector{Element::Sections, Part::Text}).setMargins(Margins{2, 0});
    edit(Selector{Element::Buttons, Part::Spacing}).setBlocks(U' ').setMargins(Margins{1, 0});
    edit(Selector{Element::Button, Part::Background}).setBlocks(U' ');
    edit(Selector{Element::Button, Part::Text}).setPadding(Margins{0, 1, 0, 0});
    edit(Selector{Element::Button, Part::Border})
        .setBracketBlocks(U'▌', U'▐')
        .setMargins(Margins{2, 0})
        .setPadding(Margins{2, 0});
    edit(Selector{Element::Frame, Part::Background}).setBlocks(U' ');
    edit(Selector{Element::Frame, Part::Border}).setBlocks(U"┌─┐│ │└─┘├─┤┬│┴┼").setMargins(Margins{2});
    edit(Selector{Element::Frame, Part::Title}).setMargins(Margins{2, 0});
    edit(Selector{Element::Choice, Part::Text}).setMargins(Margins{0, 0, 1, 0});
    edit(Selector{Element::Choice, Part::Background}).setBlocks(U' ');
    edit(Selector{Element::Choice, Part::Border}).setBlocks(U"┌─┐│ │└─┘├─┤┬│┴┼").setMargins(Margins{2});
    edit(Selector{Element::Choice, Part::Title}).setMargins(Margins{2, 0});
}

void ThemeBuilder::addPlainBlocks() {
    addClassicBlocks();
    edit(Selector{Element::Base, Part::TitleBracket}).setBracketBlocks(U'<', U'>');
    edit(Selector{Element::FooterLine, Part::Background}).setMargins(Margins{});
    edit(Selector{Element::HorizontalScrollBar, Part::Track}).setBlocks(U"                ").setMargins(Margins{});
    edit(Selector{Element::HorizontalScrollBar, Part::Thumb}).setBlocks(U"         #######");
    edit(Selector{Element::VerticalScrollBar, Part::Track}).setBlocks(U"                ").setMargins(Margins{});
    edit(Selector{Element::VerticalScrollBar, Part::Thumb}).setBlocks(U"         #######");
    edit(Selector{Element::Button, Part::Border}).setBracketBlocks(U'{', U'}');
    edit(Selector{Element::Frame, Part::Text}).setPadding(Margins{1, 0});
    edit(Selector{Element::Choice, Part::Text}).setMargins(Margins{0, 0, 1, 0});
}

}
