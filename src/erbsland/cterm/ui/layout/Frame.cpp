// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Frame.hpp"

#include "../../theme/ThemePainter.hpp"

#include <algorithm>
#include <utility>

namespace erbsland::cterm::ui::layout {

Frame::Frame(ProtectedTag) noexcept {
    themeAttributes().setElement(theme::Element::Frame);
    editLayoutMetrics().setSizePolicy(SizePolicy::Preferred);
}

auto Frame::create() -> FramePtr {
    return std::make_shared<Frame>(ProtectedTag{});
}

auto Frame::title() const noexcept -> const String & {
    return _title;
}

void Frame::setTitle(String title) {
    _title = std::move(title);
    flags().setLayoutOutdated();
    flags().setPaintOutdated();
}

auto Frame::padding() const noexcept -> Margins {
    return _padding;
}

void Frame::setPadding(const Margins padding) noexcept {
    if (_padding == padding) {
        return;
    }
    _padding = padding;
    flags().setLayoutOutdated();
    flags().setPaintOutdated();
}

auto Frame::onMeasure(MeasureScope &scope, const LayoutProposal &proposal) noexcept -> LayoutMetrics {
    auto contentMetrics = LayoutMetrics{};
    contentMetrics.setFixedSize({});
    const auto &content = contentSurface();
    if (content != nullptr && content->flags().isVisible()) {
        contentMetrics = scope.measure(content, contentProposal(proposal, _padding));
    }
    return framedMetrics(contentMetrics, scope.themeContext());
}

void Frame::onLayout(LayoutScope &scope) noexcept {
    const auto &content = contentSurface();
    if (content == nullptr || !content->flags().isVisible()) {
        return;
    }
    scope.place(content, contentRect(scope.size()));
}

auto Frame::isOpaque() const noexcept -> bool {
    return true;
}

void Frame::onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept {
    const auto frameRect = context.surfaceRect();
    if (frameRect.size().isZero()) {
        return;
    }
    theme::ThemePainter{buffer, context.theme().forPart(theme::Part::Background)}.fill(frameRect);
    theme::ThemePainter{buffer, context.theme().forPart(theme::Part::Border)}.drawFrame(frameRect);
    drawTitle(buffer, context, frameRect);
}

auto Frame::decorationSize(const Margins padding) noexcept -> Size {
    return Size{2, 2} + padding.extent();
}

auto Frame::contentRect(const Size size) const noexcept -> Rectangle {
    return Rectangle{Position{}, size}.insetBy(Margins{1}).insetBy(_padding);
}

auto Frame::contentProposal(const LayoutProposal proposal, const Margins padding) noexcept -> LayoutProposal {
    const auto decoration = decorationSize(padding);
    auto result = proposal;
    if (proposal.width().hasBound()) {
        result =
            result.withWidth(LayoutDimension{proposal.width().type(), proposal.width().value() - decoration.width()});
    }
    if (proposal.height().hasBound()) {
        result = result.withHeight(
            LayoutDimension{proposal.height().type(), proposal.height().value() - decoration.height()});
    }
    return result;
}

auto Frame::framedMetrics(const LayoutMetrics &contentMetrics, const ThemeContext &themeContext) const
    -> LayoutMetrics {
    const auto decoration = decorationSize(_padding);
    const auto titleWidth = _title.empty() ? Coordinate{0} : titleBlock(themeContext).displayWidth();
    auto minimum = contentMetrics.minimum() + decoration;
    auto preferred = contentMetrics.preferred() + decoration;
    minimum.setWidth(std::max(minimum.width(), titleWidth + Coordinate{2}));
    preferred.setWidth(std::max(preferred.width(), titleWidth + Coordinate{4}));

    auto result = layoutMetrics();
    result.setMinimum(minimum.componentMin(result.maximum()));
    result.setPreferred(preferred.componentMax(result.minimum()).componentMin(result.maximum()));
    return result;
}

auto Frame::titleBlock(const ThemeContext &themeContext) const -> String {
    const auto themeAccessor = themeContext.theme();
    const auto textStyle = themeAccessor.forPart(theme::Part::Text).style();
    const auto indicatorTheme = themeAccessor.forPart(theme::Part::Indicator);
    const auto indicatorMargins = indicatorTheme.margins();
    auto result = String{};
    result.append(indicatorTheme.block(theme::BlockRole::HorizontalWest));
    result.append(static_cast<std::size_t>(std::max(indicatorMargins.left(), 0)), U' ', textStyle);
    result.appendWithBaseStyle(_title, textStyle);
    result.append(static_cast<std::size_t>(std::max(indicatorMargins.right(), 0)), U' ', textStyle);
    result.append(indicatorTheme.block(theme::BlockRole::HorizontalEast));
    return result;
}

void Frame::drawTitle(WritableBuffer &buffer, const PaintContext &context, const Rectangle frameRect) const {
    if (_title.empty() || frameRect.width() <= 4) {
        return;
    }
    const auto title = titleBlock(context.themeContext());
    const auto leftInset =
        std::max(static_cast<Coordinate>(context.theme().forPart(theme::Part::Border).margins().left()), Coordinate{0});
    const auto x = frameRect.x1() + leftInset;
    if (x >= frameRect.x2()) {
        return;
    }
    buffer.drawText(title, Rectangle{{x, frameRect.y1()}, Size{frameRect.x2() - x, 1}});
}

}
