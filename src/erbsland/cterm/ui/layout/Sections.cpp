// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Sections.hpp"

#include "impl/StackLayoutItems.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace erbsland::cterm::ui::layout {

Sections::Sections(ProtectedTag) noexcept {
    themeAttributes().setElement(theme::Element::Sections);
}

auto Sections::create() -> SectionsPtr {
    return std::make_shared<Sections>(ProtectedTag{});
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
    const auto themeContext = context.themeContext().withState(state);
    const auto themeAccessor = themeContext.theme();
    const auto rect = Rectangle{{surfaceRect.x1(), y}, Size{surfaceRect.width(), 1}};
    buffer.fill(rect, themeAccessor.forPart(theme::Part::Border).block(theme::BlockRole::Background));

    if (!options.title().empty()) {
        const auto leftInset = std::max(
            static_cast<Coordinate>(themeAccessor.forPart(theme::Part::Border).margins().left()), Coordinate{0});
        const auto x = rect.x1() + leftInset;
        if (x < rect.x2()) {
            buffer.drawText(titleBlock(options, themeContext), Rectangle{{x, y}, Size{rect.x2() - x, 1}});
        }
    }
    if (!options.rightText().empty()) {
        const auto rightInset = std::max(
            static_cast<Coordinate>(themeAccessor.forPart(theme::Part::Border).margins().right()), Coordinate{0});
        auto rightText = String{};
        rightText.appendWithBaseStyle(options.rightText(), themeAccessor.forPart(theme::Part::Text).style());
        buffer.drawText(
            rightText,
            Rectangle{{rect.x2() - rightInset - rightText.displayWidth(), y}, Size{rightText.displayWidth(), 1}});
    }
}

auto Sections::titleBlock(const SectionOptions &options, const ThemeContext &themeContext) noexcept -> String {
    const auto themeAccessor = themeContext.theme();
    const auto titleStyle = themeAccessor.forPart(theme::Part::Title).style();
    const auto titleMargins = themeAccessor.forPart(theme::Part::Title).margins();
    auto result = String{};
    result.append(themeAccessor.forPart(theme::Part::TitleBracket).block(theme::BlockRole::HorizontalWest));
    result.append(static_cast<std::size_t>(std::max(titleMargins.left(), 0)), U' ', titleStyle);
    result.appendWithBaseStyle(options.title(), titleStyle);
    result.append(static_cast<std::size_t>(std::max(titleMargins.right(), 0)), U' ', titleStyle);
    result.append(themeAccessor.forPart(theme::Part::TitleBracket).block(theme::BlockRole::HorizontalEast));
    return result;
}

}
