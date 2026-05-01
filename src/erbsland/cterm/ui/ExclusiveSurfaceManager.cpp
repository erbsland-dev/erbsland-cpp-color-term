// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "ExclusiveSurfaceManager.hpp"

#include <stdexcept>

namespace erbsland::cterm::ui {

void ExclusiveSurfaceManager::willAdd(
    const SurfacePtr &surface, [[maybe_unused]] std::size_t index, [[maybe_unused]] const LayoutDataPtr &data) {
    if (!isManagedChild(surface)) {
        throw std::logic_error{"This surface manages its child surfaces internally."};
    }
}

void ExclusiveSurfaceManager::willRemove(
    const SurfacePtr &surface, [[maybe_unused]] std::size_t index, [[maybe_unused]] const LayoutDataPtr &data) {
    if (!isManagedChild(surface)) {
        throw std::logic_error{"This surface manages its child surfaces internally."};
    }
}

void ExclusiveSurfaceManager::willRemoveAll([[maybe_unused]] std::size_t count) {
    throw std::logic_error{"This surface manages its child surfaces internally."};
}

void ExclusiveSurfaceManager::willReplace(
    const SurfacePtr &oldSurface,
    const SurfacePtr &newSurface,
    [[maybe_unused]] std::size_t index,
    [[maybe_unused]] const LayoutDataPtr &oldData,
    [[maybe_unused]] const LayoutDataPtr &newData) {
    if (!isManagedChild(oldSurface) || !isManagedChild(newSurface)) {
        throw std::logic_error{"This surface manages its child surfaces internally."};
    }
}

void ExclusiveSurfaceManager::willMove(
    const SurfacePtr &surface, [[maybe_unused]] std::size_t fromIndex, [[maybe_unused]] std::size_t toIndex) {
    if (!isManagedChild(surface)) {
        throw std::logic_error{"This surface manages its child surfaces internally."};
    }
}

}
