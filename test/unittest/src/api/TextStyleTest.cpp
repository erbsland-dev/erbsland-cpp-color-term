// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/text/Style.hpp>
#include <erbsland/unittest/UnitTest.hpp>

namespace text = erbsland::cterm::text;

TESTED_TARGETS(Style)
class TextStyleTest final : public el::UnitTest {
public:
    void testStyleSelectorFactoriesSupportConstantEvaluatedAccessors() {
        static_assert(text::StyleSelector::paragraph().styleRole() == text::StyleRole::Paragraph);
        static_assert(
            text::StyleSelector::listItem(text::StyleListKind::Bullet, 2).styleRole() == text::StyleRole::ListItem);
        static_assert(text::StyleSelector::listItem(text::StyleListKind::Bullet, 2).level() == 2);
        static_assert(
            text::StyleSelector::listItem(text::StyleListKind::Bullet, 2).listKind() == text::StyleListKind::Bullet);

        const auto paragraph = text::StyleSelector::paragraph();
        const auto listItem = text::StyleSelector::listItem(text::StyleListKind::Bullet, 2);

        REQUIRE_EQUAL(paragraph.styleRole(), text::StyleRole::Paragraph);
        REQUIRE_EQUAL(listItem.listKind(), text::StyleListKind::Bullet);
    }

    void testStyleSelectorConstructorsSupportTokenSpecificSelectors() {
        const auto selector = text::StyleSelector{text::StyleRole::Paragraph, {"featured", "warning"}};

        REQUIRE_EQUAL(selector.styleRole(), text::StyleRole::Paragraph);
        REQUIRE_EQUAL(selector.listKind(), text::StyleListKind::Any);
        REQUIRE_EQUAL(selector.requiredStyleTokens(), std::vector<std::string>({"featured", "warning"}));
    }

    void testEditCreatesDefinitionsFromResolvedBaseAndEraseRemovesThem() {
        auto style = text::Style{};
        const auto selector = text::StyleSelector::paragraph();

        REQUIRE_FALSE(style.definition(selector).has_value());

        auto &rule = style.edit(selector);
        rule.setMargins(1, 2, 3, 4);

        REQUIRE(style.definition(selector).has_value());
        REQUIRE_EQUAL(style.resolve(selector, matchContext(selector)).margins(), (Margins{1, 2, 3, 4}));

        style.erase(selector);

        REQUIRE_FALSE(style.definition(selector).has_value());
        REQUIRE_EQUAL(style.resolve(selector, matchContext(selector)).margins(), Margins{0});
    }

    void testCloneCreatesIndependentMutableCopy() {
        auto style = text::Style{};
        auto clone = style.clone();

        clone->edit(text::StyleSelector::paragraph()).setPrefix(U"[clone] ");
        clone->edit(text::StyleSelector::heading(1)).setSuffix(U" *");

        REQUIRE_FALSE(style.resolve(text::StyleSelector::paragraph(), matchContext(text::StyleSelector::paragraph()))
                          .prefix()
                          .has_value());
        REQUIRE_EQUAL(
            toPlainText(
                *clone->resolve(text::StyleSelector::paragraph(), matchContext(text::StyleSelector::paragraph()))
                     .prefix()),
            std::string{"[clone] "});
        REQUIRE_FALSE(style.resolve(text::StyleSelector::heading(1), matchContext(text::StyleSelector::heading(1)))
                          .suffix()
                          .has_value());
        REQUIRE_EQUAL(
            toPlainText(*clone->resolve(text::StyleSelector::heading(1), matchContext(text::StyleSelector::heading(1)))
                             .suffix()),
            std::string{" *"});
    }

    void testResolvePrefersMostSpecificStyleTokenMatch() {
        auto style = text::Style{};
        style.edit(text::StyleSelector{text::StyleRole::Paragraph, {"warning"}}).setPrefix(U"[warning] ");
        style.edit(text::StyleSelector{text::StyleRole::Paragraph, {"featured", "warning"}})
            .setPrefix(U"[featured-warning] ");

        const auto selector = text::StyleSelector::paragraph();
        const auto resolved = style.resolve(selector, matchContext(selector, {"warning", "featured"}));

        REQUIRE(resolved.prefix().has_value());
        REQUIRE_EQUAL(toPlainText(*resolved.prefix()), std::string{"[featured-warning] "});
    }

