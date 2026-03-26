// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>

#include <functional>


TESTED_TARGETS(CharAttributes)
class CharAttributesTest final : public el::UnitTest {
public:
    void testDefaultResetAndMaskFactoriesDescribeSpecifiedAndEnabledBits() {
        constexpr auto inherited = CharAttributes{};
        constexpr auto reset = CharAttributes::reset();
        constexpr auto supported = CharAttributes::fromMask(
            static_cast<uint8_t>(CharAttributes::Bold.value | CharAttributes::Underline.value));

        REQUIRE_EQUAL(sizeof(CharAttributes), std::size_t{2});
        REQUIRE_FALSE(inherited.isBoldSpecified());
        REQUIRE_FALSE(inherited.isBold());
        REQUIRE(reset.isBoldSpecified());
        REQUIRE_FALSE(reset.isBold());
        REQUIRE(supported.isBoldSpecified());
        REQUIRE(supported.isBold());
        REQUIRE(supported.isUnderlineSpecified());
        REQUIRE(supported.isUnderline());
        REQUIRE_FALSE(supported.isDimSpecified());
    }

    void testSettersCanEnableDisableAndReinheritIndividualFlags() {
        auto attributes = CharAttributes{};
        attributes.setBold(true);
        attributes.setDim(false);
        attributes.setItalic(true);
        attributes.setUnderline(true);

        REQUIRE(attributes.isBoldSpecified());
        REQUIRE(attributes.isBold());
        REQUIRE(attributes.isDimSpecified());
        REQUIRE_FALSE(attributes.isDim());
        REQUIRE(attributes.isItalic());
        REQUIRE(attributes.isUnderline());

        attributes.setBoldInherited();
        attributes.setUnderlineInherited();

        REQUIRE_FALSE(attributes.isBoldSpecified());
        REQUIRE_FALSE(attributes.isUnderlineSpecified());
        REQUIRE(attributes.isDimSpecified());
        REQUIRE(attributes.isItalicSpecified());
    }

    void testResolvedWithOverwritesOnlySpecifiedFlags() {
        auto base = CharAttributes::reset();
        base.setBold(true);
        base.setItalic(true);
        base.setHidden(true);

        auto overlay = CharAttributes{};
        overlay.setBold(false);
        overlay.setUnderline(true);

        const auto resolved = overlay.resolvedWith(base);

        REQUIRE(resolved.isBoldSpecified());
        REQUIRE_FALSE(resolved.isBold());
        REQUIRE(resolved.isItalic());
        REQUIRE(resolved.isUnderline());
        REQUIRE(resolved.isHidden());
        REQUIRE_FALSE(resolved.isBlink());
    }

    void testEqualityAndHashReflectEnabledAndSpecifiedBits() {
        auto left = CharAttributes{};
        left.setBold(true);
        left.setStrikethrough(false);

        auto equal = left;
        auto different = left;
        different.setBoldInherited();

        REQUIRE(left == equal);
        REQUIRE_FALSE(left != equal);
        REQUIRE_FALSE(left == different);
        REQUIRE_EQUAL(left.hash(), std::hash<CharAttributes>{}(left));
        REQUIRE_NOT_EQUAL(left.hash(), different.hash());
    }

    void testWithFlagCreatesUpdatedCopiesWithoutMutatingTheSource() {
        const auto original = CharAttributes{};
        const auto bold = original.withFlag(CharAttributes::Bold, true);
        const auto noBold = bold.withFlag(CharAttributes::Bold, false);

        REQUIRE_FALSE(original.isBoldSpecified());
        REQUIRE(bold.isBoldSpecified());
        REQUIRE(bold.isBold());
        REQUIRE(noBold.isBoldSpecified());
        REQUIRE_FALSE(noBold.isBold());
    }
};
