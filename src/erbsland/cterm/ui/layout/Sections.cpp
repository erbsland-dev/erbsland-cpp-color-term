// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Sections.hpp"

#include "impl/StackLayoutItems.hpp"

#include "../../theme/ThemePainter.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace erbsland::cterm::ui::layout {

Sections::Sections(ProtectedTag) noexcept {
}

auto Sections::create() -> SectionsPtr {
    auto result = std::make_shared<Sections>(ProtectedTag{});
    result->initializeUi();
    return result;
}

void Sections::initializeUi() {
    Layout::initializeUi();
    themeAttributes().setElement(theme::Element::Sections);
}

void Sections::addSection(SurfacePtr surface) {
    addSection(std::move(surface), SectionOptions{});
}

void Sections::addSection(SurfacePtr surface, SectionOptions options) {
    childStorage().add(std::move(surface), std::make_shared<SectionOptions>(std::move(options)));
}

auto Sections::sectionCount() const noexcept -> std::size_t {
    return childStorage().size();
}

auto Sections::sectionOptions(const std::size_t index) const -> const SectionOptions & {
    const auto data = std::dynamic_pointer_cast<SectionOptions>(childStorage().layoutData(index));
    if (data == nullptr) {
        throw std::logic_error{"Section options are missing."};
    }
    return *data;
}

void Sections::setSectionOptions(const std::size_t index, SectionOptions options) {
    if (index >= childStorage().size()) {
        throw std::out_of_range{"Section index is out of range."};
    }
    childStorage().setLayoutData(index, std::make_shared<SectionOptions>(std::move(options)));
    flags().setPaintOutdated();
}

auto Sections::isTrailingSeparatorVisible() const noexcept -> bool {
    return _trailingSeparatorVisible;
}

void Sections::setTrailingSeparatorVisible(const bool visible) noexcept {
    if (_trailingSeparatorVisible == visible) {
        return;
    }
    _trailingSeparatorVisible = visible;
    flags().setLayoutOutdated();
}

void Sections::onLayout(LayoutScope &scope) noexcept {
    const auto newSize = scope.size();
    const auto separators = separatorCount();
    auto contentSize = newSize;
    contentSize.setHeight(std::max(newSize.height() - separators, 0));
    auto items = impl::StackLayoutItems::fromSurfaces(childStorage(), Orientation::Vertical, contentSize, scope);
    items.resolveMainSizes(contentSize.height());

    auto y = Coordinate{0};
    for (const auto &item : items.items()) {
        y += 1;
        const auto childSize = Size{item.assignedCrossSize(), item.assignedMainSize()};
        scope.place(item.surface(), Rectangle{{0, y}, childSize});
        y += item.assignedMainSize();
    }
}

void Sections::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    for (auto index = std::size_t{0}; index < childStorage().size(); ++index) {
        const auto &child = childStorage()[index];
        if (child == nullptr || !child->flags().isVisible()) {
            continue;
        }
        drawSeparatorLine(
            buffer, context, child->rectangle().y1() - 1, sectionOptions(index), child->flags().hasFocusWithin());
    }
    if (_trailingSeparatorVisible && visibleSectionCount() > 0) {
        auto y = Coordinate{0};
        for (const auto &child : childStorage()) {
            if (child != nullptr && child->flags().isVisible()) {
                y = std::max(y, child->rectangle().y2());
            }
        }
        drawSeparatorLine(buffer, context, y, SectionOptions{}, false);
    }
}

auto Sections::visibleSectionCount() const noexcept -> std::size_t {
    auto result = std::size_t{0};
    for (const auto &child : childStorage()) {
        if (child != nullptr && child->flags().isVisible()) {
            result += 1;
        }
    }
    return result;
}

auto Sections::defaultLayoutData([[maybe_unused]] const SurfacePtr &surface) const -> LayoutDataPtr {
    return std::make_shared<SectionOptions>();
}

auto Sections::separatorCount() const noexcept -> Coordinate {
    const auto visibleCount = static_cast<Coordinate>(visibleSectionCount());
    if (visibleCount == 0) {
        return 0;
    }
    return visibleCount + (_trailingSeparatorVisible ? 1 : 0);
}

void Sections::drawSeparatorLine(
    WritableBuffer &buffer,
    const PaintContext &context,
    const Coordinate y,
    const SectionOptions &options,
    const bool focusWithin) const noexcept {
    const auto surfaceRect = context.surfaceRect();
    if (y < surfaceRect.y1() || y >= surfaceRect.y2()) {
        return;
    }
    auto state = context.themeContext().state();
    if (focusWithin) {
        state |= theme::States{theme::State::FocusWithin};
    }
    const auto themeAccessor = context.theme(); //.withState(state).theme();
    const auto lineRect = Rectangle{{surfaceRect.x1(), y}, Size{surfaceRect.width(), 1}};
    theme::ThemePainter{buffer, themeAccessor.forPart(theme::Part::Border)}.drawFrame(lineRect);

    if (!options.title().empty()) {
        const auto title = theme::layout::encloseInBrackets(
            options.title(), themeAccessor, theme::Part::TitleBracket, theme::Part::Title);
        const auto rect = lineRect.insetBy(title.margins());
        buffer.drawText(title.text(), rect);
    }
    if (!options.rightText().empty()) {
        const auto textRight =
            theme::layout::stylePaddingAndMargins(options.rightText(), themeAccessor, theme::Part::Text);
        const auto rect = lineRect.insetBy(textRight.margins());
        buffer.drawText(textRight.text(), rect, Alignment::Right);
    }
}

}