    void testResolveUsesNearestLowerDefinedHeadingLevelWithoutClamp() {
        auto style = text::Style{};
        style.edit(text::StyleSelector::heading(1)).setPrefix(U"[h1] ");
        style.edit(text::StyleSelector::heading(4)).setPrefix(U"[h4] ");

        const auto resolved =
            style.resolve(text::StyleSelector::heading(12), matchContext(text::StyleSelector::heading(12)));

        REQUIRE(resolved.prefix().has_value());
        REQUIRE_EQUAL(toPlainText(*resolved.prefix()), std::string{"[h4] "});
    }

    void testDefaultStyleUsesMarkersAndListFallbackBeyondPreviousLimit() {
        auto style = text::Style{};

        const auto bulletRule = style.resolve(
            text::StyleSelector::listItem(text::StyleListKind::Bullet, 12),
            matchContext(text::StyleSelector::listItem(text::StyleListKind::Bullet, 12)));
        const auto bulletMarker = bulletRule.marker().render(1, CharStyle{}, bulletRule.indents().wrappedLineIndent());

        REQUIRE_EQUAL(toPlainText(bulletMarker.text), std::string{"◦   "});
        REQUIRE_EQUAL(bulletRule.indents().wrappedLineIndent(), 4);

        const auto numberedRule = style.resolve(
            text::StyleSelector::listItem(text::StyleListKind::Numbered, 12),
            matchContext(text::StyleSelector::listItem(text::StyleListKind::Numbered, 12)));
        const auto numberedMarker =
            numberedRule.marker().render(27, CharStyle{}, numberedRule.indents().wrappedLineIndent());

        REQUIRE_EQUAL(toPlainText(numberedMarker.text), std::string{"27. "});
    }

    void testPredefinedDefaultStylesAreSharedAndDistinct() {
        const auto &plain = text::Style::defaultStyle();
        const auto &simple = text::Style::defaultStyle(text::Style::Predefined::Simple);
        const auto &styled = text::Style::defaultStyle(text::Style::Predefined::Styled);

        REQUIRE(plain == text::Style::defaultStyle(text::Style::Predefined::Plain));
        REQUIRE(simple == text::Style::defaultStyle(text::Style::Predefined::Simple));
        REQUIRE(styled == text::Style::defaultStyle(text::Style::Predefined::Styled));
        REQUIRE(plain != simple);
        REQUIRE(plain != styled);
        REQUIRE(simple != styled);
    }

    void testSimplePredefinedDefaultStyleMatchesCompactViewerLayout() {
        const auto &style = text::Style::defaultStyle(text::Style::Predefined::Simple);

        const auto paragraphRule =
            style->resolve(text::StyleSelector::paragraph(), matchContext(text::StyleSelector::paragraph()));
        REQUIRE_EQUAL(paragraphRule.margins(), (Margins{0, 0, 1, 0}));

        const auto headingRule =
            style->resolve(text::StyleSelector::heading(1), matchContext(text::StyleSelector::heading(1)));
        REQUIRE_EQUAL(headingRule.textStyle(), CharStyle(fg::BrightYellow, CharAttributes::Bold));
        REQUIRE_EQUAL(headingRule.margins(), (Margins{2, 0, 1, 0}));

        const auto rule =
            style->resolve(text::StyleSelector::horizontalRule(), matchContext(text::StyleSelector::horizontalRule()));
        REQUIRE_EQUAL(rule.margins(), (Margins{1, 0, 1, 0}));
    }

