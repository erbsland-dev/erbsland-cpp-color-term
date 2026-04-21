// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Action.hpp"
#include "../Layout.hpp"
#include "../surface/Button.hpp"

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
    struct LayoutButton final {
        surface::ButtonPtr button;
        Coordinate width{0};
    };

    struct LayoutRow final {
        std::vector<LayoutButton> buttons;
        Coordinate width{0};
    };

private:
    /// Return all visible buttons in insertion order.
    [[nodiscard]] auto visibleButtons() const -> std::vector<surface::ButtonPtr>;
    /// Return all visible and enabled buttons in insertion order.
    [[nodiscard]] auto focusableButtons() const -> std::vector<surface::ButtonPtr>;
    /// Build centered layout rows for a size.
    [[nodiscard]] auto buildRows(Size size, Size spacing) const -> std::vector<LayoutRow>;
    /// Calculate the height required to show every visible button for the given width.
    [[nodiscard]] auto requiredHeight(Coordinate width, Size spacing) const -> Coordinate;
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
