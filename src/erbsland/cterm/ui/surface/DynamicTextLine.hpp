// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "DynamicText.hpp"
#include "Panel.hpp"

#include "../ExclusiveSurfaceManager.hpp"
#include "../LayoutData.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <string_view>

namespace erbsland::cterm::ui::surface {

class DynamicTextLine;
using DynamicTextLinePtr = std::shared_ptr<DynamicTextLine>;

/// A composed one-line surface with left, middle, and right dynamic text sections.
class DynamicTextLine : public Panel, protected ExclusiveSurfaceManager {
public:
    /// The available logical sections of the line.
    enum class Section : uint8_t {
        Left,   ///< The section anchored to the left edge.
        Middle, ///< The section centered inside the remaining gap.
        Right,  ///< The section anchored to the right edge.
    };

    /// Layout priority for keeping, shrinking, or hiding one section.
    enum class SpacePriority : uint8_t {
        Keep,   ///< Keep the preferred width even if sections overlap.
        Shrink, ///< Shrink the assigned width to the available budget.
        Hide,   ///< Hide the section when it does not fit into the available budget.
    };

public:
    /// Create a dynamic text line with fixed height `1`.
    explicit DynamicTextLine(ProtectedTag) noexcept;
    ~DynamicTextLine() override = default;

public:
    /// Create a dynamic text line.
    /// @return The new dynamic text line.
    [[nodiscard]] static auto create() -> DynamicTextLinePtr;

public:
    /// Access the dynamic text surface for one section.
    /// @param section The section to query.
    /// @return The dynamic text surface.
    [[nodiscard]] auto dynamicText(Section section) const noexcept -> const DynamicTextPtr &;
    /// Get the text for one section.
    /// @param section The section to query.
    /// @return The text for the section.
    [[nodiscard]] auto text(Section section) const noexcept -> const String &;
    /// Replace the text for one section.
    /// @param section The section to update.
    /// @param text The new section text.
    void setText(Section section, String text);
    /// Replace the text for one section from UTF-8 text.
    /// @param section The section to update.
    /// @param text The new section text.
    void setText(Section section, std::string_view text);
    /// Clear the text for one section.
    /// @param section The section to clear.
    void clearText(Section section);
    /// Replace the margins for one section.
    /// @param section The section to update.
    /// @param margins The new section margins.
    void setMargins(Section section, Margins margins) noexcept;
    /// Get the margins for one section.
    /// @param section The section to query.
    /// @return The configured section margins.
    [[nodiscard]] auto margins(Section section) const noexcept -> Margins;
    /// Replace the space priority for one section.
    /// @param section The section to update.
    /// @param priority The new section space priority.
    void setSpacePriority(Section section, SpacePriority priority) noexcept;
    /// Get the space priority for one section.
    /// @param section The section to query.
    /// @return The configured section space priority.
    [[nodiscard]] auto spacePriority(Section section) const noexcept -> SpacePriority;
    /// Get the current width assigned to one section, excluding margins.
    /// @param section The section to query.
    /// @return The assigned width in terminal cells.
    [[nodiscard]] auto sectionWidth(Section section) const noexcept -> Coordinate;

public: // implement Surface
    void onLayout(LayoutScope &scope) noexcept override;

protected:
    /// Create and attach the owned child surfaces.
    void initializeUi() override;

protected: // implement ExclusiveSurfaceManager
    [[nodiscard]] auto isManagedChild(const SurfacePtr &surface) const noexcept -> bool override;

private:
    struct SectionLayout final {
        bool visible{false};         ///< Whether the section contributes to layout.
        Coordinate leadingExtra{};   ///< Additional parent-owned space before the child.
        Coordinate trailingExtra{};  ///< Additional parent-owned space after the child.
        Coordinate preferredWidth{}; ///< The preferred child width.
        Coordinate assignedWidth{};  ///< The assigned child width excluding margins.
        Coordinate reservedWidth{};  ///< The reserved width including margins.
    };

    struct SectionData final : public LayoutData {
        /// Create section data for one dynamic text child.
        explicit SectionData(Section section) noexcept : section{section} {}

        Section section{Section::Left};                     ///< The logical section for the child.
        Margins margins;                                    ///< Section margins.
        SpacePriority spacePriority{SpacePriority::Shrink}; ///< Section space priority.
        Coordinate assignedWidth{};                         ///< The last assigned width excluding margins.
    };

private:
    /// Access mutable section data.
    [[nodiscard]] auto sectionData(Section section) noexcept -> SectionData &;
    /// Access read-only section data.
    [[nodiscard]] auto sectionData(Section section) const noexcept -> const SectionData &;
    /// Measure one section and resolve its width.
    [[nodiscard]] auto layoutSection(Section section, Coordinate availableWidth, LayoutScope &scope) noexcept
        -> SectionLayout;
    /// Place one visible child surface.
    void placeSection(Section section, const SectionLayout &layout, Coordinate x, LayoutScope &scope) noexcept;
    /// Convert one section enum into a storage index.
    [[nodiscard]] static auto indexForSection(Section section) noexcept -> std::size_t;

private:
    std::array<DynamicTextPtr, 3> _dynamicTexts; ///< The owned dynamic text children.
};

}
