// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/unittest/UnitTest.hpp>


TESTED_TARGETS(UpdateSettings)
class UpdateSettingsTest final : public el::UnitTest {
public:
    void testDefaultSettingsExposeTheExpectedDefaults() {
        const auto &settings = UpdateSettings::defaultSettings();
        const auto expectedMinimumSize = Size{0, 0};

        REQUIRE_EQUAL(settings.minimumSize(), expectedMinimumSize);
        REQUIRE_EQUAL(settings.minimumSizeBackground(), Char::space());
        REQUIRE(settings.minimumSizeMessage().empty());
        REQUIRE_FALSE(settings.showCropMarks());
        REQUIRE_EQUAL(settings.cropMarkRight(), U'▶');
        REQUIRE_EQUAL(settings.cropMarkBottomRight(), U'◢');
        REQUIRE_EQUAL(settings.cropMarkBottom(), U'▼');
        REQUIRE(settings.switchToAlternateBuffer());
        REQUIRE(&UpdateSettings::defaultSettings() == &settings);
    }

    void testSettersUpdateAllStoredValues() {
        auto settings = UpdateSettings{};
        const auto expectedMinimumSize = Size{80, 25};

        settings.setMinimumSize(expectedMinimumSize);
        settings.setMinimumSizeBackground(Char{U'.'});
        settings.setMinimumSizeMessage(String{"Terminal too small"});
        settings.setShowCropMarks(true);
        settings.setCropMarkRight(Char{U'>'});
        settings.setCropMarkBottomRight(Char{U'+'});
        settings.setCropMarkBottom(Char{U'v'});
        settings.setSwitchToAlternateBuffer(false);

        REQUIRE_EQUAL(settings.minimumSize(), expectedMinimumSize);
        REQUIRE_EQUAL(settings.minimumSizeBackground(), U'.');
        REQUIRE_EQUAL(settings.minimumSizeMessage().size(), std::size_t{18});
        REQUIRE(settings.showCropMarks());
        REQUIRE_EQUAL(settings.cropMarkRight(), U'>');
        REQUIRE_EQUAL(settings.cropMarkBottomRight(), U'+');
        REQUIRE_EQUAL(settings.cropMarkBottom(), U'v');
        REQUIRE_FALSE(settings.switchToAlternateBuffer());
    }

    void testApplyToTransfersCropMarkConfigurationToAView() {
        auto settings = UpdateSettings{};
        settings.setShowCropMarks(true);
        settings.setCropMarkRight(Char{U'>'});
        settings.setCropMarkBottomRight(Char{U'+'});
        settings.setCropMarkBottom(Char{U'v'});

        auto view = BufferView{};
        settings.applyTo(view);

        REQUIRE(view.showCropCharacters());
        REQUIRE_EQUAL(view.cropCharacter(Direction::East), U'>');
        REQUIRE_EQUAL(view.cropCharacter(Direction::SouthEast), U'+');
        REQUIRE_EQUAL(view.cropCharacter(Direction::South), U'v');
    }
};
