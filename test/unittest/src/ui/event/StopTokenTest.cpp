// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>


namespace ui = erbsland::cterm::ui;


TESTED_TARGETS(UiStopToken UiEventThread UiInvocation)
class StopTokenTest final : public el::UnitTest {
public:
    void testDefaultTokenCannotObserveStops() {
        auto token = ui::StopToken{};

        REQUIRE_FALSE(token.stopPossible());
        REQUIRE_FALSE(token.stopRequested());
    }

    void testSourceCreatesConnectedTokens() {
        auto source = ui::StopSource{};
        auto token = source.getToken();

        REQUIRE(source.stopPossible());
        REQUIRE_FALSE(source.stopRequested());
        REQUIRE(token.stopPossible());
        REQUIRE_FALSE(token.stopRequested());
    }

    void testRequestStopUpdatesExistingAndFutureTokens() {
        auto source = ui::StopSource{};
        auto firstToken = source.getToken();

        REQUIRE(source.requestStop());

        auto secondToken = source.getToken();
        REQUIRE(source.stopRequested());
        REQUIRE(firstToken.stopRequested());
        REQUIRE(secondToken.stopRequested());
    }

    void testRequestStopReturnsFalseAfterTheFirstRequest() {
        auto source = ui::StopSource{};

        REQUIRE(source.requestStop());
        REQUIRE_FALSE(source.requestStop());
    }

    void testCopiedSourceSharesTheSameStopState() {
        auto source = ui::StopSource{};
        auto copiedSource = source;
        auto token = copiedSource.getToken();

        REQUIRE_FALSE(source.stopRequested());
        REQUIRE(copiedSource.requestStop());
        REQUIRE(source.stopRequested());
        REQUIRE(token.stopRequested());
    }

    void testTokensKeepTheSharedStopStateAlive() {
        auto token = ui::StopToken{};
        {
            auto source = ui::StopSource{};
            token = source.getToken();

            REQUIRE(token.stopPossible());
            REQUIRE_FALSE(token.stopRequested());
        }

        REQUIRE(token.stopPossible());
        REQUIRE_FALSE(token.stopRequested());
    }
};
