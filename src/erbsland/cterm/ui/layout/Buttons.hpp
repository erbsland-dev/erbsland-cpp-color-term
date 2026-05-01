// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Action.hpp"
#include "../Layout.hpp"
#include "../surface/Button.hpp"

#include <functional>
#include <vector>

namespace erbsland::cterm::ui::layout {

class Buttons;
using ButtonsPtr = std::shared_ptr<Buttons>;

/// A horizontal button layout that centers rows and wraps when needed.
class Buttons final : public Layout {
public:
    /// Create a button layout.
    explicit Buttons(ProtectedTag) noexcept;
    ~Buttons() override = default;

public:
    /// Create a button layout.
    /// @return The new button layout.
    [[nodiscard]] static auto create() -> ButtonsPtr;

public:
    /// Add an action-backed button.
    /// @param action The action represented by the new button.
    /// @return The created button surface.
    auto addAction(ButtonActionPtr action) -> surface::ButtonPtr;
    /// Add an existing button.
    /// @param button The button to add.
    void addButton(surface::ButtonPtr button);
    /// Insert an existing button.
    /// @param index The insertion index.
    /// @param button The button to insert.
    void insertButton(std::size_t index, surface::ButtonPtr button);
    /// Get the number of buttons in this layout.
    [[nodiscard]] auto buttonCount() const noexcept -> std::size_t;
    /// Access one button.
    /// @param index The button index.
    [[nodiscard]] auto button(std::size_t index) const -> surface::ButtonPtr;

public: // implement Surface
    [[nodiscard]] auto onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept
        -> LayoutMetrics override;
    void onLayout(LayoutScope &scope) noexcept override;
    void onKeyPress(KeyPressEvent &keyPressEvent) noexcept override;

protected: // implement SurfaceManager
    void willAdd(const SurfacePtr &surface, std::size_t index, const LayoutDataPtr &data) override;
    void didAdd(const SurfacePtr &surface, std::size_t index, const LayoutDataPtr &data) noexcept override;
    void didRemove(const SurfacePtr &surface, std::size_t index, const LayoutDataPtr &data) noexcept override;
    void willReplace(
        const SurfacePtr &oldSurface,
        const SurfacePtr &newSurface,
        std::size_t index,
        const LayoutDataPtr &oldData,
        const LayoutDataPtr &newData) override;
    void didReplace(
        const SurfacePtr &oldSurface,
        const SurfacePtr &newSurface,
        std::size_t index,
        const LayoutDataPtr &oldData,
        const LayoutDataPtr &newData) noexcept override;

private:
    /// Initialize theme attributes after construction.
    void initializeUi() override;

    struct LayoutButton final {
        surface::ButtonPtr button;
        Margins margins;
        Coordinate width{0};
    };

    struct LayoutRow final {
        std::vector<LayoutButton> buttons;
        Margins margins;
        Coordinate width{0};
    };

    using MeasureButtonFunction = std::function<LayoutMetrics(const surface::ButtonPtr &, const LayoutProposal &)>;

private:
    /// Return all visible buttons in insertion order.
    [[nodiscard]] auto visibleButtons() const -> std::vector<surface::ButtonPtr>;
    /// Return all visible and enabled buttons in insertion order.
    [[nodiscard]] auto focusableButtons() const -> std::vector<surface::ButtonPtr>;
    /// Build centered layout rows for a size.
    [[nodiscard]] auto buildRows(Size size, Size spacing, const MeasureButtonFunction &measureButton) const
        -> std::vector<LayoutRow>;
    /// Collapse two adjacent margins with a minimum spacing.
    [[nodiscard]] static auto collapsedSpacing(Coordinate minimum, Coordinate trailing, Coordinate leading) noexcept
        -> Coordinate;
    /// Calculate the height required to show the prepared rows.
    [[nodiscard]] static auto requiredHeight(const std::vector<LayoutRow> &rows, Size spacing) noexcept -> Coordinate;
    /// Calculate the minimum height required to show the prepared rows.
    [[nodiscard]] static auto requiredMinimumHeight(const std::vector<LayoutRow> &rows, Size spacing) noexcept
        -> Coordinate;
    /// Test if a button is fully inside the current local surface rectangle.
    [[nodiscard]] auto isButtonFullyVisible(const surface::ButtonPtr &button) const noexcept -> bool;
    /// Move focus in a direction.
    auto moveFocus(bool forward) noexcept -> bool;
    /// Add the represented action to this layout's dispatch container.
    void attachAction(const ActionPtr &action);
    /// Remove an action when no remaining button uses it.
    void detachActionIfUnused(const ActionPtr &action);

private:
};

}
