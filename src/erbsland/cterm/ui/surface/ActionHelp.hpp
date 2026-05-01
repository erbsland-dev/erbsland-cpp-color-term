// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Action.hpp"
#include "../Surface.hpp"

#include "../../theme/StringWithMargins.hpp"

#include <memory>
#include <vector>

namespace erbsland::cterm::ui::surface {

class ActionHelp;
using ActionHelpPtr = std::shared_ptr<ActionHelp>;

/// A one-line surface that renders the currently available keyboard actions for the focused surface chain.
class ActionHelp final : public Surface {
public:
    /// Create an action-help surface.
    explicit ActionHelp(ProtectedTag) noexcept;
    ~ActionHelp() override = default;

public:
    /// Create an action-help surface.
    /// @return The new action-help surface.
    [[nodiscard]] static auto create() -> ActionHelpPtr;

public:
    /// Collect currently displayable actions for the nearest page.
    /// @return The sorted and deduplicated actions.
    [[nodiscard]] auto collectActions() -> std::vector<ActionPtr>;
    /// Render currently displayable actions for a fixed width.
    /// @param context The current paint context.
    /// @param width The available width.
    /// @return The rendered help line.
    [[nodiscard]] auto renderHelpText(const PaintContext &context, Coordinate width) -> String;

public: // implement Surface
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

private:
    /// Initialize theme and scheduled repaint refresh after construction.
    void initializeUi() override;

    struct ActionCandidate final {
        ActionPtr action;          ///< The action.
        int chainIndex{};          ///< The surface-chain index, focused surface first.
        std::size_t actionIndex{}; ///< The action index in collection order.
    };

private:
    /// Build the focused surface chain for the nearest page.
    /// @param page The page.
    /// @return The focused chain ending with the page.
    [[nodiscard]] auto focusedSurfaceChain(const PagePtr &page) const -> std::vector<SurfacePtr>;
    /// Add action candidates for one surface.
    /// @param surface The surface to inspect.
    /// @param chainIndex The surface-chain index.
    /// @param candidates The destination candidate list.
    /// @param seen The already seen actions.
    static void addCandidates(
        const SurfacePtr &surface,
        int chainIndex,
        std::vector<ActionCandidate> &candidates,
        std::vector<ActionPtr> &seen);
    /// Render one action-help item.
    /// @param action The action to render.
    /// @param context The current paint context.
    /// @return The rendered item.
    [[nodiscard]] static auto renderActionText(const ActionPtr &action, const PaintContext &context)
        -> theme::StringWithMargins;
};

}
