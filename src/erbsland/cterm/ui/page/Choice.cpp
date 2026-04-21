// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "Choice.hpp"

#include "../Application.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace erbsland::cterm::ui::page {

Choice::Choice(String title, String description, ProtectedTag protectedTag) :
    Page{protectedTag},
    _title{std::move(title)},
    _description{std::move(description)},
    _centered{layout::Centered::create()},
    _frame{layout::Frame::create()},
    _stack{layout::Stack::create(Orientation::Vertical)},
    _descriptionText{surface::TextBox::create(String{})},
    _buttons{layout::Buttons::create()} {
    themeAttributes().setElement(theme::Element::Choice);
}

auto Choice::create(String title, String description) -> ChoicePtr {
    auto result = std::make_shared<Choice>(std::move(title), std::move(description), ProtectedTag{});
    result->_descriptionText->themeAttributes().setElement(theme::Element::Choice);
    result->_frame->themeAttributes().setElement(theme::Element::Choice);
    auto frameMetrics = result->_frame->editLayoutMetrics();
    frameMetrics.setMinimumWidth(cMinimumDialogWidth)
        .setPreferredWidth(cPreferredDialogWidth)
        .setMaximumWidth(cMaximumDialogWidth)
        .setSizePolicyForWidth(DimensionPolicy::Preferred);
    result->_frame->setPadding(cDefaultPadding);
    result->_buttons->editLayoutMetrics().setSizePolicyForHeight(DimensionPolicy::Preferred);
    result->_stack->addSurface(result->_descriptionText);
    result->_stack->addSurface(result->_buttons);
    result->_frame->setContentSurface(result->_stack);
    result->_centered->setContentSurface(result->_frame);
    result->addSurface(result->_centered);
    result->updatePromptLayoutState();
    return result;
}

auto Choice::create(const std::string_view title, const std::string_view description) -> ChoicePtr {
    return create(String{title, EncodingErrors::Replace}, String{description, EncodingErrors::Replace});
}

auto Choice::title() const noexcept -> const String & {
    return _title;
}

void Choice::setTitle(String title) {
    _title = std::move(title);
    updatePromptLayoutState();
}

auto Choice::description() const noexcept -> const String & {
    return _description;
}

void Choice::setDescription(String description) {
    _description = std::move(description);
    updatePromptLayoutState();
}

void Choice::setCallback(Callback callback) {
    _callback = std::move(callback);
}

auto Choice::margins() const noexcept -> Margins {
    return _margins;
}

void Choice::setMargins(const Margins margins) noexcept {
    if (_margins == margins) {
        return;
    }
    _margins = margins;
    updatePromptLayoutState();
}

auto Choice::padding() const noexcept -> Margins {
    return _padding;
}

void Choice::setPadding(const Margins padding) noexcept {
    if (_padding == padding) {
        return;
    }
    _padding = padding;
    updatePromptLayoutState();
}

auto Choice::preferredLineWidth() const noexcept -> std::optional<Coordinate> {
    return _preferredLineWidth;
}

void Choice::setPreferredLineWidth(std::optional<Coordinate> width) {
    if (width.has_value()) {
        width = std::max(*width, Coordinate{1});
    }
    if (_preferredLineWidth == width) {
        return;
    }
    _preferredLineWidth = width;
    updatePromptLayoutState();
}

auto Choice::addChoice(std::string id, std::string title, Keys keys, const int priority) -> ActionPtr {
    if (title.empty()) {
        throw std::invalid_argument{"Choice title must not be empty."};
    }
    auto action = ButtonAction::create(title);
    action->setKeys(std::move(keys));
    action->help().setPriority(priority);
    const auto insertionIndex = _nextInsertionIndex++;
    const auto weakChoice = SurfaceWeakPtr{weak_from_this()};
    action->setTriggerFn([weakChoice, insertionIndex]([[maybe_unused]] const ActionTriggerContext &context) -> void {
        if (const auto surface = weakChoice.lock(); surface != nullptr) {
            if (const auto choice = std::dynamic_pointer_cast<Choice>(surface); choice != nullptr) {
                choice->selectChoice(insertionIndex);
            }
        }
    });
    auto button = surface::Button::create(action);
    const auto insertIndex = insertionPositionFor(priority, insertionIndex);
    _choices.insert(
        _choices.begin() + static_cast<std::ptrdiff_t>(insertIndex),
        ChoiceData{std::move(id), String{title}, priority, insertionIndex, action, button});
    _buttons->insertButton(insertIndex, std::move(button));
    flags().setLayoutOutdated();
    flags().setPaintOutdated();
    return action;
}

auto Choice::addChoice(std::string title, Keys keys, const int priority) -> ActionPtr {
    auto id = title;
    return addChoice(std::move(id), std::move(title), std::move(keys), priority);
}

void Choice::show() {
    getApplication().display().pushPage(std::dynamic_pointer_cast<Page>(shared_from_this()));
    focusDefaultChoice();
}

void Choice::dismiss() {
    getApplication().display().popPage();
}

auto Choice::isOpaque() const noexcept -> bool {
    return false;
}

void Choice::onLayout(LayoutScope &scope) noexcept {
    Page::onLayout(scope);
    if (!hasFocusSurface()) {
        focusDefaultChoice();
    }
}

void Choice::onPaint([[maybe_unused]] WritableBuffer &buffer, [[maybe_unused]] const PaintContext &context) noexcept {
    const auto style = context.theme().forPart(theme::Part::Background).style();
    buffer.rect().forEach([&](const auto &pos) -> void { buffer.set(pos, buffer.get(pos).withStyleReplaced(style)); });
}

void Choice::selectChoice(const std::size_t insertionIndex) {
    const auto it = std::ranges::find_if(
        _choices, [&](const ChoiceData &choice) -> bool { return choice.insertionIndex == insertionIndex; });
    if (it == _choices.end()) {
        return;
    }
    const auto selection = Selection{it->id, it->title, it->insertionIndex};
    dismiss();
    if (_callback) {
        _callback(selection);
    }
}

void Choice::focusDefaultChoice() noexcept {
    for (const auto &choice : _choices) {
        if (choice.button != nullptr && choice.button->isEnabled()) {
            static_cast<void>(choice.button->requestFocus());
            return;
        }
    }
}

auto Choice::insertionPositionFor(const int priority, const std::size_t insertionIndex) const noexcept -> std::size_t {
    const auto it = std::ranges::find_if(_choices, [&](const ChoiceData &choice) -> bool {
        if (choice.priority != priority) {
            return choice.priority < priority;
        }
        return choice.insertionIndex > insertionIndex;
    });
    return static_cast<std::size_t>(std::ranges::distance(_choices.begin(), it));
}

void Choice::updatePromptLayoutState() {
    _centered->setPadding(_margins);
    _frame->setTitle(_title);
    _frame->setPadding(_padding);
    _descriptionText->setText(_description);
    _descriptionText->setPreferredLineWidth(_preferredLineWidth);
    _descriptionText->flags().setVisible(!_description.empty());
    flags().setLayoutOutdated();
    flags().setPaintOutdated();
}

}
