// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../LayoutData.hpp"

#include "../../String.hpp"

#include <utility>

namespace erbsland::cterm::ui::layout {

/// Metadata for one child section.
class SectionOptions final : public LayoutData {
public:
    /// Create empty section options.
    SectionOptions() = default;
    /// Create section options with text for the title line.
    /// @param title The optional section title.
    /// @param rightText The optional right-side text.
    SectionOptions(String title, String rightText = {}) noexcept :
        _title{std::move(title)}, _rightText{std::move(rightText)} {}
    ~SectionOptions() = default;

public: // accessors
    /// Access the section title.
    [[nodiscard]] auto title() const noexcept -> const String & { return _title; }
    /// Replace the section title.
    void setTitle(String title) noexcept { _title = std::move(title); }
    /// Access the right-side text.
    [[nodiscard]] auto rightText() const noexcept -> const String & { return _rightText; }
    /// Replace the right-side text.
    void setRightText(String rightText) noexcept { _rightText = std::move(rightText); }

private:
    String _title;     ///< Optional section title.
    String _rightText; ///< Optional right-side text.
};

}
