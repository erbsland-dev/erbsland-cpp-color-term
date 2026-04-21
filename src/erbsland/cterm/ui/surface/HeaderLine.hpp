// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "TextLine.hpp"

namespace erbsland::cterm::ui::surface {

class HeaderLine;
using HeaderLinePtr = std::shared_ptr<HeaderLine>;

/// A themed header line.
class HeaderLine final : public TextLine {
public:
    /// Create a header line with fixed height `1`.
    explicit HeaderLine(ProtectedTag) noexcept;
    ~HeaderLine() override = default;

public:
    /// Create a header line.
    /// @return The new header line.
    [[nodiscard]] static auto create() noexcept -> HeaderLinePtr;
};

}
