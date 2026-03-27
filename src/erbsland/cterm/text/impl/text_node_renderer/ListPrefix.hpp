// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include "../../../String.hpp"

#include <utility>


namespace erbsland::cterm::text::impl::text_node_renderer {


/// The rendered prefix for one list item.
class ListPrefix final {
public:
    /// Create one rendered list prefix.
    /// @param text The rendered bullet or number prefix.
    /// @param terminalText The paragraph prefix used for terminal output.
    /// @param width The visible prefix width in terminal cells.
    ListPrefix(String text = {}, String terminalText = {}, const int width = 0) :
        _text{std::move(text)}, _terminalText{std::move(terminalText)}, _width{width} {}

public:
    /// Access the rendered bullet or number prefix.
    [[nodiscard]] auto text() const noexcept -> const String & { return _text; }
    /// Access the paragraph prefix used for terminal output.
    [[nodiscard]] auto terminalText() const noexcept -> const String & { return _terminalText; }
    /// Access the visible prefix width in terminal cells.
    [[nodiscard]] auto width() const noexcept -> int { return _width; }

private:
    String _text;         ///< The rendered bullet or number prefix.
    String _terminalText; ///< The paragraph prefix used for terminal output.
    int _width{0};        ///< The visible prefix width in terminal cells.
};


}
