// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../../Style.hpp"
#include "../../TextNode.hpp"

#include <optional>
#include <string_view>
#include <vector>

namespace erbsland::cterm::text::impl::text_node_renderer {

/// Resolves renderer-facing style rules for text nodes.
class StyleResolver final {
public:
    /// Create a resolver for the given style.
    /// @param style The style used during rendering. `nullptr` falls back to the default style.
    explicit StyleResolver(StyleConstPtr style);

    // defaults
    ~StyleResolver() = default;
    StyleResolver(const StyleResolver &) = delete;
    StyleResolver(StyleResolver &&) = delete;
    auto operator=(const StyleResolver &) -> StyleResolver & = delete;
    auto operator=(StyleResolver &&) -> StyleResolver & = delete;

public:
    /// Access the base text style.
    /// @return The configured base text style.
    [[nodiscard]] auto baseTextStyle() const noexcept -> const CharStyle & { return _style->baseTextStyle(); }
    /// Resolve the block rule for one text node.
    /// @param node The node to resolve.
    /// @param role The semantic role.
    /// @param listKind Optional list kind qualifier.
    /// @param levelOverride Optional explicit level override.
    /// @return The resolved rule.
    [[nodiscard]] auto blockRule(
        const TextNode &node,
        StyleRole role,
        StyleListKind listKind = StyleListKind::Any,
        std::optional<int> levelOverride = std::nullopt) -> StyleRule;
    /// Resolve the horizontal-rule style.
    /// @return The resolved rule.
    [[nodiscard]] auto horizontalRuleRule() -> StyleRule {
        return resolveNoStyle(StyleRole::HorizontalRule, std::nullopt, StyleListKind::Any);
    }
    /// Split and normalize style tokens.
    /// @param value The raw style value.
    /// @return The unique sorted tokens.
    [[nodiscard]] static auto splitStyleTokens(std::string_view value) -> std::vector<std::string>;

private:
    struct CacheEntry final {
        StyleRole role{StyleRole::Paragraph};
        std::optional<int> level;
        StyleListKind listKind{StyleListKind::Any};
        StyleRule rule;
    };

    [[nodiscard]] auto resolveNoStyle(StyleRole role, std::optional<int> level, StyleListKind listKind) -> StyleRule;
    [[nodiscard]] auto resolveWithStyleTokens(
        StyleRole role, std::optional<int> level, StyleListKind listKind, std::string_view styleValue) const
        -> StyleRule;

private:
    StyleConstPtr _style;               ///< The style used for all selector resolution.
    std::vector<CacheEntry> _ruleCache; ///< Cached rules for selector-only lookups.
};

}
