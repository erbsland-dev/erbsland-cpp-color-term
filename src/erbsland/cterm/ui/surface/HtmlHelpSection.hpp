// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "AbstractHelpSection.hpp"
#include "ScrollingBufferView.hpp"

#include "../../CursorBuffer.hpp"
#include "../../text/Style.hpp"

#include <memory>
#include <string>

namespace erbsland::cterm::ui::surface {

class HtmlHelpSection;
using HtmlHelpSectionPtr = std::shared_ptr<HtmlHelpSection>;

/// A help section that renders HTML content into a scrollable view.
class HtmlHelpSection : public AbstractHelpSection {
public:
    /// Create a HTML help section.
    explicit HtmlHelpSection(ProtectedTag) noexcept;
    ~HtmlHelpSection() override = default;

public:
    /// Create a HTML help section.
    /// @return The new HTML help section.
    [[nodiscard]] static auto create() -> HtmlHelpSectionPtr;

public:
    /// Access the section title.
    [[nodiscard]] auto title() const noexcept -> std::string_view override;
    /// Replace the section title.
    /// @param title The new title.
    void setTitle(std::string_view title);
    /// Access the HTML source.
    [[nodiscard]] auto html() const noexcept -> const std::string &;
    /// Replace the HTML source.
    /// @param html The new HTML content.
    void setHtml(std::string_view html);
    /// Access the rendered scroll view.
    [[nodiscard]] auto view() const noexcept -> const ScrollingBufferViewPtr &;
    /// Access the rendered content size.
    [[nodiscard]] auto contentSize() const noexcept -> Size;

public: // implement AbstractHelpSection
    void scrollUp(Coordinate count = 1) noexcept override;
    void scrollDown(Coordinate count = 1) noexcept override;
    void pageUp() noexcept override;
    void pageDown() noexcept override;
    void scrollToTop() noexcept override;
    void scrollToBottom() noexcept override;

public: // implement Surface
    void onLayout(LayoutScope &scope) noexcept override;

protected:
    /// Attach the owned child surfaces.
    void initializeUi() override;
    /// Render the HTML document if the source or available width changed.
    /// @param contentWidth The available content width.
    void renderDocumentIfRequired(Coordinate contentWidth);
    /// Create the cursor buffer.
    static auto createCursorBuffer() -> std::shared_ptr<CursorBuffer>;

private:
    std::string _title;                            ///< The section title.
    std::string _html;                             ///< The HTML source.
    std::shared_ptr<CursorBuffer> _documentBuffer; ///< The document buffer.
    ScrollingBufferViewPtr _view;                  ///< The scroll view.
    text::StyleConstPtr _style;                    ///< HTML style.
    Coordinate _renderedContentWidth{0};           ///< The last width used to render the document.
    bool _documentDirty{true};                     ///< Whether the document buffer must be rebuilt.
};

}
