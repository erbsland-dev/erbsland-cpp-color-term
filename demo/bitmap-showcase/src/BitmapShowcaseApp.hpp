// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <array>

#include <erbsland/cterm/all.hpp>


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
    void drawSelector(Buffer &buffer, Rectangle rect) const;
    void drawPreview(Buffer &buffer, Rectangle rect) const;
    void drawScaleModeVariant(Buffer &buffer, Rectangle rect, std::size_t variantIndex) const;
    void drawColorModeVariant(Buffer &buffer, Rectangle rect, std::size_t variantIndex) const;
    void drawLayoutVariant(Buffer &buffer, Rectangle rect, std::size_t variantIndex) const;
    void drawStyleVariant(Buffer &buffer, Rectangle rect, std::size_t variantIndex) const;
    void drawPreviewPanel(Buffer &buffer, Rectangle rect, std::string_view title, Color fillColor) const;
    void drawFooter(Buffer &buffer, Rectangle rect) const;
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
    std::size_t _animationCycle{0};
    std::size_t _pageIndex{0};
    std::array<std::size_t, 4> _selectedVariantByPage{};
    bool _quitRequested{false};
};


}
