// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "support/TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <functional>

TESTED_TARGETS(CharStyle)
class CharStyleTest final : public el::UnitTest {
public:
    void testConstructionAccessorsAndHashReflectColorAndAttributes() {
        auto attributes = CharAttributes{};
        attributes.setBold(true);

        const auto inherited = CharStyle{};
        const auto styled = CharStyle{Color{fg::Yellow, bg::Blue}, attributes};

        REQUIRE_EQUAL(inherited.color(), Color{});
        REQUIRE_EQUAL(inherited.attributes(), CharAttributes{});
        REQUIRE_EQUAL(styled.color(), Color(fg::Yellow, bg::Blue));
        REQUIRE(styled.attributes().isBold());
        REQUIRE_EQUAL(styled.hash(), std::hash<CharStyle>{}(styled));
    }

    void testSettersUpdateIndividualStyleParts() {
        auto style = CharStyle{};
        auto attributes = CharAttributes{};
        attributes.setUnderline(true);

        style.setFg(fg::Green);
        style.setBg(bg::Black);
        style.setAttributes(attributes);

        REQUIRE_EQUAL(style.fg(), Foreground{fg::Green});
        REQUIRE_EQUAL(style.bg(), Background{bg::Black});
        REQUIRE(style.attributes().isUnderline());
    }

    void testOverlayAndBaseCombineColorAndAttributesTogether() {
        auto baseAttributes = CharAttributes{};
        baseAttributes.setBold(true);
        auto overlayAttributes = CharAttributes{};
        overlayAttributes.setBold(false);
        overlayAttributes.setItalic(true);

        const auto base = CharStyle{Color{fg::Green, bg::Blue}, baseAttributes};
        const auto overlay = CharStyle{Color{fg::Inherited, bg::Black}, overlayAttributes};

        const auto overlaid = base.withOverlay(overlay);
        REQUIRE_EQUAL(overlaid.color(), Color(fg::Green, bg::Black));
        REQUIRE_FALSE(overlaid.attributes().isBold());
        REQUIRE(overlaid.attributes().isItalic());

        const auto reappliedBase = overlay.withBase(base);
        REQUIRE_EQUAL(reappliedBase, overlaid);
    }
};
