// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "SurfaceContainer.hpp"

#include "Surface.hpp"

#include <algorithm>
#include <ranges>
#include <stdexcept>

namespace erbsland::cterm::ui {

SurfaceContainer::SurfaceContainer(Surface &owner) noexcept : _owner{owner} {
}

void SurfaceContainer::setManager(SurfaceManager &manager) noexcept {
    _manager = &manager;
}

void SurfaceContainer::clearManager() noexcept {
    _manager = nullptr;
}

auto SurfaceContainer::empty() const noexcept -> bool {
    return _surfaces.empty();
}

auto SurfaceContainer::size() const noexcept -> std::size_t {
    return _surfaces.size();
}

auto SurfaceContainer::operator[](const std::size_t index) const noexcept -> const SurfacePtr & {
    return _surfaces[index].surface;
}

auto SurfaceContainer::at(const std::size_t index) const -> const SurfacePtr & {
    return _surfaces.at(index).surface;
}

auto SurfaceContainer::layoutData(const std::size_t index) const -> LayoutDataPtr {
    return _surfaces.at(index).data;
}

auto SurfaceContainer::layoutData(const SurfacePtr &surface) const -> LayoutDataPtr {
    const auto index = indexOf(surface);
    if (index == cInvalidIndex) {
        return {};
    }
    return _surfaces[index].data;
}

void SurfaceContainer::setLayoutData(const std::size_t index, LayoutDataPtr data) {
    if (index >= _surfaces.size()) {
        throw std::out_of_range{"Surface index is out of range."};
    }
    _surfaces[index].data = std::move(data);
    invalidateLayout();
}

void SurfaceContainer::setLayoutData(const SurfacePtr &surface, LayoutDataPtr data) {
    setLayoutData(requireIndexOf(surface), std::move(data));
}

void SurfaceContainer::add(SurfacePtr surface) {
    insert(_surfaces.size(), std::move(surface));
}

void SurfaceContainer::add(SurfacePtr surface, LayoutDataPtr data) {
    insert(_surfaces.size(), std::move(surface), std::move(data));
}

void SurfaceContainer::remove(const std::size_t index) {
    if (index >= _surfaces.size()) {
        throw std::out_of_range{"Surface index is out of range."};
    }
    auto item = _surfaces[index];
    if (_manager != nullptr) {
        _manager->willRemove(item.surface, index, item.data);
    }
    _surfaces.erase(_surfaces.begin() + static_cast<std::ptrdiff_t>(index));
    detachFromOwner(item.surface);
    if (_manager != nullptr) {
        _manager->didRemove(item.surface, index, item.data);
    }
    invalidateLayout();
}

void SurfaceContainer::remove(const SurfacePtr &surface) {
    const auto index = indexOf(surface);
    if (index == cInvalidIndex) {
        return;
    }
    remove(index);
}

void SurfaceContainer::removeAll() {
    if (_surfaces.empty()) {
        return;
    }
    const auto oldSurfaces = _surfaces;
    if (_manager != nullptr) {
        _manager->willRemoveAll(oldSurfaces.size());
        for (auto index = std::size_t{0}; index < oldSurfaces.size(); ++index) {
            _manager->willRemove(oldSurfaces[index].surface, index, oldSurfaces[index].data);
        }
    }
    for (const auto &item : oldSurfaces) {
        detachFromOwner(item.surface);
    }
    _surfaces.clear();
    if (_manager != nullptr) {
        for (auto index = std::size_t{0}; index < oldSurfaces.size(); ++index) {
            _manager->didRemove(oldSurfaces[index].surface, index, oldSurfaces[index].data);
        }
        _manager->didRemoveAll();
    }
    invalidateLayout();
}

void SurfaceContainer::insert(const std::size_t index, SurfacePtr surface) {
    insert(index, std::move(surface), {});
}

void SurfaceContainer::insert(const std::size_t index, SurfacePtr surface, LayoutDataPtr data) {
    if (index > _surfaces.size()) {
        throw std::out_of_range{"Surface insertion index is out of range."};
    }
    validateMaximumCount(_surfaces.size() + 1);
    validateNewChild(surface);
    auto itemData = dataOrDefault(surface, std::move(data));
    auto item = Item{std::move(surface), std::move(itemData)};
    if (_manager != nullptr) {
        _manager->willAdd(item.surface, index, item.data);
    }
    detachFromCurrentParent(item.surface);
    attachToOwner(item.surface);
    _surfaces.insert(_surfaces.begin() + static_cast<std::ptrdiff_t>(index), item);
    if (_manager != nullptr) {
        _manager->didAdd(item.surface, index, item.data);
    }
    invalidateLayout();
}

void SurfaceContainer::insertAfter(const SurfacePtr &anchor, SurfacePtr surface) {
    insert(requireIndexOf(anchor) + 1, std::move(surface));
}

void SurfaceContainer::insertBefore(const SurfacePtr &anchor, SurfacePtr surface) {
    insert(requireIndexOf(anchor), std::move(surface));
}

void SurfaceContainer::replace(const SurfacePtr &oldSurface, SurfacePtr newSurface) {
    replace(requireIndexOf(oldSurface), std::move(newSurface));
}

void SurfaceContainer::replace(const SurfacePtr &oldSurface, SurfacePtr newSurface, LayoutDataPtr data) {
    replace(requireIndexOf(oldSurface), std::move(newSurface), std::move(data));
}

void SurfaceContainer::replace(const std::size_t index, SurfacePtr surface) {
    replace(index, std::move(surface), {});
}

void SurfaceContainer::replace(const std::size_t index, SurfacePtr surface, LayoutDataPtr data) {
    if (index >= _surfaces.size()) {
        throw std::out_of_range{"Surface replacement index is out of range."};
    }
    if (_surfaces[index].surface == surface) {
        if (data != nullptr) {
            setLayoutData(index, std::move(data));
        }
        return;
    }
    validateNewChild(surface);
    auto oldItem = _surfaces[index];
    auto itemData = dataOrDefault(surface, std::move(data));
    auto newItem = Item{std::move(surface), std::move(itemData)};
    if (_manager != nullptr) {
        _manager->willReplace(oldItem.surface, newItem.surface, index, oldItem.data, newItem.data);
    }
    detachFromCurrentParent(newItem.surface);
    attachToOwner(newItem.surface);
    _surfaces[index] = newItem;
    detachFromOwner(oldItem.surface);
    if (_manager != nullptr) {
        _manager->didReplace(oldItem.surface, newItem.surface, index, oldItem.data, newItem.data);
    }
    invalidateLayout();
}

void SurfaceContainer::move(const std::size_t fromIndex, const std::size_t toIndex) {
    if (fromIndex >= _surfaces.size() || toIndex >= _surfaces.size()) {
        throw std::out_of_range{"Surface move index is out of range."};
    }
    if (fromIndex == toIndex) {
        return;
    }
    auto item = _surfaces[fromIndex];
    if (_manager != nullptr) {
        _manager->willMove(item.surface, fromIndex, toIndex);
    }
    _surfaces.erase(_surfaces.begin() + static_cast<std::ptrdiff_t>(fromIndex));
    _surfaces.insert(_surfaces.begin() + static_cast<std::ptrdiff_t>(toIndex), item);
    if (_manager != nullptr) {
        _manager->didMove(item.surface, fromIndex, toIndex);
    }
    invalidateLayout();
}

void SurfaceContainer::move(const SurfacePtr &surface, const std::size_t toIndex) {
    move(requireIndexOf(surface), toIndex);
}

void SurfaceContainer::moveAfter(const SurfacePtr &surface, const SurfacePtr &anchor) {
    const auto surfaceIndex = requireIndexOf(surface);
    const auto anchorIndex = requireIndexOf(anchor);
    if (surfaceIndex == anchorIndex) {
        return;
    }
    move(surfaceIndex, surfaceIndex < anchorIndex ? anchorIndex : anchorIndex + 1);
}

void SurfaceContainer::moveBefore(const SurfacePtr &surface, const SurfacePtr &anchor) {
    const auto surfaceIndex = requireIndexOf(surface);
    const auto anchorIndex = requireIndexOf(anchor);
    if (surfaceIndex == anchorIndex) {
        return;
    }
    move(surfaceIndex, surfaceIndex < anchorIndex ? anchorIndex - 1 : anchorIndex);
}

auto SurfaceContainer::requireIndexOf(const SurfacePtr &surface) const -> std::size_t {
    const auto index = indexOf(surface);
    if (index == cInvalidIndex) {
        throw std::out_of_range{"Surface is not in this container."};
    }
    return index;
}

auto SurfaceContainer::indexOf(const SurfacePtr &surface) const noexcept -> std::size_t {
    const auto it = std::ranges::find_if(_surfaces, [&](const Item &item) { return item.surface == surface; });
    if (it == _surfaces.end()) {
        return cInvalidIndex;
    }
    return static_cast<std::size_t>(std::ranges::distance(_surfaces.begin(), it));
}

auto SurfaceContainer::isInOwnerParentChain(const SurfacePtr &surface) const noexcept -> bool {
    if (surface.get() == &_owner) {
        return true;
    }
    for (auto parent = _owner.parent().lock(); parent != nullptr; parent = parent->parent().lock()) {
        if (parent == surface) {
            return true;
        }
    }
    return false;
}

void SurfaceContainer::validateNewChild(const SurfacePtr &surface) const {
    if (surface == nullptr) {
        throw std::invalid_argument{"Cannot add a null surface."};
    }
    if (isInOwnerParentChain(surface)) {
        throw std::invalid_argument{"Cannot add a surface to its own parent chain."};
    }
    if (indexOf(surface) != cInvalidIndex) {
        throw std::invalid_argument{"Surface is already in this container."};
    }
}

auto SurfaceContainer::dataOrDefault(const SurfacePtr &surface, LayoutDataPtr data) const -> LayoutDataPtr {
    if (data != nullptr || _manager == nullptr) {
        return data;
    }
    return _manager->defaultLayoutData(surface);
}

void SurfaceContainer::validateMaximumCount(const std::size_t count) const {
    if (_manager != nullptr && count > _manager->maximumSurfaceCount()) {
        throw std::invalid_argument{"Too many child surfaces for this surface."};
    }
}

void SurfaceContainer::detachFromCurrentParent(const SurfacePtr &surface) const {
    if (const auto currentParent = surface->parent().lock(); currentParent != nullptr) {
        currentParent->childStorage().remove(surface);
    }
}

void SurfaceContainer::attachToOwner(const SurfacePtr &surface) const {
    surface->_parent = _owner.shared_from_this();
}

void SurfaceContainer::detachFromOwner(const SurfacePtr &surface) const noexcept {
    if (surface->parent().lock().get() == &_owner) {
        surface->clearSubtreeFocusIfNeeded();
        surface->_parent.reset();
    }
}

void SurfaceContainer::invalidateLayout() {
    _owner.flags().setLayoutOutdated();
}

}
