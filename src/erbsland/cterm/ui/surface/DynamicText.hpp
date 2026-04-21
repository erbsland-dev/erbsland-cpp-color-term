// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../Surface.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>

namespace erbsland::cterm::ui::surface {

class DynamicText;
using DynamicTextPtr = std::shared_ptr<DynamicText>;

/// A one-line text surface with optional dynamic update callback.
class DynamicText final : public Surface {
public:
    /// Automatic update handling.
    enum class UpdateMode : uint8_t {
        Static,    ///< Never call the update function; text changes happen only through `setText()`.
        OnResize,  ///< Call the update function when the available width changes.
        OnRefresh, ///< Call the update function before every paint pass.
    };

    /// Callback type used to update the text.
    using UpdateFn = std::function<void(String &text, Coordinate availableWidth)>;

public:
    /// Create dynamic text with an initial text and alignment.
    /// @param text The initial text.
    /// @param alignment The alignment used when painting.
    DynamicText(String text, Alignment alignment, ProtectedTag);
    ~DynamicText() override = default;

public:
    /// Create dynamic text with an initial terminal string.
    /// @param text The initial text.
    /// @param alignment The alignment used when painting.
    /// @return The new dynamic text.
    [[nodiscard]] static auto create(String text = {}, Alignment alignment = Alignment::TopLeft) -> DynamicTextPtr;
    /// Create dynamic text with an initial UTF-8 text.
    /// @param text The initial text.
    /// @param alignment The alignment used when painting.
    /// @return The new dynamic text.
    [[nodiscard]] static auto create(std::string_view text, Alignment alignment = Alignment::TopLeft) -> DynamicTextPtr;

public:
    /// Get the current text.
    [[nodiscard]] auto text() const noexcept -> const String & { return _text; }
    /// Replace the current text.
    /// @param text The new text.
    void setText(String text);
    /// Replace the current text from UTF-8 text.
    /// @param text The new text.
    void setText(std::string_view text);
    /// Get the current update mode.
    [[nodiscard]] auto updateMode() const noexcept -> UpdateMode { return _updateMode; }
    /// Replace the update mode.
    /// @param updateMode The new update mode.
    void setUpdateMode(UpdateMode updateMode) noexcept;
    /// Replace the update function.
    /// @param updateFn The new update function.
    void setUpdateFn(UpdateFn updateFn);
    /// Remove the update function.
    void clearUpdateFn() noexcept;
    /// Get the current text display width.
    [[nodiscard]] auto textDisplayWidth() const noexcept -> Coordinate { return _text.displayWidth(); }
    /// Update the text for a known width when the update mode requires it.
    /// @param availableWidth The available width.
    void updateForWidth(Coordinate availableWidth) noexcept;

public: // implement Surface
    [[nodiscard]] auto onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept
        -> LayoutMetrics override;
    void onLayout(LayoutScope &scope) noexcept override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

private:
    void updatePreferredSize();

private:
    String _text;                               ///< The text to display.
    TextOptions _textOptions;                   ///< Paint options.
    UpdateMode _updateMode{UpdateMode::Static}; ///< The update mode.
    UpdateFn _updateFn;                         ///< Optional update function.
    std::optional<Coordinate> _lastWidth;       ///< Last width used for OnResize.
};

}
