// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "Panel.hpp"

#include "../../geometry/Margins.hpp"
#include "../../String.hpp"

#include <array>
#include <cstdint>
#include <memory>

namespace erbsland::cterm::ui::surface {

class AbstractLine;
using AbstractLinePtr = std::shared_ptr<AbstractLine>;

/// A one-line panel with left, middle, and right text sections.
/// Subclasses provide section text, presence, margins, and collapse behavior while this base class owns layout.
class AbstractLine : public Panel {
public:
    /// The available logical sections of the line.
    enum class Section : uint8_t {
        Left,   ///< The section anchored to the left edge.
        Middle, ///< The section centered inside the remaining gap.
        Right,  ///< The section anchored to the right edge.
    };

    /// Overflow handling when one section does not fit into its allocated width.
    enum class CollapseBehavior : uint8_t {
        Never,    ///< Always render the full section, even if it overlaps other sections.
        Ellipsis, ///< Trim the section and add one ellipsis marker.
        Hide,     ///< Omit the section completely if it does not fit.
    };

public:
    /// Create a line surface with fixed height `1`.
    explicit AbstractLine(ProtectedTag) noexcept;
    ~AbstractLine() override = default;

public:
    /// Get the text for one section.
    /// Return a reference to storage owned by the subclass.
    /// @param section The section to query.
    /// @return The text for the section.
    [[nodiscard]] virtual auto text(Section section) const -> const String & = 0;
    /// Test whether one section currently contributes text to the layout.
    /// Sections without text reserve no width and their margins are ignored.
    /// The default implementation returns `!text(section).empty()`.
    /// @param section The section to query.
    /// @return `true` if the section currently contributes to the layout.
    [[nodiscard]] virtual auto hasText(Section section) const -> bool;
    /// Get the margins for one section.
    /// Only left and right margins affect layout because the line height is fixed to one row.
    /// The default implementation returns zero margins.
    /// @param section The section to query.
    /// @return The margins for the section.
    [[nodiscard]] virtual auto margins(Section section) const -> Margins;
    /// Get the collapse handling for one section.
    /// The default is `CollapseBehavior::Ellipsis`.
    /// @param section The section to query.
    /// @return The configured collapse behavior.
    [[nodiscard]] virtual auto collapseBehavior(Section section) const noexcept -> CollapseBehavior;
    /// Get the current pre-collapse width budget for one section.
    /// This width excludes the section's own margins and is updated during layout and paint passes.
    /// @param section The section to query.
    /// @return The current section width budget in terminal cells.
    [[nodiscard]] auto sectionWidth(Section section) const noexcept -> Coordinate;

public: // implement Surface
    void onLayout(LayoutScope &scope) noexcept override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

protected:
    /// Recalculate the cached section layout information for the given total width.
    /// @param totalWidth The available width of the line.
    void refreshSectionLayouts(Coordinate totalWidth) const noexcept;
    /// Get the cached reserved width for one section.
    /// This includes margins for non-overlapping sections and is zero for absent or `Never` sections.
    /// @param section The section to query.
    /// @return The reserved width in terminal cells.
    [[nodiscard]] auto reservedWidth(Section section) const noexcept -> Coordinate;
    /// Calculate the total horizontal margins for one section.
    /// Negative margins are clamped to zero for line layout.
    /// @param margins The source margins.
    /// @return The effective horizontal margin width.
    [[nodiscard]] static auto horizontalMargins(const Margins &margins) noexcept -> Coordinate;

private:
    enum class SectionAnchor : uint8_t {
        Left,
        Center,
        Right,
    };

    struct SectionLayout final {
        const String *text{};          ///< The source text.
        String visibleText;            ///< The visible text when a collapsed copy is required.
        Margins margins{};             ///< The effective margins for the section.
        Coordinate availableWidth{};   ///< The pre-collapse text width excluding margins.
        Coordinate reservedWidth{};    ///< The non-overlapping reserved width including margins.
        Coordinate textDisplayWidth{}; ///< The display width of the rendered text.
    };

private:
    /// Resolve one section for the given available width.
    /// @param section The section to resolve.
    /// @param availableWidth The available width before this section's own margins are removed.
    /// @return The resolved section layout.
    [[nodiscard]] auto layoutSection(Section section, Coordinate availableWidth) const -> SectionLayout;
    /// Create one ellipsized section string.
    /// @param text The original section text.
    /// @param availableWidth The available width.
    /// @param anchor The section anchor.
    /// @return The ellipsized section string.
    [[nodiscard]] static auto
    ellipsizedText(const StringView &text, Coordinate availableWidth, SectionAnchor anchor) noexcept -> String;
    /// Copy the first characters whose total display width fits into the limit.
    /// @param text The source text.
    /// @param maximumWidth The maximum display width.
    /// @return The visible prefix.
    [[nodiscard]] static auto prefixForWidth(const StringView &text, Coordinate maximumWidth) noexcept -> String;
    /// Copy the last characters whose total display width fits into the limit.
    /// @param text The source text.
    /// @param maximumWidth The maximum display width.
    /// @return The visible suffix.
    [[nodiscard]] static auto suffixForWidth(const StringView &text, Coordinate maximumWidth) noexcept -> String;
    /// Get the anchor used for one section.
    /// @param section The section to query.
    /// @return The section anchor.
    [[nodiscard]] static auto anchorForSection(Section section) noexcept -> SectionAnchor;
    /// Get the cached layout for one section.
    /// @param section The section to query.
    /// @return The cached layout for that section.
    [[nodiscard]] auto layoutFor(Section section) const noexcept -> const SectionLayout &;
    /// Convert one section enum into a storage index.
    /// @param section The section.
    /// @return The zero-based section index.
    [[nodiscard]] static auto indexForSection(Section section) noexcept -> std::size_t;

private:
    mutable std::array<SectionLayout, 3> _sectionLayouts; ///< Cached layout data for the current width.
};

}
