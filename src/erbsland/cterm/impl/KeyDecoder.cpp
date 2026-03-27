// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "KeyDecoder.hpp"

#include "UnicodeWidth.hpp"

#include <optional>


namespace erbsland::cterm::impl {


auto KeyDecoder::consoleSequenceDefinitions() noexcept -> const std::array<ConsoleSequenceDefinition, 32> & {
    static constexpr auto cConsoleSequenceDefinitions = std::array<ConsoleSequenceDefinition, 32>{{
        {"\n", Key::Enter},       {"\r", Key::Enter},       {"\t", Key::Tab},         {" ", Key::Space},
        {"\x1b", Key::Escape},    {"\x08", Key::Backspace}, {"\x7f", Key::Backspace}, {"\x1b[Z", Key::BackTab},
        {"\x1b[A", Key::Up},      {"\x1b[B", Key::Down},    {"\x1b[C", Key::Right},   {"\x1b[D", Key::Left},
        {"\x1b[H", Key::Home},    {"\x1b[F", Key::End},     {"\x1bOH", Key::Home},    {"\x1bOF", Key::End},
        {"\x1b[2~", Key::Insert}, {"\x1b[3~", Key::Delete}, {"\x1b[5~", Key::PageUp}, {"\x1b[6~", Key::PageDown},
        {"\x1bOP", Key::F1},      {"\x1bOQ", Key::F2},      {"\x1bOR", Key::F3},      {"\x1bOS", Key::F4},
        {"\x1b[15~", Key::F5},    {"\x1b[17~", Key::F6},    {"\x1b[18~", Key::F7},    {"\x1b[19~", Key::F8},
        {"\x1b[20~", Key::F9},    {"\x1b[21~", Key::F10},   {"\x1b[23~", Key::F11},   {"\x1b[24~", Key::F12},
    }};
    return cConsoleSequenceDefinitions;
}

auto KeyDecoder::createCharacterKey(const CombinedChar &character) noexcept -> Key {
    if (character.codePointCount() <= 1) {
        return Key{Key::Character, character.mainCodePoint()};
    }
    return Key{Key::Combined, character.utf32()};
}

auto KeyDecoder::parseUtf8CodePointPrefix(const std::size_t offset) const noexcept -> U8ParseResult {
    return U8Buffer{_text}.parseCodePointPrefix(offset);
}

auto KeyDecoder::parseConsoleInputPrefix() const noexcept -> ParseResult {
    if (_text.empty()) {
        return ParseResult{U8ParseStatus::Invalid, 0};
    }
    auto matchedSequence = std::optional<ConsoleSequenceDefinition>{};
    auto hasLongerPrefix = false;
    for (const auto &definition : consoleSequenceDefinitions()) {
        if (_text.starts_with(definition.sequence)) {
            if (!matchedSequence.has_value() || definition.sequence.size() > matchedSequence->sequence.size()) {
                matchedSequence = definition;
            }
        }
        if (definition.sequence.starts_with(_text) && definition.sequence.size() > _text.size()) {
            hasLongerPrefix = true;
        }
    }
    if (matchedSequence.has_value()) {
        return ParseResult{U8ParseStatus::Parsed, Key{matchedSequence->type}, matchedSequence->sequence.size()};
    }
    if (_text[0] == '\x1b' && hasLongerPrefix) {
        return ParseResult{U8ParseStatus::NeedMoreData, 0};
    }
    if (_text[0] == '\x1b' && !hasLongerPrefix) {
        return ParseResult{U8ParseStatus::Invalid, _text.size()};
    }

    const auto firstCodePoint = parseUtf8CodePointPrefix(0);
    if (firstCodePoint.status() != U8ParseStatus::Parsed) {
        return ParseResult{firstCodePoint.status(), firstCodePoint.consumedByteCount()};
    }
    const auto baseCodePoint = firstCodePoint.codePoint();
    if (CombinedChar::isControlCode(baseCodePoint)) {
        return ParseResult{U8ParseStatus::Invalid, firstCodePoint.consumedByteCount()};
    }
    if (consoleCharacterWidth(baseCodePoint) == 0) {
        return ParseResult{U8ParseStatus::Invalid, firstCodePoint.consumedByteCount()};
    }

    auto character = CombinedChar{baseCodePoint};
    auto offset = firstCodePoint.consumedByteCount();
    while (offset < _text.size()) {
        const auto nextCodePoint = parseUtf8CodePointPrefix(offset);
        if (nextCodePoint.status() == U8ParseStatus::NeedMoreData) {
            return ParseResult{U8ParseStatus::NeedMoreData, 0};
        }
        if (nextCodePoint.status() != U8ParseStatus::Parsed) {
            return ParseResult{U8ParseStatus::Parsed, createCharacterKey(character), offset};
        }
        const auto codePoint = nextCodePoint.codePoint();
        if (CombinedChar::isControlCode(codePoint)) {
            return ParseResult{U8ParseStatus::Parsed, createCharacterKey(character), offset};
        }
        if (consoleCharacterWidth(codePoint) != 0) {
            return ParseResult{U8ParseStatus::Parsed, createCharacterKey(character), offset};
        }
        character = character.withCombining(codePoint);
        offset = nextCodePoint.consumedByteCount();
    }
    return ParseResult{U8ParseStatus::Parsed, createCharacterKey(character), offset};
}

auto KeyDecoder::decodeConsoleInput() const noexcept -> Key {
    const auto result = parseConsoleInputPrefix();
    if (result.status() == U8ParseStatus::Parsed && result.consumedByteCount() == _text.size()) {
        return result.key();
    }
    return {};
}


}
