// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TerminalTestSupport.hpp"

#include <erbsland/cterm/ui/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

namespace ui = erbsland::cterm::ui;


TESTED_TARGETS(UiTextBox)
class UiTextBoxTest final : public UNITTEST_SUBCLASS(TerminalTestHelper) {
public:
    void testCreateUsesTheTextDimensionsAsPreferredSize() {
        const auto textBox = ui::TextBox::create(String{U"AB\n\nC\n"});

        requireStringEqual(textBox->text(), U"AB\n\nC\n");
        REQUIRE_EQUAL(textBox->geometry().preferred(), Size(2, 3));
    }

    void testSetTextRecomputesPreferredSizeWithoutChangingContentSemantics() {
        const auto textBox = ui::TextBox::create("X");

        textBox->setText(String{U"中A\nB"});

        requireStringEqual(textBox->text(), U"中A\nB");
        REQUIRE_EQUAL(textBox->geometry().preferred(), Size(3, 2));
    }

private:
    void requireStringEqual(const String &actual, const std::u32string_view expected) {
        REQUIRE_EQUAL(actual.size(), expected.size());
        for (std::size_t i = 0; i < expected.size(); ++i) {
            REQUIRE_EQUAL(actual[i], expected[i]);
        }
    }
};
