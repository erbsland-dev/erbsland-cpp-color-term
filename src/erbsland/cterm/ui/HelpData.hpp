// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <cstdint>
#include <string>

namespace erbsland::cterm::ui {

/// Visibility of action help metadata.
enum class HelpVisibility : uint8_t {
    Hidden,   ///< Hide this action from generated help.
    Footer,   ///< Show this action only in compact footer help.
    HelpPage, ///< Show this action only in detailed help.
    All,      ///< Show this action in compact footer help and detailed help.
};

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
    /// Set the detailed help description.
    /// @param description The detailed help description.
    /// @return This object.
    auto setDescription(std::string description) -> HelpData &;
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
    int _priority{0};                                ///< Display priority.
    HelpVisibility _visibility{HelpVisibility::All}; ///< Where this metadata is shown.
};

}
