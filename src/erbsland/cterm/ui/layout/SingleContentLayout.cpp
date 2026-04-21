// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "SingleContentLayout.hpp"

namespace erbsland::cterm::ui::layout {

void SingleContentLayout::setContentSurface(SurfacePtr contentSurface) {
    if (_contentSurface == contentSurface) {
        return;
    }
    if (contentSurface == nullptr) {
        childStorage().removeAll();
        return;
    }
    if (childStorage().empty()) {
        childStorage().add(std::move(contentSurface));
        return;
    }
    childStorage().replace(std::size_t{0}, std::move(contentSurface));
}

auto SingleContentLayout::contentSurface() const noexcept -> const SurfacePtr & {
    return _contentSurface;
}

void SingleContentLayout::onContentSurfaceChanged(
    [[maybe_unused]] const SurfacePtr &oldContentSurface,
    [[maybe_unused]] const SurfacePtr &newContentSurface) noexcept {
}

auto SingleContentLayout::maximumSurfaceCount() const noexcept -> std::size_t {
    return 1;
}

void SingleContentLayout::didAdd(
    const SurfacePtr &surface,
    [[maybe_unused]] const std::size_t index,
    [[maybe_unused]] const LayoutDataPtr &data) noexcept {
    updateContentSurface(surface);
}

void SingleContentLayout::didRemove(
    const SurfacePtr &surface,
    [[maybe_unused]] const std::size_t index,
    [[maybe_unused]] const LayoutDataPtr &data) noexcept {
    if (_contentSurface == surface) {
        updateContentSurface({});
    }
}

void SingleContentLayout::didReplace(
    [[maybe_unused]] const SurfacePtr &oldSurface,
    const SurfacePtr &newSurface,
    [[maybe_unused]] const std::size_t index,
    [[maybe_unused]] const LayoutDataPtr &oldData,
    [[maybe_unused]] const LayoutDataPtr &newData) noexcept {
    updateContentSurface(newSurface);
}

void SingleContentLayout::updateContentSurface(SurfacePtr contentSurface) noexcept {
    if (_contentSurface == contentSurface) {
        return;
    }
    auto oldContentSurface = std::move(_contentSurface);
    _contentSurface = std::move(contentSurface);
    onContentSurfaceChanged(oldContentSurface, _contentSurface);
}

}
