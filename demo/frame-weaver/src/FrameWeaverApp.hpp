// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <erbsland/cterm/all.hpp>

#include <chrono>
#include <random>
#include <vector>


namespace demo::frameweaver {


using namespace erbsland::cterm;


/// Continuously add random frames to demonstrate automatic line combinations.
class FrameWeaverApp final {
public:
    /// Run the demo until the user quits.
    void run();

private:
    enum class StyleMode : uint8_t {
        Light,
        Double,
        Heavy,
        Mixed,
        Block,
        Custom,
        All,
    };

    struct FrameSpec final {
        double x{};
        double y{};
        double width{};
        double height{};
        FrameStyle style = FrameStyle::Light;
        Char16StylePtr customStyle{};
        Color color{};
    };

private:
    [[nodiscard]] auto canvasSize() const noexcept -> Size;
    void handleKey(const Key &key) noexcept;
    void updateAnimation(std::chrono::milliseconds elapsed) noexcept;
    void addFrame() noexcept;
    void renderFrame();
    void renderFrames(Buffer &buffer, Rectangle contentRect) const;
    [[nodiscard]] auto createRandomFrame() -> FrameSpec;
    [[nodiscard]] static auto frameRectangle(FrameSpec frame, Rectangle contentRect) -> Rectangle;
    [[nodiscard]] static auto prismFrameStyle() -> const Char16StylePtr &;
    [[nodiscard]] static auto colors() -> const ColorSequence &;
    [[nodiscard]] auto availableStyles() const -> std::vector<FrameSpec>;
    [[nodiscard]] auto modeName() const -> std::string_view;
    [[nodiscard]] auto buildPrompt() const -> String;

private:
    Terminal _terminal{Size{78, 25}};
    std::mt19937 _rng{std::random_device{}()};
    std::vector<FrameSpec> _frames;
    std::chrono::milliseconds _frameDelay{1000};
    std::chrono::milliseconds _accumulator{};
    std::chrono::steady_clock::time_point _lastTick{std::chrono::steady_clock::now()};
    StyleMode _styleMode{StyleMode::Heavy};
    bool _quitRequested{false};
};


}
