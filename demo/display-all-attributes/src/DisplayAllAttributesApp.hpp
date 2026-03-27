// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "TerminalApplication.hpp"

#include <array>
#include <string>
#include <string_view>


namespace demo::display_all_attributes {


using namespace erbsland::cterm;


/// Display the ANSI character attributes supported by the current terminal setup.
class DisplayAllAttributesApp final : public TerminalApplication {
public:
    /// Render the attribute overview once and exit the demo.
    auto beforeRun() -> int override;

private:
    /// One attribute row in the demo output.
    struct AttributeSpec {
        std::string_view name;       ///< The display name for the row.
        CharAttributes::Flag flag;   ///< The represented attribute.
        std::string_view sgrCodes;   ///< The related ANSI SGR codes.
        std::string_view note;       ///< A short note for the row.
        std::string_view sampleText; ///< The sample text rendered with the attribute.
    };

private:
    [[nodiscard]] static auto attributeSpecs() -> const std::array<AttributeSpec, 8> &;
    static void printHeader(Terminal &terminal) noexcept;
    static void printAttributeTable(Terminal &terminal) noexcept;
    static void printAttributeRow(Terminal &terminal, const AttributeSpec &spec, bool supported) noexcept;
    static void printCombinations(Terminal &terminal) noexcept;
    static void printPausePrompt(Terminal &terminal) noexcept;
    [[nodiscard]] static auto padded(std::string_view text, std::size_t width) -> std::string;
    [[nodiscard]] static auto supportLabel(bool supported) -> std::string_view;
    [[nodiscard]] static auto sampleAttributes(CharAttributes::Flag flag) -> CharAttributes;
};


}
