// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "SingleContentLayout.hpp"

namespace erbsland::cterm::ui::layout {

class Centered;
using CenteredPtr = std::shared_ptr<Centered>;

/// A one-child layout that centers its content inside padded available space.
class Centered final : public SingleContentLayout {
public:
    /// Create a centered layout.
    explicit Centered(ProtectedTag) noexcept;
    ~Centered() override = default;

public:
    /// Create a centered layout.
    /// @return The new centered layout.
    [[nodiscard]] static auto create() -> CenteredPtr;

public:
    /// Access the padding around the centered content.
    /// @return The padding.
    [[nodiscard]] auto padding() const noexcept -> Margins;
    /// Replace the padding around the centered content.
    /// @param padding The new padding.
    void setPadding(Margins padding) noexcept;

public: // implement Surface
    [[nodiscard]] auto onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept
        -> LayoutMetrics override;
    void onLayout(LayoutScope &scope) noexcept override;

private:
    /// Subtract padding from a size.
    [[nodiscard]] static auto contentSize(Size size, Margins padding) noexcept -> Size;
    /// Create a child proposal by removing the collapsed content inset from this layout's proposal.
    [[nodiscard]] static auto contentProposal(LayoutProposal proposal, Margins contentInset) noexcept -> LayoutProposal;
    /// Add the collapsed content inset to content metrics.
    [[nodiscard]] auto paddedMetrics(const LayoutMetrics &contentMetrics, Margins contentInset) const noexcept
        -> LayoutMetrics;

private:
    Margins _padding; ///< Padding around the centered content.
};

}
