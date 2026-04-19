// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "AbstractStatusLine.hpp"

#include <array>
#include <functional>
#include <memory>
#include <optional>

namespace erbsland::cterm::ui::surface {

class StatusLine;
using StatusLinePtr = std::shared_ptr<StatusLine>;

/// A configurable status line with optional left, middle, and right fields.
class StatusLine final : public AbstractStatusLine {
public:
    /// Automatic update handling for one field.
    enum class UpdateMode : uint8_t {
        Static,    ///< Never call the update function; text changes happen only through `setText()`.
        OnResize,  ///< Call the update function when the section width changes and an update function exists.
        OnRefresh, ///< Call the update function before every paint pass when an update function exists.
    };

    /// Callback type used to update one field.
    using UpdateFn = std::function<void(String &text, Coordinate sectionWidth)>;

    /// The owned configuration and content for one field.
    struct Field final {
        String text;                                                   ///< The current field text.
        CollapseBehavior collapseBehavior{CollapseBehavior::Ellipsis}; ///< The field collapse behavior.
        UpdateMode updateMode{UpdateMode::Static};                     ///< The field update mode.
        UpdateFn updateFn;                                             ///< The optional field update function.
        Margins margins{};                                             ///< The margins used for field layout.
    };

public:
    /// Create a status line with fixed height `1`.
    explicit StatusLine(ProtectedTag) noexcept;
    ~StatusLine() override = default;

public:
    /// Create a configurable status line.
    /// @return The new status line.
    [[nodiscard]] static auto create() noexcept -> StatusLinePtr;

public:
    /// Replace the text for one section.
    /// @param section The section to update.
    /// @param text The new section text.
    void setText(Section section, String text);
    /// Replace the text for one section from UTF-8 text.
    /// @param section The section to update.
    /// @param text The new section text.
    void setText(Section section, std::string_view text);
    /// Clear the text for one section while keeping the field configuration.
    /// @param section The section to clear.
    void clearText(Section section);
    /// Replace the margins for one section.
    /// @param section The section to update.
    /// @param margins The new field margins.
    void setMargins(Section section, Margins margins) noexcept;
    /// Replace the collapse behavior for one section.
    /// @param section The section to update.
    /// @param behavior The new collapse behavior.
    void setCollapseBehavior(Section section, CollapseBehavior behavior) noexcept;
    /// Get the configured collapse behavior for one section.
    /// Missing sections use `CollapseBehavior::Ellipsis`.
    /// @param section The section to query.
    /// @return The configured collapse behavior.
    [[nodiscard]] auto collapseBehavior(Section section) const noexcept -> CollapseBehavior override;
    /// Replace the update mode for one section.
    /// @param section The section to update.
    /// @param updateMode The new update mode.
    void setUpdateMode(Section section, UpdateMode updateMode) noexcept;
    /// Get the configured update mode for one section.
    /// Missing sections use `UpdateMode::Static`.
    /// @param section The section to query.
    /// @return The configured update mode.
    [[nodiscard]] auto updateMode(Section section) const noexcept -> UpdateMode;
    /// Replace the update function for one section.
    /// @param section The section to update.
    /// @param updateFn The new update function.
    void setUpdateFn(Section section, UpdateFn updateFn);
    /// Remove the update function for one section.
    /// @param section The section to update.
    void clearUpdateFn(Section section) noexcept;

public: // implement AbstractStatusLine
    [[nodiscard]] auto text(Section section) const -> const String & override;
    [[nodiscard]] auto hasText(Section section) const -> bool override;
    [[nodiscard]] auto margins(Section section) const -> Margins override;

public: // implement Surface
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

private:
    /// Get mutable access to one field, creating it if required.
    /// @param section The section to access.
    /// @return The requested field.
    [[nodiscard]] auto fieldOrCreate(Section section) noexcept -> Field &;
    /// Get access to the optional field storage for one section.
    /// @param section The section to access.
    /// @return The optional field storage for that section.
    [[nodiscard]] auto fieldStorage(Section section) noexcept -> std::optional<Field> &;
    /// @overload
    [[nodiscard]] auto fieldStorage(Section section) const noexcept -> const std::optional<Field> &;
    /// Get the effective update width for one section.
    /// @param section The section to query.
    /// @param totalWidth The available total line width.
    /// @return The pre-collapse width budget for that section.
    [[nodiscard]] auto updateWidthForSection(Section section, Coordinate totalWidth) const noexcept -> Coordinate;
    /// Run one field update if required.
    /// @param section The section to update.
    /// @param totalWidth The available total line width.
    void updateField(Section section, Coordinate totalWidth) noexcept;
    /// Convert one section enum into a storage index.
    /// @param section The section.
    /// @return The zero-based section index.
    [[nodiscard]] static auto indexForSection(Section section) noexcept -> std::size_t;
    /// Mark one section as requiring a fresh `OnResize` callback.
    /// @param section The section to reset.
    void resetResizeTracking(Section section) noexcept;

private:
    std::array<std::optional<Field>, 3> _fields;          ///< The optional fields indexed by section.
    std::array<std::optional<Coordinate>, 3> _lastWidths; ///< The last widths used for `OnResize` updates.
};

}
