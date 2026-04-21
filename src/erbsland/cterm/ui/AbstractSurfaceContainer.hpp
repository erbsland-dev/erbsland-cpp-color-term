// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "LayoutData.hpp"
#include "SurfacePtr.hpp"
#include "ui_namespace.hpp"

#include <cstddef>
#include <iterator>

namespace erbsland::cterm::ui {

/// Abstract interface for surface child containers.
class AbstractSurfaceContainer {
public:
    /// Read-only iterator over child surfaces.
    class const_iterator {
    public:
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::forward_iterator_tag;
        using pointer = const SurfacePtr *;
        using reference = const SurfacePtr &;
        using value_type = SurfacePtr;

    public:
        const_iterator() = default;
        const_iterator(const AbstractSurfaceContainer *container, std::size_t index) noexcept;

    public:
        [[nodiscard]] auto operator*() const noexcept -> reference;
        [[nodiscard]] auto operator->() const noexcept -> pointer;
        auto operator++() noexcept -> const_iterator &;
        auto operator++(int) noexcept -> const_iterator;
        [[nodiscard]] auto operator==(const const_iterator &other) const noexcept -> bool;
        [[nodiscard]] auto operator!=(const const_iterator &other) const noexcept -> bool;

    private:
        const AbstractSurfaceContainer *_container{nullptr}; ///< The iterated container.
        std::size_t _index{0};                               ///< The current surface index.
    };

public:
    // defaults
    virtual ~AbstractSurfaceContainer() = default;
    AbstractSurfaceContainer(const AbstractSurfaceContainer &) = delete;
    AbstractSurfaceContainer(AbstractSurfaceContainer &&) = delete;
    auto operator=(const AbstractSurfaceContainer &) -> AbstractSurfaceContainer & = delete;
    auto operator=(AbstractSurfaceContainer &&) -> AbstractSurfaceContainer & = delete;

public:
    /// Test if there are no child surfaces.
    [[nodiscard]] virtual auto empty() const noexcept -> bool = 0;
    /// Get the number of child surfaces.
    [[nodiscard]] virtual auto size() const noexcept -> std::size_t = 0;
    /// Get a child surface by index without bounds checks.
    [[nodiscard]] virtual auto operator[](std::size_t index) const noexcept -> const SurfacePtr & = 0;
    /// Get a child surface by index with bounds checks.
    /// @throws std::out_of_range If the index is outside the child list.
    [[nodiscard]] virtual auto at(std::size_t index) const -> const SurfacePtr & = 0;
    /// Get layout data for the child surface at the given index.
    /// @throws std::out_of_range If the index is outside the child list.
    [[nodiscard]] virtual auto layoutData(std::size_t index) const -> LayoutDataPtr = 0;
    /// Get layout data for the given child surface.
    /// @return The layout data, or an empty pointer if the surface is not a child.
    [[nodiscard]] virtual auto layoutData(const SurfacePtr &surface) const -> LayoutDataPtr = 0;
    /// Get a const iterator to the first child surface.
    [[nodiscard]] auto begin() const noexcept -> const_iterator;
    /// Get a const iterator past the last child surface.
    [[nodiscard]] auto end() const noexcept -> const_iterator;

public:
    /// Add a child surface at the end.
    /// @param surface The surface to add.
    virtual void add(SurfacePtr surface) = 0;
    /// Add a child surface with layout data at the end.
    /// @param surface The surface to add.
    /// @param data The layout data for the parent-child relation.
    virtual void add(SurfacePtr surface, LayoutDataPtr data) = 0;
    /// Remove the child surface at the given index.
    /// @param index The index to remove.
    virtual void remove(std::size_t index) = 0;
    /// Remove the given child surface.
    /// @param surface The child surface to remove.
    virtual void remove(const SurfacePtr &surface) = 0;
    /// Remove all child surfaces.
    virtual void removeAll() = 0;
    /// Insert a child surface before the given index.
    /// @param index The insertion index.
    /// @param surface The surface to insert.
    virtual void insert(std::size_t index, SurfacePtr surface) = 0;
    /// Insert a child surface with layout data before the given index.
    /// @param index The insertion index.
    /// @param surface The surface to insert.
    /// @param data The layout data for the parent-child relation.
    virtual void insert(std::size_t index, SurfacePtr surface, LayoutDataPtr data) = 0;
    /// Insert a child surface after the given anchor.
    /// @param anchor The existing child surface after which the new surface is inserted.
    /// @param surface The surface to insert.
    virtual void insertAfter(const SurfacePtr &anchor, SurfacePtr surface) = 0;
    /// Insert a child surface before the given anchor.
    /// @param anchor The existing child surface before which the new surface is inserted.
    /// @param surface The surface to insert.
    virtual void insertBefore(const SurfacePtr &anchor, SurfacePtr surface) = 0;
    /// Replace an existing child surface.
    /// @param oldSurface The existing child surface.
    /// @param newSurface The replacement surface.
    virtual void replace(const SurfacePtr &oldSurface, SurfacePtr newSurface) = 0;
    /// Replace an existing child surface and layout data.
    /// @param oldSurface The existing child surface.
    /// @param newSurface The replacement surface.
    /// @param data The layout data for the replacement surface.
    virtual void replace(const SurfacePtr &oldSurface, SurfacePtr newSurface, LayoutDataPtr data) = 0;
    /// Replace the child surface at the given index.
    /// @param index The replacement index.
    /// @param surface The replacement surface.
    virtual void replace(std::size_t index, SurfacePtr surface) = 0;
    /// Replace the child surface and layout data at the given index.
    /// @param index The replacement index.
    /// @param surface The replacement surface.
    /// @param data The layout data for the replacement surface.
    virtual void replace(std::size_t index, SurfacePtr surface, LayoutDataPtr data) = 0;
    /// Move a child surface to a new index.
    /// @param fromIndex The current child index.
    /// @param toIndex The new child index.
    virtual void move(std::size_t fromIndex, std::size_t toIndex) = 0;
    /// Move a child surface to a new index.
    /// @param surface The child surface to move.
    /// @param toIndex The new child index.
    virtual void move(const SurfacePtr &surface, std::size_t toIndex) = 0;
    /// Move a child surface after the given anchor.
    /// @param surface The child surface to move.
    /// @param anchor The anchor after which the surface is moved.
    virtual void moveAfter(const SurfacePtr &surface, const SurfacePtr &anchor) = 0;
    /// Move a child surface before the given anchor.
    /// @param surface The child surface to move.
    /// @param anchor The anchor before which the surface is moved.
    virtual void moveBefore(const SurfacePtr &surface, const SurfacePtr &anchor) = 0;

protected:
    /// Create the abstract container base.
    AbstractSurfaceContainer() = default;
};

inline AbstractSurfaceContainer::const_iterator::const_iterator(
    const AbstractSurfaceContainer *container, const std::size_t index) noexcept :
    _container{container}, _index{index} {
}

inline auto AbstractSurfaceContainer::const_iterator::operator*() const noexcept -> reference {
    return (*_container)[_index];
}

inline auto AbstractSurfaceContainer::const_iterator::operator->() const noexcept -> pointer {
    return &(*_container)[_index];
}

inline auto AbstractSurfaceContainer::const_iterator::operator++() noexcept -> const_iterator & {
    ++_index;
    return *this;
}

inline auto AbstractSurfaceContainer::const_iterator::operator++(int) noexcept -> const_iterator {
    auto result = *this;
    ++*this;
    return result;
}

inline auto AbstractSurfaceContainer::const_iterator::operator==(const const_iterator &other) const noexcept -> bool {
    return _container == other._container && _index == other._index;
}

inline auto AbstractSurfaceContainer::const_iterator::operator!=(const const_iterator &other) const noexcept -> bool {
    return !(*this == other);
}

inline auto AbstractSurfaceContainer::begin() const noexcept -> const_iterator {
    return const_iterator{this, 0};
}

inline auto AbstractSurfaceContainer::end() const noexcept -> const_iterator {
    return const_iterator{this, size()};
}

}
