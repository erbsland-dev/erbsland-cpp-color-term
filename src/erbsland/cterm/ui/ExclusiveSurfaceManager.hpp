// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "SurfaceManager.hpp"

namespace erbsland::cterm::ui {

/// A surface manager that ensures that only allows a fixed, exclusive set of child surfaces.
/// This is a convenience subclass for composition patterns:
/// <code>
/// class MySurface : public Surface, protected ExclusiveSurfaceManager {
/// public:
///     [[nodiscard]] static auto create() -> MySurfacePtr {
///          auto result = std::make_shared<MySurface>();
///          result->initializeUi();
///          return result;
///     }
/// protected:
///     auto isManagedChild(const SurfacePtr &surface) const noexcept -> bool {
///         return surface == _managedChild;
///     }
/// private:
///     void initializeUi() {
///        _managedChild = TextBox::create();
///        childStorage().add(_managedChild); // works!
///     }
/// private:
///     TextBoxPtr _managedChild;
/// };
/// </code>
class ExclusiveSurfaceManager : public SurfaceManager {
public:
    ~ExclusiveSurfaceManager() override = default;

public: // implement SurfaceManager
    void willAdd(const SurfacePtr &surface, std::size_t index, const LayoutDataPtr &data) final;
    void willRemove(const SurfacePtr &surface, std::size_t index, const LayoutDataPtr &data) final;
    void willRemoveAll(std::size_t count) final;
    void willReplace(
        const SurfacePtr &oldSurface,
        const SurfacePtr &newSurface,
        std::size_t index,
        const LayoutDataPtr &oldData,
        const LayoutDataPtr &newData) final;
    void willMove(const SurfacePtr &surface, std::size_t fromIndex, std::size_t toIndex) final;

protected:
    /// Check if this manager manages the given surface.
    /// @return `true` to accept a surface being added as a child, `false` to throw an exception.
    [[nodiscard]] virtual auto isManagedChild(const SurfacePtr &surface) const noexcept -> bool = 0;
};

}
