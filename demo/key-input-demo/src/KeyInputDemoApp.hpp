// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <erbsland/cterm/all.hpp>

#include <array>
#include <chrono>
#include <random>


namespace demo::keyinputdemo {


using namespace erbsland::cterm;


/// Interactive demo that visualizes detected key presses in a horizontally scrolling field.
class KeyInputDemoApp final {
public:
    /// Run the demo until the user presses Escape.
    void run();

private:
    [[nodiscard]] auto canvasSize() const noexcept -> Size;
    [[nodiscard]] auto fieldRectForCanvas(Size canvasSize) const noexcept -> Rectangle;
    [[nodiscard]] auto visibleFieldSize() const noexcept -> Size;
    void initializeScrollBuffer() noexcept;
    void advanceScroll() noexcept;
    void handleKey(const Key &key) noexcept;
    void stampKeyBlock(const Key &key) noexcept;
    void renderFrame();
    void drawHeader(Rectangle rect);
    void drawField(Rectangle rect);
    void drawFooter(Rectangle rect);
    [[nodiscard]] auto footerText() const -> String;

private:
    [[nodiscard]] static auto backgroundChar() noexcept -> Char;
    [[nodiscard]] static auto guideColumnChar() noexcept -> Char;
    [[nodiscard]] static auto stampColors() noexcept -> const std::array<Color, 10> &;

private:
    constexpr static auto cScrollBufferSize = Size{250, 30};
    constexpr static auto cScrollDelay = std::chrono::milliseconds{200};

private:
    Terminal _terminal{Size{96, 20}};
    UpdateSettings _updateSettings;
    Buffer _buffer;
    RemappedBuffer _scrollBuffer{cScrollBufferSize, Orientation::Horizontal, backgroundChar()};
    std::mt19937 _random{std::random_device{}()};
    std::size_t _insertedColumnCount{0};
    bool _quitRequested{false};
};


}
