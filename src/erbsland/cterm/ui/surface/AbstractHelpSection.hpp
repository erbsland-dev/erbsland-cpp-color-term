// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Surface.hpp"

#include <string_view>

namespace erbsland::cterm::ui::surface {

class AbstractHelpSection;
using AbstractHelpSectionPtr = std::shared_ptr<AbstractHelpSection>;

/// A surface that provides one titled section for a help viewer.
class AbstractHelpSection : public Surface {
public:
    /// Create an abstract help section.
    explicit AbstractHelpSection(ProtectedTag) noexcept;
    ~AbstractHelpSection() override = default;

public:
    /// Access the section title.
    /// @return The title displayed by the help viewer.
    [[nodiscard]] virtual auto title() const noexcept -> std::string_view = 0;

public:
    /// Scroll one or more rows up.
    virtual void scrollUp(Coordinate count = 1) noexcept;
    /// Scroll one or more rows down.
    virtual void scrollDown(Coordinate count = 1) noexcept;
    /// Scroll one page up.
    virtual void pageUp() noexcept;
    /// Scroll one page down.
    virtual void pageDown() noexcept;
    /// Scroll to the first content row.
    virtual void scrollToTop() noexcept;
    /// Scroll to the last content row.
    virtual void scrollToBottom() noexcept;
};

}
