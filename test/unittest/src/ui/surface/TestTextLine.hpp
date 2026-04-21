// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/TestHelper.hpp"

#include <erbsland/cterm/ui/surface/AbstractLine.hpp>

#include <array>
#include <memory>

class TestTextLine final : public ui::AbstractLine {
public:
    explicit TestTextLine(ProtectedTag) noexcept : AbstractLine{ProtectedTag{}} {}

    [[nodiscard]] static auto create() noexcept -> std::shared_ptr<TestTextLine> {
        return std::make_shared<TestTextLine>(ProtectedTag{});
    }

public:
    [[nodiscard]] auto text(const Section section) const -> const String & override {
        return _texts[static_cast<std::size_t>(section)];
    }

    [[nodiscard]] auto collapseBehavior(const Section section) const noexcept -> CollapseBehavior override {
        return _behaviors[static_cast<std::size_t>(section)];
    }

    [[nodiscard]] auto margins(const Section section) const -> Margins override {
        return _margins[static_cast<std::size_t>(section)];
    }

    void setText(const Section section, String text) { _texts[static_cast<std::size_t>(section)] = std::move(text); }

    void setCollapseBehavior(const Section section, const CollapseBehavior behavior) noexcept {
        _behaviors[static_cast<std::size_t>(section)] = behavior;
    }

    void setMargins(const Section section, const Margins margins) noexcept {
        _margins[static_cast<std::size_t>(section)] = margins;
    }

private:
    std::array<String, 3> _texts;
    std::array<CollapseBehavior, 3> _behaviors{
        CollapseBehavior::Ellipsis,
        CollapseBehavior::Ellipsis,
        CollapseBehavior::Ellipsis,
    };
    std::array<Margins, 3> _margins{};
};
