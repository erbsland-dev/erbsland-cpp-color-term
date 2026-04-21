// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <erbsland/cterm/CursorBuffer.hpp>
#include <erbsland/cterm/text/Style.hpp>
#include <erbsland/cterm/ui/Panel.hpp>
#include <erbsland/cterm/ui/ScrollingBufferView.hpp>

#include <memory>

namespace demo::ui_html_viewer {

using namespace erbsland::cterm;

class HtmlDocumentPanel final : public ui::Panel {
public:
    explicit HtmlDocumentPanel(ProtectedTag protectedTag) noexcept;
    ~HtmlDocumentPanel() override = default;

public:
    [[nodiscard]] static auto create() -> std::shared_ptr<HtmlDocumentPanel> {
        auto panel = std::make_shared<HtmlDocumentPanel>(ProtectedTag{});
        panel->initializeUi();
        return panel;
    }

public:
    void setHtml(std::string html);
    void setStyle(text::StyleConstPtr style) noexcept;
    [[nodiscard]] auto contentSize() const noexcept -> Size;
    [[nodiscard]] auto view() const noexcept -> const ui::ScrollingBufferViewPtr &;

public: // implement Surface
    void onLayout(ui::LayoutScope &scope) noexcept override;

private:
    void initializeUi();
    void renderDocumentIfRequired(Coordinate contentWidth);

private:
    std::shared_ptr<CursorBuffer> _documentBuffer = std::make_shared<CursorBuffer>(
        Size{1, 1}, CursorBuffer::OverflowMode::ExpandThenShift, Size{250, 10'000}, Char{U' ', fg::Default, bg::Black});
    ui::ScrollingBufferViewPtr _view = ui::ScrollingBufferView::create(_documentBuffer);
    std::string _html;
    text::StyleConstPtr _style = text::Style::defaultStyle(text::Style::Predefined::Styled);
    Coordinate _renderedContentWidth{0};
    bool _documentDirty{true};
};

}
