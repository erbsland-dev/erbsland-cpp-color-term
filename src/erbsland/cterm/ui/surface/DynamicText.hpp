// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "StaticText.hpp"

#include <chrono>
#include <functional>
#include <memory>
#include <string_view>

namespace erbsland::cterm::ui::surface {

class DynamicText;
using DynamicTextPtr = std::shared_ptr<DynamicText>;

/// A one-line text surface with optional manual or scheduled updates.
class DynamicText final : public StaticText {
public:
    /// Duration type used for the automatic update interval.
    using milliseconds = std::chrono::milliseconds;
    /// Callback type used to update the text.
    using UpdateFn = std::function<void(String &text, Coordinate availableWidth)>;

public:
    /// Create dynamic text with an initial text and alignment.
    /// @param text The initial text.
    /// @param alignment The alignment used when painting.
    DynamicText(String text, Alignment alignment, ProtectedTag);
    ~DynamicText() override = default;

public:
    /// Create dynamic text with an initial terminal string.
    /// @param text The initial text.
    /// @param alignment The alignment used when painting.
    /// @return The new dynamic text.
    [[nodiscard]] static auto create(String text = {}, Alignment alignment = Alignment::TopLeft) -> DynamicTextPtr;
    /// Create dynamic text with an initial UTF-8 text.
    /// @param text The initial text.
    /// @param alignment The alignment used when painting.
    /// @return The new dynamic text.
    [[nodiscard]] static auto create(std::string_view text, Alignment alignment = Alignment::TopLeft) -> DynamicTextPtr;

public:
    /// Replace the update function.
    /// @param updateFn The new update function.
    void setUpdateFn(UpdateFn updateFn);
    /// Remove the update function.
    void clearUpdateFn() noexcept;
    /// Update the text once by calling the update function.
    void updateText();
    /// Get the automatic update interval.
    /// @return A non-positive duration for manual updates.
    [[nodiscard]] auto updateInterval() const noexcept -> milliseconds { return _updateInterval; }
    /// Replace the automatic update interval.
    /// A value less than or equal to zero disables automatic updates.
    /// @param interval The new interval.
    void setUpdateInterval(milliseconds interval);

private:
    UpdateFn _updateFn;                       ///< Optional update function.
    milliseconds _updateInterval{};           ///< Automatic update interval, or zero for manual updates.
    ScheduledActionRef _scheduledUpdateRef{}; ///< Scheduled automatic update action.
};

}
