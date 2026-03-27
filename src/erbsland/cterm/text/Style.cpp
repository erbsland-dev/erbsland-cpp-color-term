// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Style.hpp"

#include <algorithm>
#include <limits>
#include <ranges>


namespace erbsland::cterm::text {


Style::Style() :
    _baseTextStyle{Color{fg::White, bg::Black}},
    _baseBlockLayout{0, ParagraphIndents::cUseLineIndent, ParagraphIndents::cUseLineIndent, Margins{0}} {
    edit(StyleSelector::strong()).setTextStyle(fg::Inherited, CharAttributes::Bold);
    edit(StyleSelector::emphasis()).setTextStyle(fg::Inherited, CharAttributes::Italic);
    edit(StyleSelector::underline()).setTextStyle(fg::Inherited, CharAttributes::Underline);
    edit(StyleSelector::link()).setTextStyle(fg::Inherited, CharAttributes::Underline);
    edit(StyleSelector::definitionDescription()).setLineIndent(4);
    edit(StyleSelector{StyleRole::ListItem, std::nullopt, StyleListKind::Any}).setWrappedLineIndent(4);
    edit(StyleSelector::listItem(StyleListKind::Bullet, 0)).setLiteralMarker(U"•\t");
    edit(StyleSelector::listItem(StyleListKind::Bullet, 1)).setLiteralMarker(U"‣\t");
    edit(StyleSelector::listItem(StyleListKind::Bullet, 2)).setLiteralMarker(U"⁃\t");
    edit(StyleSelector::listItem(StyleListKind::Bullet, 3)).setLiteralMarker(U"◦\t");
    edit(StyleSelector{StyleRole::ListItem, std::nullopt, StyleListKind::Numbered}).setOrderedMarker(U".\t");
}


auto Style::clone() const -> StylePtr {
    return std::make_shared<Style>(*this);
}


auto Style::baseTextStyle() const noexcept -> const CharStyle & {
    return _baseTextStyle;
}


void Style::setBaseTextStyle(const CharStyle &style) noexcept {
    _baseTextStyle = style;
}


auto Style::baseBlockLayout() const noexcept -> const ParagraphIndents & {
    return _baseBlockLayout;
}


void Style::setBaseBlockLayout(const ParagraphIndents &layout) noexcept {
    _baseBlockLayout = layout;
}


auto Style::definition(const StyleSelector &selector) const noexcept
    -> std::optional<std::reference_wrapper<const StyleRule>> {
    const auto iterator = findEntry(selector);
    if (iterator == _definitions.end()) {
        return std::nullopt;
    }
    return iterator->rule;
}


auto Style::edit(const StyleSelector &selector) -> StyleRule & {
    const auto normalized = normalizedSelector(selector);
    if (auto iterator = findEntry(normalized); iterator != _definitions.end()) {
        return iterator->rule;
    }
    const auto combinedTokens = std::span<const std::string>{normalized.requiredStyleTokens()};
    auto resolved = resolve(
        normalized,
        StyleMatchContext{normalized.styleRole(), normalized.level(), normalized.listKind(), combinedTokens});
    _definitions.push_back(Entry{.selector = normalized, .rule = std::move(resolved)});
    return _definitions.back().rule;
}


void Style::erase(const StyleSelector &selector) noexcept {
    if (auto iterator = findEntry(selector); iterator != _definitions.end()) {
        _definitions.erase(iterator);
    }
}


auto Style::resolve(const StyleSelector &selector, const StyleMatchContext &context) const -> StyleRule {
    const auto effectiveContext = mergeSelectorIntoContext(selector, context);
    const auto combinedTokens = combineSelectorAndContextTokens(selector, effectiveContext.styleTokens());
    const auto combinedTokenSpan = std::span<const std::string>{combinedTokens};
    auto resolved = defaultRuleFor(effectiveContext.role(), effectiveContext.listKind(), effectiveContext.level());

    auto bestPass = std::numeric_limits<int>::max();
    auto bestLevelRank = -1;
    auto bestTokenCount = -1;
    auto bestLevelValue = std::numeric_limits<int>::min();
    auto hasMatch = false;
    for (const auto &entry : _definitions) {
        if (entry.selector.styleRole() != effectiveContext.role()) {
            continue;
        }
        if (!tokensMatch(entry.selector.requiredStyleTokens(), combinedTokenSpan)) {
            continue;
        }

        const auto tokenSpecific = !entry.selector.requiredStyleTokens().empty();
        const auto kindExact = entry.selector.listKind() == effectiveContext.listKind();
        const auto kindAny = entry.selector.listKind() == ListKind::Any;
        if (!kindExact && !kindAny) {
            continue;
        }

        auto levelRank = 0;
        auto levelValue = std::numeric_limits<int>::min();
        if (entry.selector.level().has_value()) {
            if (!effectiveContext.level().has_value()) {
                continue;
            }
            if (*entry.selector.level() > *effectiveContext.level()) {
                continue;
            }
            levelValue = *entry.selector.level();
            levelRank = *entry.selector.level() == *effectiveContext.level() ? 2 : 1;
        }

        auto pass = 6;
        if (tokenSpecific) {
            if (kindExact) {
                pass = levelRank == 2 ? 1 : (levelRank == 1 ? 2 : 3);
            } else {
                pass = 4;
            }
        } else if (kindExact) {
            pass = 5;
        }

        const auto tokenCount = static_cast<int>(entry.selector.requiredStyleTokens().size());
        const auto isBetter = !hasMatch || pass < bestPass || (pass == bestPass && levelRank > bestLevelRank) ||
            (pass == bestPass && levelRank == bestLevelRank && tokenCount > bestTokenCount) ||
            (pass == bestPass && levelRank == bestLevelRank && tokenCount == bestTokenCount &&
             levelValue > bestLevelValue);
        if (!isBetter) {
            continue;
        }
        bestPass = pass;
        bestLevelRank = levelRank;
        bestTokenCount = tokenCount;
        bestLevelValue = levelValue;
        resolved = entry.rule;
        hasMatch = true;
    }

    return resolved;
}


auto Style::defaultStyle(const Predefined predefined) noexcept -> const StyleConstPtr & {
    switch (predefined) {
    case Predefined::Plain:
        return defaultPlain();
    case Predefined::Simple:
        return defaultSimple();
    case Predefined::Styled:
        return defaultStyled();
    }
    return defaultPlain();
}


auto Style::defaultPlain() noexcept -> const StyleConstPtr & {
    static const StyleConstPtr cPlainStyle = std::make_shared<Style>();
    return cPlainStyle;
}


auto Style::defaultSimple() noexcept -> const StyleConstPtr & {
    static const auto cSimpleStyle = createSimpleDefaultStyle();
    return cSimpleStyle;
}


auto Style::defaultStyled() noexcept -> const StyleConstPtr & {
    static const auto cStyledStyle = createStyledDefaultStyle();
    return cStyledStyle;
}


auto Style::createSimpleDefaultStyle() -> StyleConstPtr {
    using Selector = StyleSelector;

    auto value = std::make_shared<Style>();
    value->edit(Selector::paragraph()).setMargins(0, 0, 1, 0);
    value->edit(Selector::heading(1)).setTextStyle(fg::BrightYellow, CharAttributes::Bold).setMargins(2, 0, 1, 0);
    value->edit(Selector::heading(2)).setTextStyle(fg::BrightCyan, CharAttributes::Bold).setMargins(0, 0, 1, 0);
    value->edit(Selector::heading(3)).setTextStyle(fg::BrightGreen, CharAttributes::Bold);
    value->edit(Selector::definitionList()).setMargins(0, 0, 1, 0);
    value->edit(Selector::definitionTerm()).setTextStyle(fg::BrightYellow, CharAttributes::Bold);
    value->edit(Selector::definitionDescription()).setMargins(0);
    value->edit(Selector::codeBlock()).setMargins(1, 0);
    value->edit(Selector::listContainer(ListKind::Numbered, 0)).setMargins(0, 0, 1, 0);
    value->edit(Selector::listContainer(ListKind::Numbered, 1)).setMargins(0);
    value->edit(Selector::listContainer(ListKind::Bullet, 0)).setMargins(0, 0, 1, 0);
    value->edit(Selector::listContainer(ListKind::Bullet, 1)).setMargins(0);
    value->edit(Selector::listItem(ListKind::Bullet, 0)).setIndents(ParagraphIndents{0, 0, 4, Margins{0}});
    value->edit(Selector::listItem(ListKind::Numbered, 0)).setIndents(ParagraphIndents{0, 0, 4, Margins{0}});
    value->edit(Selector::horizontalRule()).setMargins(0, 1);
    return value;
}


auto Style::createStyledDefaultStyle() -> StyleConstPtr {
    using Selector = StyleSelector;

    auto value = std::make_shared<Style>();
    value->edit(Selector::paragraph()).setMargins(0, 2, 1, 6);
    value->edit(Selector::heading(1))
        .setMargins(3, 1, 1, 2)
        .setTextStyle(fg::BrightYellow, CharAttributes::Bold)
        .setPrefix(U"─▸◆ ", CharStyle{fg::Yellow})
        .setSuffix(U" ◆◂─", CharStyle{fg::Yellow})
        .setLineFill(U'─', fg::BrightBlack);
    value->edit(Selector::heading(2))
        .setMargins(2, 2, 1, 3)
        .setTextStyle(fg::BrightCyan, CharAttributes::Bold)
        .setPrefix(U"-◆ ", CharStyle{fg::Cyan})
        .setSuffix(U" ◆-", CharStyle{fg::Cyan});
    value->edit(Selector::heading(3)).setMargins(1, 2, 1, 6).setTextStyle(fg::BrightGreen, CharAttributes::Bold);
    value->edit(Selector::definitionTerm()).setTextStyle(fg::BrightYellow, CharAttributes::Bold).setMargins(0, 2, 0, 6);
    value->edit(Selector::definitionDescription()).setMargins(0, 2, 1, 10);
    value->edit(Selector::codeBlock()).setMargins(0, 2, 1, 8);
    value->edit(Selector::listContainer(ListKind::Numbered, 0)).setMargins(0, 2, 1, 2);
    value->edit(Selector::listContainer(ListKind::Numbered, 1)).setMargins(0);
    value->edit(Selector::listContainer(ListKind::Bullet, 0)).setMargins(0, 2, 1, 3);
    value->edit(Selector::listContainer(ListKind::Bullet, 1)).setMargins(0);
    value->edit(Selector::listItem(ListKind::Bullet, 0))
        .setIndents(ParagraphIndents{0, 0, 3, Margins{0}})
        .setLiteralMarker(U"•\t", CharStyle{fg::Yellow});
    value->edit(Selector::listItem(ListKind::Bullet, 1)).setLiteralMarker(U"︎︎︎⁃\t", CharStyle{fg::Cyan});
    value->edit(Selector::listItem(ListKind::Bullet, 2)).setLiteralMarker(U"‣\t", CharStyle{fg::Green});
    value->edit(Selector::horizontalRule())
        .setLineFill(U'─', fg::Magenta)
        .setMargins(2)
        .setPrefix(U"◆◂", {fg::Magenta})
        .setSuffix(U"▸◆", {fg::Magenta});
    return value;
}


auto Style::normalizedSelector(StyleSelector selector) -> StyleSelector {
    std::ranges::sort(selector.requiredStyleTokens());
    selector.requiredStyleTokens().erase(
        std::ranges::unique(selector.requiredStyleTokens()).begin(), selector.requiredStyleTokens().end());
    return selector;
}


auto Style::defaultRuleFor(const Role role, const ListKind listKind, const std::optional<int> level) const noexcept
    -> StyleRule {
    auto rule = StyleRule{};
    rule.setTextStyle(CharStyle{});
    rule.setIndents(_baseBlockLayout);
    if (role == Role::ListItem && listKind == ListKind::Any && level.has_value()) {
        rule.setIndents(ParagraphIndents{0, 0, 4, Margins{0}});
    }
    return rule;
}


auto Style::mergeSelectorIntoContext(const StyleSelector &selector, const StyleMatchContext &context)
    -> StyleMatchContext {
    auto result = context;
    result.setRole(selector.styleRole());
    if (selector.level().has_value()) {
        result.setLevel(selector.level());
    }
    if (selector.listKind() != ListKind::Any || context.listKind() == ListKind::Any) {
        result.setListKind(selector.listKind());
    }
    return result;
}


auto Style::combineSelectorAndContextTokens(
    const StyleSelector &selector, const std::span<const std::string> contextTokens) -> std::vector<std::string> {
    auto tokens = std::vector<std::string>{};
    tokens.reserve(selector.requiredStyleTokens().size() + contextTokens.size());
    for (const auto &token : selector.requiredStyleTokens()) {
        tokens.push_back(token);
    }
    for (const auto &token : contextTokens) {
        tokens.push_back(token);
    }
    std::ranges::sort(tokens);
    tokens.erase(std::ranges::unique(tokens).begin(), tokens.end());
    return tokens;
}


auto Style::tokensMatch(
    const std::span<const std::string> requiredTokens, const std::span<const std::string> contextTokens) noexcept
    -> bool {
    if (requiredTokens.empty()) {
        return true;
    }
    for (const auto &token : requiredTokens) {
        if (std::ranges::find(contextTokens, token) == contextTokens.end()) {
            return false;
        }
    }
    return true;
}


auto Style::findEntry(const StyleSelector &selector) noexcept -> std::vector<Entry>::iterator {
    const auto normalized = normalizedSelector(selector);
    return std::ranges::find(_definitions, normalized, &Entry::selector);
}


auto Style::findEntry(const StyleSelector &selector) const noexcept -> std::vector<Entry>::const_iterator {
    const auto normalized = normalizedSelector(selector);
    return std::ranges::find(_definitions, normalized, &Entry::selector);
}


}
