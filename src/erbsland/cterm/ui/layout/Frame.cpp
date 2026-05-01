// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Frame.hpp"

#include "../../theme/LayoutHelper.hpp"
#include "../../theme/ThemePainter.hpp"

#include <algorithm>
#include <utility>

namespace erbsland::cterm::ui::layout {

Frame::Frame(ProtectedTag) noexcept {
    editLayoutMetrics().setSizePolicy(SizePolicy::Preferred);
}

auto Frame::create() -> FramePtr {
    auto result = std::make_shared<Frame>(ProtectedTag{});
    result->initializeUi();
    return result;
}

void Frame::initializeUi() {
    SingleContentLayout::initializeUi();
    themeAttributes().setElement(theme::Element::Frame);
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
        auto contentInset = _padding;
        contentMetrics = measureContent(content, scope, proposal, contentInset);
        auto result = framedMetrics(contentMetrics, scope.themeContext(), contentInset);
        const auto decoration = decorationSize(contentInset);
        const auto resolvedContentWidth = result.preferred().width() - decoration.width();
        if (resolvedContentWidth < contentMetrics.preferred().width()) {
            const auto resolvedFrameWidth = result.preferred().width();
            contentMetrics = measureContent(
                content, scope, proposal.withWidth(LayoutDimension::atMost(resolvedFrameWidth)), contentInset);
            result = framedMetrics(contentMetrics, scope.themeContext(), contentInset);
        }
        return result;
    }
    return framedMetrics(contentMetrics, scope.themeContext(), _padding);
}

void Frame::onLayout(LayoutScope &scope) noexcept {
    const auto &content = contentSurface();
    if (content == nullptr || !content->flags().isVisible()) {
        return;
    }
    const auto initialMetrics = scope.measure(content, LayoutProposal::atMost(scope.size()));
    scope.place(content, contentRect(scope.size(), initialMetrics.margins()));
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

auto Frame::contentRect(const Size size, const Margins childMargins) const noexcept -> Rectangle {
    return Rectangle{Position{}, size}.insetBy(Margins{1}).insetBy(_padding.expandedWith(childMargins));
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

auto Frame::measureContent(
    const SurfacePtr &content, MeasureScope &scope, const LayoutProposal proposal, Margins &contentInset) const noexcept
    -> LayoutMetrics {
    auto contentMetrics = scope.measure(content, contentProposal(proposal, contentInset));
    contentInset = _padding.expandedWith(contentMetrics.margins());
    return scope.measure(content, contentProposal(proposal, contentInset));
}

auto Frame::framedMetrics(
    const LayoutMetrics &contentMetrics, const ThemeContext &themeContext, const Margins contentInset) const
    -> LayoutMetrics {
    const auto decoration = decorationSize(contentInset);
    const auto titleWidth = titleBlock(themeContext).displayWidth();
    auto minimum = contentMetrics.minimum() + decoration;
    auto preferred = contentMetrics.preferred() + decoration;
    minimum.setWidth(std::max(minimum.width(), titleWidth + Coordinate{2}));
    preferred.setWidth(std::max(preferred.width(), titleWidth + Coordinate{4}));

    auto result = layoutMetrics();
    result.setMinimum(minimum.limitedWith(result.maximum()));
    result.setPreferred(preferred.expandedWith(result.minimum()).limitedWith(result.maximum()));
    return result;
}

auto Frame::titleBlock(const ThemeContext &themeContext) const -> theme::StringWithMargins {
    const auto themeAccessor = themeContext.theme();
    return theme::layout::encloseInBrackets(_title, themeAccessor, theme::Part::TitleBracket, theme::Part::Title);
}

void Frame::drawTitle(WritableBuffer &buffer, const PaintContext &context, const Rectangle frameRect) const {
    if (_title.empty() || frameRect.width() <= 8) {
        return;
    }
    const auto title = titleBlock(context.themeContext());
    const auto rect = Rectangle{
        context.surfaceRect().topLeft() + Position{title.margins().left(), 0},
        Size{context.surfaceRect().width() - title.margins().horizontalExtent(), 1}};
    buffer.drawText(title.text(), rect);
}

}
