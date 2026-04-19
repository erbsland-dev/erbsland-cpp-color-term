// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0

#include "EmbeddedHtml.hpp"

#include <erbsland/cterm/CursorBuffer.hpp>
#include <erbsland/cterm/text/HtmlRenderer.hpp>

#include <array>
#include <chrono>
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <random>

namespace profiling::htmlrenderer {

using namespace erbsland::cterm;

constexpr auto cIterationCount = std::size_t{2'000};
constexpr auto cMinimumWidth = 40;
constexpr auto cMaximumWidth = 200;
constexpr auto cWidthSeed = std::uint32_t{0x5EED1234U};

[[nodiscard]] auto documentStyle() -> const text::StyleConstPtr & {
    return text::Style::defaultStyled();
}

[[nodiscard]] auto documentBaseColor() -> Color {
    return documentStyle()->baseTextStyle().color();
}

[[nodiscard]] auto createWidthSequence() -> std::array<int, cIterationCount> {
    auto widths = std::array<int, cIterationCount>{};
    auto generator = std::minstd_rand{cWidthSeed};
    auto distribution = std::uniform_int_distribution<int>{cMinimumWidth, cMaximumWidth};
    for (auto &width : widths) {
        width = distribution(generator);
    }
    return widths;
}

[[nodiscard]] auto hashBuffer(const CursorBuffer &buffer) -> std::uint64_t {
    auto hash = std::uint64_t{1469598103934665603ULL};
    for (auto y = 0; y < buffer.size().height(); ++y) {
        for (auto x = 0; x < buffer.size().width(); ++x) {
            hash ^= static_cast<std::uint64_t>(buffer.get(Position{x, y}).hash());
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}

auto main() -> int {
    const auto widths = createWidthSequence();
    const auto baseColor = documentBaseColor();
    const auto html = cEmbeddedHtml;
    const auto style = documentStyle();
    auto documentBuffer = std::make_shared<CursorBuffer>(
        Size{cMaximumWidth, 1},
        CursorBuffer::OverflowMode::ExpandThenShift,
        Size{cMaximumWidth, 20'000},
        Char{U' ', baseColor});
    documentBuffer->reserve(Size{cMaximumWidth, 20'000});

    const auto startTime = std::chrono::steady_clock::now();
    for (const auto width : widths) {
        documentBuffer->resize(Size{width, 1}, BufferResizeMode::Fast, Char{U' ', baseColor});
        documentBuffer->setColor(baseColor);
        documentBuffer->clearScreen();
        documentBuffer->moveHome();
        text::HtmlRenderer{html, style}.renderTo(documentBuffer);
    }
    const auto elapsed = std::chrono::steady_clock::now() - startTime;

    std::cout << std::format(
        "html-renderer-profile iterations={} seed={} final-size={}x{} checksum={} elapsed-ms={}\n",
        cIterationCount,
        cWidthSeed,
        documentBuffer->size().width(),
        documentBuffer->size().height(),
        hashBuffer(*documentBuffer),
        std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count());
    return 0;
}

}

auto main() -> int {
    return profiling::htmlrenderer::main();
}
