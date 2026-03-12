// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <erbsland/cterm/all.hpp>

#include <array>


namespace demo::bitmapshowcase {


using namespace erbsland::cterm;


/// Interactive demo showing the bitmap rendering modes and options.
class BitmapShowcaseApp final {
public:
    /// Run the demo until the user quits.
    void run();

private:
    [[nodiscard]] auto canvasSize() const noexcept -> Size;
    void handleKey(const Key &key) noexcept;
    void renderFrame();
    void drawSelector(Rectangle rect);
    void drawPreview(Rectangle rect);
    void drawScaleModeVariant(Rectangle rect, std::size_t variantIndex);
    void drawColorModeVariant(Rectangle rect, std::size_t variantIndex);
    void drawLayoutVariant(Rectangle rect, std::size_t variantIndex);
    void drawStyleVariant(Rectangle rect, std::size_t variantIndex);
    void drawPreviewPanel(Rectangle rect, std::string_view title, Color fillColor);
    void drawFooter(Rectangle rect);
    [[nodiscard]] auto footerText() const -> String;
    [[nodiscard]] auto pageTitle() const -> std::string_view;
    [[nodiscard]] auto variantCount(std::size_t pageIndex) const noexcept -> std::size_t;
    [[nodiscard]] auto variantTitle(std::size_t pageIndex, std::size_t variantIndex) const -> std::string_view;
    [[nodiscard]] auto selectedVariantIndex() const noexcept -> std::size_t;
    void selectVariantDelta(int delta) noexcept;

private:
    [[nodiscard]] static auto ringBitmap() -> const Bitmap &;
    [[nodiscard]] static auto rocketBitmap() -> const Bitmap &;
    [[nodiscard]] static auto waveBitmap() -> const Bitmap &;
    [[nodiscard]] static auto circuitBitmap() -> const Bitmap &;
    [[nodiscard]] static auto rainbowColors() -> const ColorSequence &;

private:
    Terminal _terminal{Size{92, 30}};
    UpdateSettings _updateSettings;
    Buffer _buffer;
    std::size_t _animationCycle{0};
    std::size_t _pageIndex{0};
    std::array<std::size_t, 4> _selectedVariantByPage{};
    bool _quitRequested{false};
};


}
