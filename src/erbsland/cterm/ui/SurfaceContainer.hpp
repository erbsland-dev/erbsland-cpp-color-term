// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "AbstractSurfaceContainer.hpp"
#include "SurfaceManager.hpp"
#include "ui_namespace.hpp"

#include <cstddef>
#include <limits>
#include <vector>

namespace erbsland::cterm::ui {

class Surface;

/// Ordered child surface collection for one surface.
class SurfaceContainer final : public AbstractSurfaceContainer {
public:
    /// One child surface with metadata owned by the parent-child relation.
    struct Item final {
        SurfacePtr surface; ///< The child surface.
        LayoutDataPtr data; ///< Optional layout data.
    };
    /// The underlying ordered item container.
    using Container = std::vector<Item>;

public:
    /// Create a child surface container for the given owner surface.
    /// @param owner The owning surface.
    explicit SurfaceContainer(Surface &owner) noexcept;
    ~SurfaceContainer() override = default;

    // defaults
    SurfaceContainer(const SurfaceContainer &) = delete;
    SurfaceContainer(SurfaceContainer &&) = delete;
    auto operator=(const SurfaceContainer &) -> SurfaceContainer & = delete;
    auto operator=(SurfaceContainer &&) -> SurfaceContainer & = delete;

public:
    /// Attach a manager for child-surface policy and notifications.
    /// @param manager The manager to attach.
    void setManager(SurfaceManager &manager) noexcept;
    /// Clear the attached surface manager.
    void clearManager() noexcept;

public:
    /// Test if there are no child surfaces.
    [[nodiscard]] auto empty() const noexcept -> bool override;
    /// Get the number of child surfaces.
    [[nodiscard]] auto size() const noexcept -> std::size_t override;
    /// Get a child surface by index without bounds checks.
    [[nodiscard]] auto operator[](std::size_t index) const noexcept -> const SurfacePtr & override;
    /// Get a child surface by index with bounds checks.
    /// @throws std::out_of_range If the index is outside the child list.
    [[nodiscard]] auto at(std::size_t index) const -> const SurfacePtr & override;
    /// Get layout data for the child surface at the given index.
    /// @throws std::out_of_range If the index is outside the child list.
    [[nodiscard]] auto layoutData(std::size_t index) const -> LayoutDataPtr override;
    /// Get layout data for the given child surface.
    /// @return The layout data, or an empty pointer if the surface is not a child.
    [[nodiscard]] auto layoutData(const SurfacePtr &surface) const -> LayoutDataPtr override;
    /// Replace layout data for the child surface at the given index.
    /// @param index The child surface index.
    /// @param data The new layout data.
    void setLayoutData(std::size_t index, LayoutDataPtr data);
    /// Replace layout data for the given child surface.
    /// @param surface The child surface.
    /// @param data The new layout data.
    void setLayoutData(const SurfacePtr &surface, LayoutDataPtr data);

public:
    /// Add a child surface at the end.
    /// @param surface The surface to add.
    /// @throws std::invalid_argument If the surface is null, already present, or would create a cycle.
    void add(SurfacePtr surface) override;
    /// Add a child surface with layout data at the end.
    /// @param surface The surface to add.
    /// @param data The layout data for the parent-child relation.
    /// @throws std::invalid_argument If the surface is null, already present, or would create a cycle.
    void add(SurfacePtr surface, LayoutDataPtr data) override;
    /// Remove the child surface at the given index.
    /// @param index The index to remove.
    /// @throws std::out_of_range If the index is outside the child list.
    void remove(std::size_t index) override;
    /// Remove the given child surface.
    /// @param surface The child surface to remove.
    void remove(const SurfacePtr &surface) override;
    /// Remove all child surfaces.
    void removeAll() override;
    /// Insert a child surface before the given index.
    /// @param index The insertion index.
    /// @param surface The surface to insert.
    /// @throws std::invalid_argument If the surface is null, already present, or would create a cycle.
    /// @throws std::out_of_range If the index is outside the valid insertion range.
    void insert(std::size_t index, SurfacePtr surface) override;
    /// Insert a child surface with layout data before the given index.
    /// @param index The insertion index.
    /// @param surface The surface to insert.
    /// @param data The layout data for the parent-child relation.
    /// @throws std::invalid_argument If the surface is null, already present, or would create a cycle.
    /// @throws std::out_of_range If the index is outside the valid insertion range.
    void insert(std::size_t index, SurfacePtr surface, LayoutDataPtr data) override;
    /// Insert a child surface after the given anchor.
    /// @param anchor The existing child surface after which the new surface is inserted.
    /// @param surface The surface to insert.
    /// @throws std::invalid_argument If the surface is null, already present, or would create a cycle.
    /// @throws std::out_of_range If the anchor is not a child surface.
    void insertAfter(const SurfacePtr &anchor, SurfacePtr surface) override;
    /// Insert a child surface before the given anchor.
    /// @param anchor The existing child surface before which the new surface is inserted.
    /// @param surface The surface to insert.
    /// @throws std::invalid_argument If the surface is null, already present, or would create a cycle.
    /// @throws std::out_of_range If the anchor is not a child surface.
    void insertBefore(const SurfacePtr &anchor, SurfacePtr surface) override;
    /// Replace an existing child surface.
    /// @param oldSurface The existing child surface.
    /// @param newSurface The replacement surface.
    /// @throws std::invalid_argument If the replacement is null, already present, or would create a cycle.
    /// @throws std::out_of_range If the old surface is not a child surface.
    void replace(const SurfacePtr &oldSurface, SurfacePtr newSurface) override;
    /// Replace an existing child surface and layout data.
    /// @param oldSurface The existing child surface.
    /// @param newSurface The replacement surface.
    /// @param data The layout data for the replacement surface.
    /// @throws std::invalid_argument If the replacement is null, already present, or would create a cycle.
    /// @throws std::out_of_range If the old surface is not a child surface.
    void replace(const SurfacePtr &oldSurface, SurfacePtr newSurface, LayoutDataPtr data) override;
    /// Replace the child surface at the given index.
    /// @param index The replacement index.
    /// @param surface The replacement surface.
    /// @throws std::invalid_argument If the replacement is null, already present, or would create a cycle.
    /// @throws std::out_of_range If the index is outside the child list.
    void replace(std::size_t index, SurfacePtr surface) override;
    /// Replace the child surface and layout data at the given index.
    /// @param index The replacement index.
    /// @param surface The replacement surface.
    /// @param data The layout data for the replacement surface.
    /// @throws std::invalid_argument If the replacement is null, already present, or would create a cycle.
    /// @throws std::out_of_range If the index is outside the child list.
    void replace(std::size_t index, SurfacePtr surface, LayoutDataPtr data) override;
    /// Move a child surface to a new index.
    /// @param fromIndex The current child index.
    /// @param toIndex The new child index.
    /// @throws std::out_of_range If either index is outside the child list.
    void move(std::size_t fromIndex, std::size_t toIndex) override;
    /// Move a child surface to a new index.
    /// @param surface The child surface to move.
    /// @param toIndex The new child index.
    /// @throws std::out_of_range If the surface is not a child or the index is outside the child list.
    void move(const SurfacePtr &surface, std::size_t toIndex) override;
    /// Move a child surface after the given anchor.
    /// @param surface The child surface to move.
    /// @param anchor The anchor after which the surface is moved.
    /// @throws std::out_of_range If either surface is not a child surface.
    void moveAfter(const SurfacePtr &surface, const SurfacePtr &anchor) override;
    /// Move a child surface before the given anchor.
    /// @param surface The child surface to move.
    /// @param anchor The anchor before which the surface is moved.
    /// @throws std::out_of_range If either surface is not a child surface.
    void moveBefore(const SurfacePtr &surface, const SurfacePtr &anchor) override;

private:
    /// Find the index of a child surface or throw if it is not present.
    [[nodiscard]] auto requireIndexOf(const SurfacePtr &surface) const -> std::size_t;
    /// Find the index of a child surface.
    [[nodiscard]] auto indexOf(const SurfacePtr &surface) const noexcept -> std::size_t;
    /// Test if the given surface is in the parent chain of the owner surface.
    [[nodiscard]] auto isInOwnerParentChain(const SurfacePtr &surface) const noexcept -> bool;
    /// Validate a new child surface before insertion.
    void validateNewChild(const SurfacePtr &surface) const;
    /// Create default layout data when none was supplied.
    [[nodiscard]] auto dataOrDefault(const SurfacePtr &surface, LayoutDataPtr data) const -> LayoutDataPtr;
    /// Enforce the attached manager's maximum child count.
    void validateMaximumCount(std::size_t count) const;
    /// Detach the child surface from a different current parent.
    void detachFromCurrentParent(const SurfacePtr &surface) const;
    /// Set this container as the parent for the given surface.
    void attachToOwner(const SurfacePtr &surface) const;
    /// Clear this container as the parent for the given surface.
    void detachFromOwner(const SurfacePtr &surface) const noexcept;
    /// Mark the owner surface as requiring layout.
    void invalidateLayout();

private:
    static constexpr auto cInvalidIndex = std::numeric_limits<std::size_t>::max();

private:
    Surface &_owner;                   ///< The owning surface.
    Container _surfaces;               ///< The ordered child surfaces.
    SurfaceManager *_manager{nullptr}; ///< Optional surface manager.
};

}
