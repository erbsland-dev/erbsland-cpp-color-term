// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "SurfaceManager.hpp"

namespace erbsland::cterm::ui {

auto SurfaceManager::defaultLayoutData([[maybe_unused]] const SurfacePtr &surface) const -> LayoutDataPtr {
    return {};
}

void SurfaceManager::willAdd(
    [[maybe_unused]] const SurfacePtr &surface,
    [[maybe_unused]] const std::size_t index,
    [[maybe_unused]] const LayoutDataPtr &data) {
}

void SurfaceManager::didAdd(
    [[maybe_unused]] const SurfacePtr &surface,
    [[maybe_unused]] const std::size_t index,
    [[maybe_unused]] const LayoutDataPtr &data) noexcept {
}

void SurfaceManager::willRemove(
    [[maybe_unused]] const SurfacePtr &surface,
    [[maybe_unused]] const std::size_t index,
    [[maybe_unused]] const LayoutDataPtr &data) {
}

void SurfaceManager::didRemove(
    [[maybe_unused]] const SurfacePtr &surface,
    [[maybe_unused]] const std::size_t index,
    [[maybe_unused]] const LayoutDataPtr &data) noexcept {
}

void SurfaceManager::willRemoveAll([[maybe_unused]] const std::size_t count) {
}

void SurfaceManager::didRemoveAll() noexcept {
}

void SurfaceManager::willReplace(
    [[maybe_unused]] const SurfacePtr &oldSurface,
    [[maybe_unused]] const SurfacePtr &newSurface,
    [[maybe_unused]] const std::size_t index,
    [[maybe_unused]] const LayoutDataPtr &oldData,
    [[maybe_unused]] const LayoutDataPtr &newData) {
}

void SurfaceManager::didReplace(
    [[maybe_unused]] const SurfacePtr &oldSurface,
    [[maybe_unused]] const SurfacePtr &newSurface,
    [[maybe_unused]] const std::size_t index,
    [[maybe_unused]] const LayoutDataPtr &oldData,
    [[maybe_unused]] const LayoutDataPtr &newData) noexcept {
}

void SurfaceManager::willMove(
    [[maybe_unused]] const SurfacePtr &surface,
    [[maybe_unused]] const std::size_t fromIndex,
    [[maybe_unused]] const std::size_t toIndex) {
}

void SurfaceManager::didMove(
    [[maybe_unused]] const SurfacePtr &surface,
    [[maybe_unused]] const std::size_t fromIndex,
    [[maybe_unused]] const std::size_t toIndex) noexcept {
}

}
