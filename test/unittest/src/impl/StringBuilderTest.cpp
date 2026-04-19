// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "TestHelper.hpp"

#include <erbsland/cterm/impl/StringBuilder.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <string>

namespace termimpl = erbsland::cterm::impl;

TESTED_TARGETS(StringBuilder)
class StringBuilderTest final : public UNITTEST_SUBCLASS(TestHelper) {
public:
    void testEmptyBuilderReserveAndClear() {
        auto builder = termimpl::StringBuilder{};

        builder.reserve(32);
        REQUIRE(builder.empty());
        REQUIRE_EQUAL(builder.size(), std::size_t{0});
        REQUIRE_EQUAL(builder.displayWidth(), 0);

        builder.append(Char{U'A'});
        builder.clear();

        REQUIRE(builder.empty());
        REQUIRE_EQUAL(builder.size(), std::size_t{0});
        REQUIRE_EQUAL(builder.displayWidth(), 0);
    }

    void testAppendCharAndStringViewTrackDisplayWidth() {
        auto builder = termimpl::StringBuilder{};
        auto text = String{"BC界"};

        builder.append(Char{U'A', fg::Red, bg::Black});
        builder.append(StringView{text});

        const auto result = builder.toString();
        REQUIRE_EQUAL(render(result), std::string{"ABC界"});
        REQUIRE_EQUAL(builder.size(), std::size_t{4});
        REQUIRE_EQUAL(builder.displayWidth(), 5);
        REQUIRE_EQUAL(result[0].color(), Color(fg::Red, bg::Black));
    }

    void testAppendWithBaseStyleResolvesInheritedComponents() {
        auto builder = termimpl::StringBuilder{};
        auto inherited = String{};
        inherited.append(Char{U'A', Color{fg::Inherited, bg::Blue}, CharAttributes{}});

        auto attributes = CharAttributes{};
        attributes.setBold(true);
        builder.appendWithBaseStyle(StringView{inherited}, CharStyle{Color{fg::Green, bg::Black}, attributes});

        const auto result = builder.toString();
        REQUIRE_EQUAL(render(result), std::string{"A"});
        REQUIRE_EQUAL(result[0].color(), Color(fg::Green, bg::Blue));
        REQUIRE(result[0].attributes().isBold());
    }

    void testAppendStyledUtf32AndUtf8MatchStringSemantics() {
        auto builder = termimpl::StringBuilder{};
        auto attributes = CharAttributes{};
        attributes.setItalic(true);
        const auto style = CharStyle{Color{fg::Yellow, bg::Blue}, attributes};

        builder.appendStyled(U"A\u0301界", style);
        builder.appendStyled(bytes({0x43, 0xE2, 0x28, 0xA1, 0x44}), style, EncodingErrors::Replace);

        const auto result = builder.toString();
        REQUIRE_EQUAL(
            render(result),
            bytes({0x41, 0xCC, 0x81, 0xE7, 0x95, 0x8C, 0x43, 0xEF, 0xBF, 0xBD, 0x28, 0xEF, 0xBF, 0xBD, 0x44}));
        REQUIRE_EQUAL(result.size(), std::size_t{7});
        REQUIRE_EQUAL(result.displayWidth(), 8);
        REQUIRE_EQUAL(result[0].style(), style);
        REQUIRE_EQUAL(result[3], U'\uFFFD');
        REQUIRE_EQUAL(result[5], U'\uFFFD');
    }

    void testAppendStyledFiltersUnsupportedControlCharacters() {
        auto builder = termimpl::StringBuilder{};

        builder.appendStyled("A\r\x01\t\nB", CharStyle{fg::White});

        const auto result = builder.toString();
        REQUIRE_EQUAL(render(result), std::string{"A\t\nB"});
        REQUIRE_EQUAL(result.size(), std::size_t{4});
    }

    void testToStringCopiesAndLeavesBuilderUnchanged() {
        auto builder = termimpl::StringBuilder{};
        builder.appendStyled(U"Alpha", CharStyle{fg::Cyan});

        const auto copy = builder.toString();
        builder.append(Char{U'!'});

        REQUIRE_EQUAL(render(copy), std::string{"Alpha"});
        REQUIRE_EQUAL(render(builder.toString()), std::string{"Alpha!"});
    }

    void testTakeStringClearsBuilderAndAllowsFastReuse() {
        auto builder = termimpl::StringBuilder{};
        builder.reserve(24);
        builder.appendStyled(U"Hello", CharStyle{fg::Green});

        const auto taken = builder.takeString();

        REQUIRE_EQUAL(render(taken), std::string{"Hello"});
        REQUIRE(builder.empty());
        REQUIRE_EQUAL(builder.displayWidth(), 0);

        builder.appendStyled(U"Bye", CharStyle{fg::Magenta});
        REQUIRE_EQUAL(render(builder.toString()), std::string{"Bye"});
    }

private:
    [[nodiscard]] static auto render(const String &text) -> std::string {
        auto result = std::string{};
        for (const auto &character : text) {
            character.appendTo(result);
        }
        return result;
    }
};
