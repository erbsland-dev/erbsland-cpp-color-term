// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "PropertyEditor.hpp"

#include <stdexcept>
#include <utility>

namespace erbsland::cterm::theme {

auto PropertyEditor::setColor(const Color color) noexcept -> PropertyEditor & {
    _properties->setColor(color);
    return *this;
}

auto PropertyEditor::setColorSequence(ColorSequence colorSequence) noexcept -> PropertyEditor & {
    _properties->setColorSequence(std::move(colorSequence));
    return *this;
}

auto PropertyEditor::setAttributes(const CharAttributes attributes) noexcept -> PropertyEditor & {
    _properties->setAttributes(attributes);
    return *this;
}

auto PropertyEditor::setStyle(const CharStyle style) noexcept -> PropertyEditor & {
    _properties->setColor(style.color());
    _properties->setAttributes(style.attributes());
    return *this;
}

auto PropertyEditor::setBlock(const BlockRole role, const char32_t codePoint) noexcept -> PropertyEditor & {
    _properties->setBlock(role, codePoint);
    return *this;
}

auto PropertyEditor::setBlocks(const std::u32string_view blocks) -> PropertyEditor & {
    if (blocks.size() != 9 && blocks.size() != 16) {
        throw std::invalid_argument{"Theme block tables require exactly 9 or 16 code points."};
    }
    auto blockTable = Properties::Blocks{};
    for (auto index = std::size_t{0}; index < blocks.size(); ++index) {
        blockTable[index] = blocks[index];
    }
    if (blocks.size() == 9) {
        blockTable[static_cast<std::size_t>(BlockRole::HorizontalWest)] = blocks[0];
        blockTable[static_cast<std::size_t>(BlockRole::HorizontalCenter)] = blocks[1];
        blockTable[static_cast<std::size_t>(BlockRole::HorizontalEast)] = blocks[2];
        blockTable[static_cast<std::size_t>(BlockRole::VerticalNorth)] = blocks[0];
        blockTable[static_cast<std::size_t>(BlockRole::VerticalCenter)] = blocks[3];
        blockTable[static_cast<std::size_t>(BlockRole::VerticalSouth)] = blocks[6];
        blockTable[static_cast<std::size_t>(BlockRole::Single)] = blocks[0];
    }
    _properties->setBlocks(blockTable);
    return *this;
}

auto PropertyEditor::setMargins(const Margins margins) noexcept -> PropertyEditor & {
    _properties->setMargins(margins);
    return *this;
}

auto PropertyEditor::setPadding(const Margins padding) noexcept -> PropertyEditor & {
    _properties->setPadding(padding);
    return *this;
}

}
