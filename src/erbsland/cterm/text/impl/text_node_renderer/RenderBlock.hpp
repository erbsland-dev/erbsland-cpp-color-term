// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "BlockKind.hpp"

#include "../../../String.hpp"
#include "../../Style.hpp"

#include <algorithm>
#include <optional>
#include <utility>
#include <vector>

namespace erbsland::cterm::text::impl::text_node_renderer {

/// A fully planned render block shared by string and terminal output.
class RenderBlock final {
public:
    /// Create one paragraph-like render block.
    /// @param kind The physical block kind to emit.
    /// @param text The styled text payload for the block.
    /// @param style The resolved block rule, including inherited margins.
    RenderBlock(const BlockKind kind, String text, StyleRule style) :
        _kind{kind}, _text{std::move(text)}, _style{std::move(style)} {}

    /// Create one filled paragraph-like render block.
    /// @param kind The physical block kind to emit.
    /// @param text The styled text payload for the block.
    /// @param style The resolved block rule, including inherited margins.
    /// @param fillCharacter The fill character used for the line.
    RenderBlock(const BlockKind kind, String text, StyleRule style, Char fillCharacter) :
        _kind{kind}, _text{std::move(text)}, _style{std::move(style)}, _fillCharacter{fillCharacter} {}

    /// Create one horizontal rule render block.
    /// @param leadingText Optional text anchored at the start of the rule.
    /// @param trailingText Optional text anchored at the end of the rule.
    /// @param style The resolved block rule, including inherited margins.
    /// @param fillCharacter The fill character used for the rule body.
    RenderBlock(
        std::optional<String> leadingText, std::optional<String> trailingText, StyleRule style, Char fillCharacter) :
        _kind{BlockKind::HorizontalRule},
        _leadingText{std::move(leadingText)},
        _trailingText{std::move(trailingText)},
        _style{std::move(style)},
        _fillCharacter{fillCharacter} {}

    RenderBlock() = default;

public:
    /// Access the concrete block kind to render.
    [[nodiscard]] auto kind() const noexcept -> BlockKind { return _kind; }
    /// Access the styled text payload for paragraph and filled-line blocks.
    [[nodiscard]] auto text() const noexcept -> const String & { return _text; }
    /// Access optional text anchored at the start of a horizontal rule.
    [[nodiscard]] auto leadingText() const noexcept -> const std::optional<String> & { return _leadingText; }
    /// Access optional text anchored at the end of a horizontal rule.
    [[nodiscard]] auto trailingText() const noexcept -> const std::optional<String> & { return _trailingText; }
    /// Access the optional paragraph text override used only by `renderTo()`.
    [[nodiscard]] auto terminalText() const noexcept -> const std::optional<String> & { return _terminalText; }
    /// Access the resolved block rule, including inherited margins.
    [[nodiscard]] auto style() const noexcept -> const StyleRule & { return _style; }
    /// Access the resolved block rule, including inherited margins.
    [[nodiscard]] auto style() noexcept -> StyleRule & { return _style; }
    /// Access the optional fill character for horizontal rules and filled headings.
    [[nodiscard]] auto fillCharacter() const noexcept -> const std::optional<Char> & { return _fillCharacter; }
    /// Access the additional plain-text indentation used only by `renderString()`.
    [[nodiscard]] auto stringFirstLineIndent() const noexcept -> int { return _stringFirstLineIndent; }
    /// Access the additional plain-text indentation for wrapped or continuation lines.
    [[nodiscard]] auto stringWrappedLineIndent() const noexcept -> int { return _stringWrappedLineIndent; }

    /// Replace the styled text payload for paragraph and filled-line blocks.
    /// @param text The new styled text payload.
    void setText(String text) { _text = std::move(text); }
    /// Replace the paragraph text override used only by `renderTo()`.
    /// @param terminalText The new terminal text override.
    void setTerminalText(String terminalText) { _terminalText = std::move(terminalText); }
    /// Increase the plain-text first-line indentation.
    /// @param indent The additional indentation to add.
    void addStringFirstLineIndent(const int indent) noexcept { _stringFirstLineIndent += indent; }
    /// Increase the plain-text wrapped-line indentation.
    /// @param indent The additional indentation to add.
    void addStringWrappedLineIndent(const int indent) noexcept { _stringWrappedLineIndent += indent; }

    /// Render this block into plain text.
    /// @return The plain-text representation of the block.
    [[nodiscard]] auto renderString() const -> String;

private:
    BlockKind _kind{BlockKind::Paragraph}; ///< The concrete block kind to render.
    String _text;                          ///< The styled text payload for paragraph and filled-line blocks.
    std::optional<String> _leadingText;    ///< Optional text anchored at the start of a horizontal rule.
    std::optional<String> _trailingText;   ///< Optional text anchored at the end of a horizontal rule.
    std::optional<String> _terminalText;   ///< Optional paragraph text override used only by `renderTo()`.
    StyleRule _style;                      ///< The resolved block rule, including inherited margins.
    std::optional<Char> _fillCharacter;    ///< Optional fill character for horizontal rules and headings.
    int _stringFirstLineIndent{0};         ///< Additional plain-text indentation used only by `renderString()`.
    int _stringWrappedLineIndent{0};       ///< Additional plain-text indentation for wrapped lines.
};

/// The planned block list used by all output paths.
using RenderBlocks = std::vector<RenderBlock>;

}
