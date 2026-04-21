// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Surface.hpp"

namespace erbsland::cterm::ui::surface {

class ScrollCorner;
using ScrollCornerPtr = std::shared_ptr<ScrollCorner>;

/// The one-cell fill surface where horizontal and vertical scroll bars meet.
class ScrollCorner final : public Surface {
public:
    /// Create a scroll corner.
    explicit ScrollCorner(ProtectedTag) noexcept;
    ~ScrollCorner() override = default;

public:
    /// Create a scroll corner.
    /// @return The new scroll corner.
    [[nodiscard]] static auto create() noexcept -> ScrollCornerPtr;

public:
    /// Get the fill character.
    /// @return The fill character.
    [[nodiscard]] auto fill() const noexcept -> const Char &;
    /// Replace the fill character.
    /// @param fill The new fill character.
    void setFill(Char fill) noexcept;

public: // implement Surface
    [[nodiscard]] auto isOpaque() const noexcept -> bool override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

private:
    Char _fill{U' ', Color{fg::Black, bg::BrightBlack}}; ///< The corner fill character.
    bool _fillOverride{false};                           ///< The fill character was explicitly configured.
};

}