    void testStyledPredefinedDefaultStyleMatchesExtendedViewerDecorations() {
        const auto &style = text::Style::defaultStyle(text::Style::Predefined::Styled);

        const auto paragraphRule =
            style->resolve(text::StyleSelector::paragraph(), matchContext(text::StyleSelector::paragraph()));
        REQUIRE_EQUAL(paragraphRule.margins(), (Margins{0, 2, 1, 6}));

        const auto headingRule =
            style->resolve(text::StyleSelector::heading(1), matchContext(text::StyleSelector::heading(1)));
        REQUIRE(headingRule.prefix().has_value());
        REQUIRE_EQUAL(toPlainText(*headingRule.prefix()), std::string{"─▸◆ "});
        REQUIRE(headingRule.suffix().has_value());
        REQUIRE_EQUAL(toPlainText(*headingRule.suffix()), std::string{" ◆◂─"});
        REQUIRE(headingRule.lineFill().has_value());
        REQUIRE_EQUAL(headingRule.lineFill()->charStr(), std::string{"─"});

        const auto rule =
            style->resolve(text::StyleSelector::horizontalRule(), matchContext(text::StyleSelector::horizontalRule()));
        REQUIRE(rule.prefix().has_value());
        REQUIRE_EQUAL(toPlainText(*rule.prefix()), std::string{"◆◂"});
        REQUIRE(rule.suffix().has_value());
        REQUIRE_EQUAL(toPlainText(*rule.suffix()), std::string{"▸◆"});
        REQUIRE(rule.lineFill().has_value());
        REQUIRE_EQUAL(rule.lineFill()->style(), CharStyle{fg::Magenta});
    }

    void testStyleRuleConvenienceApiSupportsFluentConfiguration() {
        auto rule = text::StyleRule{};
        rule.setTextStyle(fg::BrightYellow, CharAttributes::Bold)
            .setMargins(1, 2, 3, 4)
            .setLineIndent(5)
            .setFirstLineIndent(6)
            .setWrappedLineIndent(7)
            .setPrefix(U"[")
            .setSuffix(U"]")
            .setLineFill(U'=', CharStyle{fg::Green})
            .setLiteralMarker(U"◆ ", CharStyle{fg::Cyan});

        REQUIRE_EQUAL(rule.textStyle(), CharStyle(fg::BrightYellow, CharAttributes::Bold));
        REQUIRE_EQUAL(rule.margins(), (Margins{1, 2, 3, 4}));
        REQUIRE_EQUAL(rule.indents().lineIndent(), 5);
        REQUIRE_EQUAL(rule.indents().firstLineIndent(), 6);
        REQUIRE_EQUAL(rule.indents().wrappedLineIndent(), 7);
        REQUIRE(rule.prefix().has_value());
        REQUIRE_EQUAL(toPlainText(*rule.prefix()), std::string{"["});
        REQUIRE(rule.suffix().has_value());
        REQUIRE_EQUAL(toPlainText(*rule.suffix()), std::string{"]"});
        REQUIRE(rule.lineFill().has_value());
        REQUIRE_EQUAL(rule.lineFill()->style(), CharStyle{fg::Green});
        REQUIRE_EQUAL(rule.marker().kind(), text::StyleMarker::Kind::Literal);
        REQUIRE_EQUAL(toPlainText(rule.marker().literal()), std::string{"◆ "});
    }

    void testMarkerRenderExpandsTabsAndFallsBackToOneSpaceAfterOverflow() {
        auto marker = text::StyleMarker{};
        marker.setLiteral(U"----\t");

        const auto rendered = marker.render(1, CharStyle{}, 4);

        REQUIRE_EQUAL(toPlainText(rendered.text), std::string{"---- "});
        REQUIRE_EQUAL(rendered.width, 5);
    }

private:
    [[nodiscard]] static auto
    matchContext(const text::StyleSelector &selector, std::initializer_list<std::string> styleTokens = {})
        -> text::StyleMatchContext {
        auto tokens = std::vector<std::string>{styleTokens};
        _tokens = std::move(tokens);
        return text::StyleMatchContext{
            selector.styleRole(), selector.level(), selector.listKind(), std::span<const std::string>{_tokens}};
    }

    [[nodiscard]] static auto toPlainText(const String &textValue) -> std::string {
        auto result = std::string{};
        for (const auto &character : textValue) {
            result += character.charStr();
        }
        return result;
    }

private:
    inline static auto _tokens = std::vector<std::string>{};
};
