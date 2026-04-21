// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "SectionOptions.hpp"

#include "../Layout.hpp"

namespace erbsland::cterm::ui::layout {

class Sections;
using SectionsPtr = std::shared_ptr<Sections>;

/// A vertical layout that separates child surfaces with themed title lines.
class Sections final : public Layout {
public:
    /// Section options type, kept here for source compatibility.
    using SectionOptions = layout::SectionOptions;

public:
    /// Create a sections layout.
    explicit Sections(ProtectedTag) noexcept;
    ~Sections() override = default;

public:
    /// Create a sections layout.
    /// @return The new sections layout.
    [[nodiscard]] static auto create() -> SectionsPtr;

public:
    /// Add one surface as a section.
    /// @param surface The section body surface.
    void addSection(SurfacePtr surface);
    /// Add one surface as a section.
    /// @param surface The section body surface.
    /// @param options The section metadata.
    void addSection(SurfacePtr surface, SectionOptions options);
    /// Get the number of configured sections.
    [[nodiscard]] auto sectionCount() const noexcept -> std::size_t;
    /// Access options for one section.
    /// @param index The section index.
    /// @return The section options.
    [[nodiscard]] auto sectionOptions(std::size_t index) const -> const SectionOptions &;
    /// Replace options for one section.
    /// @param index The section index.
    /// @param options The new section options.
    void setSectionOptions(std::size_t index, SectionOptions options);
    /// Test if a trailing separator is drawn after the last section.
    [[nodiscard]] auto isTrailingSeparatorVisible() const noexcept -> bool;
    /// Change whether a trailing separator is drawn after the last section.
    /// @param visible The new visibility state.
    void setTrailingSeparatorVisible(bool visible) noexcept;

public: // implement Surface
    void onLayout(LayoutScope &scope) noexcept override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

protected: // implement SurfaceManager
    [[nodiscard]] auto defaultLayoutData(const SurfacePtr &surface) const -> LayoutDataPtr override;

private:
    /// Count visible child sections.
    [[nodiscard]] auto visibleSectionCount() const noexcept -> std::size_t;
    /// Calculate the number of separator rows for the current visible sections.
    [[nodiscard]] auto separatorCount() const noexcept -> Coordinate;
    /// Draw one separator line.
    void drawSeparatorLine(
        WritableBuffer &buffer,
        const PaintContext &context,
        Coordinate y,
        const SectionOptions &options,
        bool focusWithin) const noexcept;
    /// Build the title block for one separator.
    [[nodiscard]] static auto titleBlock(const SectionOptions &options, const ThemeContext &themeContext) noexcept
        -> String;

private:
    bool _trailingSeparatorVisible{true}; ///< Whether to draw the final separator.
};

}
