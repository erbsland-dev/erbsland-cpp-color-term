// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "UiWorldViewApp.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <stdexcept>

namespace demo::ui_world_view {

void UiWorldViewApp::setupUi() {
    auto page = ui::Page::create();
    addDefaultActions(page);

    auto panAction = ui::Action::create("Pan");
    panAction->help().setDescription("Pan the map view using the arrow keys.");
    panAction->setKeys({Key::Left, Key::Up, Key::Down, Key::Right});
    panAction->setTriggerFn([this](const ui::ActionTriggerContext &context) -> void {
        switch (context.key().type()) {
        case Key::Left:
            panBy(Position{-6, 0});
            break;
        case Key::Right:
            panBy(Position{6, 0});
            break;
        case Key::Up:
            panBy(Position{0, -3});
            break;
        case Key::Down:
            panBy(Position{0, 3});
            break;
        default:
            break;
        }
    });
    panAction->help().setPriority(100);
    page->actions().add(panAction);

    auto zoomAction = ui::Action::create("Zoom");
    zoomAction->help().setDescription("Zoom in and out of the map.");
    zoomAction->setKeys(Keys{U'+', U'-', U']', U'[', U'='}.setMainKeyCount(2));
    zoomAction->setTriggerFn([this](const ui::ActionTriggerContext &context) -> void {
        switch (context.key().unicode()) {
        case U'+':
        case U'=':
        case U']':
            zoomIn();
            break;
        case U'-':
        case U'[':
            zoomOut();
            break;
        default:
            break;
        }
    });
    zoomAction->help().setPriority(90);
    page->actions().add(zoomAction);

    auto labelsAction = ui::Action::create("Labels");
    labelsAction->help().setDescription("Toggle display of map labels.");
    labelsAction->setKeys(Keys{U'l'});
    labelsAction->setTriggerFn([this]() -> void { toggleLabels(); });
    labelsAction->help().setPriority(70);
    page->actions().add(labelsAction);

    auto gridAction = ui::Action::create("Grid");
    gridAction->help().setDescription("Toggle display of map grid.");
    gridAction->setKeys(Keys{U'c'});
    gridAction->setTriggerFn([this]() -> void { toggleGrid(); });
    gridAction->help().setPriority(60);
    page->actions().add(gridAction);

    auto root = ui::Stack::create(Orientation::Vertical);
    page->addSurface(root);

    using Section = ui::DynamicTextLine::Section;
    using SpacePriority = ui::DynamicTextLine::SpacePriority;

    auto header = ui::HeaderLine::create();
    header->setText(Section::Left, String{"UI World View", fg::BrightWhite});
    header->setMargins(Section::Left, Margins{1, 0});
    header->setSpacePriority(Section::Middle, SpacePriority::Hide);
    header->dynamicText(Section::Middle)->setUpdateFn([this](String &text, const Coordinate) -> void {
        const auto zoom = _mapView != nullptr ? _mapView->zoomIndex() + 1 : std::size_t{0};
        const auto zoomCount = _mapView != nullptr ? _mapView->zoomLevelCount() : std::size_t{0};
        const auto labelsShown = _mapView != nullptr && _mapView->labelsShown();
        const auto gridShown = _mapView != nullptr && _mapView->gridShown();
        text = String{
            std::format(
                "zoom {}/{}  labels {}  grid {}",
                zoom,
                zoomCount,
                labelsShown ? "on" : "off",
                gridShown ? "on" : "off"),
            fg::BrightYellow};
    });
    header->dynamicText(Section::Middle)->updateText();
    header->setMargins(Section::Middle, Margins{1, 0});
    header->dynamicText(Section::Right)->setUpdateFn([this](String &text, const Coordinate) -> void {
        const auto mapSize = _mapView != nullptr ? _mapView->mapSize() : Size{};
        text = String{std::format("{} x {}", mapSize.width(), mapSize.height()), fg::BrightCyan};
    });
    header->dynamicText(Section::Right)->updateText();
    header->setMargins(Section::Right, Margins{1, 0});
    root->addSurface(header);
    _headerStatus = header;

    _mapHost = ui::Stack::create(Orientation::Vertical);
    root->addSurface(_mapHost);

    auto footer = ui::FooterLine::create();
    footer->leftText()->setUpdateFn([this](String &text, const Coordinate) -> void {
        const auto visibleRect = _mapView != nullptr && _mapArea != nullptr
            ? _mapView->visibleSourceRect(_mapArea->visibleContentRect())
            : Rectangle{};
        text = String{
            std::format(
                "x: {}–{}  y: {}–{}",
                visibleRect.x1(),
                std::max(visibleRect.x2() - 1, visibleRect.x1()),
                visibleRect.y1(),
                std::max(visibleRect.y2() - 1, visibleRect.y1())),
            fg::BrightGreen};
    });
    root->addSurface(footer);
    _footerStatus = footer;

    setMainPage(page);
}

auto UiWorldViewApp::processCommandLineArguments(const CommandLineArgs &args) -> ExitCode {
    try {
        auto dataFilePath = std::filesystem::path{};
        for (auto index = std::size_t{1}; index < args.size(); ++index) {
            const auto &argument = args[index];
            if (argument == "--help" || argument == "-h") {
                printUsage();
                return 0;
            }
            if (argument.starts_with('-')) {
                throw std::runtime_error{std::format("Unknown option: {}", argument)};
            }
            if (!dataFilePath.empty()) {
                throw std::runtime_error{"Only one world map data file argument is supported."};
            }
            dataFilePath = std::filesystem::path{argument};
        }

        _dataFilePath = dataFilePath.empty() ? defaultDataFilePath() : dataFilePath;
        _mapData = std::make_shared<WorldMapData>(WorldMapData::loadFromFile(_dataFilePath));
        _mapView = WorldMapView::create(_mapData);
        _mapArea = ui::ScrollArea::create();
        _mapArea->setContentSurface(_mapView);
        _mapHost->addSurface(_mapArea);
        updateDynamicUi();
    } catch (const std::exception &exception) {
        terminal().printLine(fg::BrightRed, exception.what());
        printUsage();
        return 1;
    }
    return cExitCodeContinue;
}

void UiWorldViewApp::updateDynamicUi() noexcept {
    if (_mapArea == nullptr || _mapView == nullptr) {
        return;
    }
    _mapArea->flags().setLayoutOutdated();
    _mapArea->flags().setPaintOutdated();
    _mapView->flags().setPaintOutdated();
    if (_headerStatus != nullptr) {
        using Section = ui::DynamicTextLine::Section;
        _headerStatus->dynamicText(Section::Middle)->updateText();
        _headerStatus->dynamicText(Section::Right)->updateText();
        _headerStatus->flags().setLayoutOutdated();
        _headerStatus->flags().setPaintOutdated();
    }
    if (_footerStatus != nullptr) {
        _footerStatus->leftText()->updateText();
        _footerStatus->flags().setPaintOutdated();
    }
}

void UiWorldViewApp::panBy(const Position delta) noexcept {
    if (_mapArea != nullptr) {
        _mapArea->scrollBy(delta);
        updateDynamicUi();
    }
}

void UiWorldViewApp::zoomIn() noexcept {
    changeZoom(true);
}

void UiWorldViewApp::zoomOut() noexcept {
    changeZoom(false);
}

void UiWorldViewApp::changeZoom(const bool zoomIn) noexcept {
    if (_mapArea == nullptr || _mapView == nullptr) {
        return;
    }

    const auto oldOffset = _mapArea->scrollOffset();
    const auto oldViewportSize = _mapArea->viewportRect().size();
    const auto oldContentSize = _mapView->contentSize();
    const auto changed = zoomIn ? _mapView->zoomIn() : _mapView->zoomOut();
    if (!changed) {
        return;
    }

    const auto newContentSize = _mapView->contentSize();
    const auto scaledOffset = [](const Coordinate offset,
                                 const Coordinate viewportExtent,
                                 const Coordinate oldExtent,
                                 const Coordinate newExtent) noexcept -> Coordinate {
        if (oldExtent <= 0 || newExtent <= 0) {
            return 0;
        }
        const auto center = static_cast<double>(offset) + static_cast<double>(std::max(viewportExtent, 1)) / 2.0;
        const auto scale = static_cast<double>(newExtent) / static_cast<double>(oldExtent);
        return static_cast<Coordinate>(std::lround(center * scale - static_cast<double>(viewportExtent) / 2.0));
    };
    _mapArea->setScrollOffset(
        Position{
            scaledOffset(oldOffset.x(), oldViewportSize.width(), oldContentSize.width(), newContentSize.width()),
            scaledOffset(oldOffset.y(), oldViewportSize.height(), oldContentSize.height(), newContentSize.height())});
    updateDynamicUi();
}

void UiWorldViewApp::toggleLabels() noexcept {
    if (_mapView != nullptr) {
        _mapView->setLabelsShown(!_mapView->labelsShown());
        updateDynamicUi();
    }
}

void UiWorldViewApp::toggleGrid() noexcept {
    if (_mapView != nullptr) {
        _mapView->setGridShown(!_mapView->gridShown());
        updateDynamicUi();
    }
}

void UiWorldViewApp::printUsage() const {
    terminal().printLine(fg::BrightGreen, "Usage:");
    terminal().printLine(fg::BrightWhite, "ui-world-view [world-map.txt]");
}

auto UiWorldViewApp::defaultDataFilePath() -> std::filesystem::path {
    return std::filesystem::path{ERBSLAND_COLOR_TERM_UI_WORLD_VIEW_DEFAULT_DATA_FILE};
}

}
