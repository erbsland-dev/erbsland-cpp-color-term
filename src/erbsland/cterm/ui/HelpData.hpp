// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "HelpFormat.hpp"
#include "HelpVisibility.hpp"

#include <string>

namespace erbsland::cterm::ui {

/// Display metadata for a user-interface action.
class HelpData {
public:
    /// Create empty help data.
    HelpData() = default;
    /// Create help data with a short display name.
    /// @param name The short display name.
    explicit HelpData(std::string name);

public: // accessors
    /// Get the short display name.
    [[nodiscard]] auto name() const noexcept -> const std::string & { return _name; }
    /// Set the short display name.
    /// @param name The short display name.
    /// @return This object.
    auto setName(std::string name) -> HelpData &;
    /// Get the detailed help description.
    [[nodiscard]] auto description() const noexcept -> const std::string & { return _description; }
    /// Get the format of the detailed help description.
    [[nodiscard]] auto descriptionFormat() const noexcept -> HelpFormat { return _descriptionFormat; }
    /// Set the detailed help description.
    /// @param description The detailed help description.
    /// @param format The format of the detailed help description.
    /// @return This object.
    auto setDescription(std::string description, HelpFormat format = HelpFormat::Text) -> HelpData &;
    /// Get the display priority.
    [[nodiscard]] auto priority() const noexcept -> int { return _priority; }
    /// Set the display priority.
    /// @param priority The display priority; larger values are displayed first.
    /// @return This object.
    auto setPriority(int priority) noexcept -> HelpData &;
    /// Get the help visibility.
    [[nodiscard]] auto visibility() const noexcept -> HelpVisibility { return _visibility; }
    /// Set the help visibility.
    /// @param visibility The help visibility.
    /// @return This object.
    auto setVisibility(HelpVisibility visibility) noexcept -> HelpData &;

public: // tests
    /// Test if this metadata shall be shown in compact footer help.
    [[nodiscard]] auto isVisibleInFooter() const noexcept -> bool;
    /// Test if this metadata shall be shown in detailed help.
    [[nodiscard]] auto isVisibleOnHelpPage() const noexcept -> bool;

private:
    std::string _name;                               ///< Short display name for compact help.
    std::string _description;                        ///< Detailed help text.
    HelpFormat _descriptionFormat{HelpFormat::Text}; ///< Format of the detailed help text.
    int _priority{0};                                ///< Display priority.
    HelpVisibility _visibility{HelpVisibility::All}; ///< Where this metadata is shown.
};

}
