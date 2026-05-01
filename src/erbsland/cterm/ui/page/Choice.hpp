// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../layout/Buttons.hpp"
#include "../layout/Centered.hpp"
#include "../layout/Frame.hpp"
#include "../layout/Stack.hpp"
#include "../Page.hpp"
#include "../surface/TextBox.hpp"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace erbsland::cterm::ui::page {

class Choice;
using ChoicePtr = std::shared_ptr<Choice>;

/// A non-opaque modal page that asks the user to choose one action.
class Choice final : public Page {
public:
    /// The selected choice passed to the callback.
    struct Selection final {
        std::string id;       ///< Stable application identifier for the choice.
        String title;         ///< Display title of the choice.
        std::size_t index{0}; ///< Insertion index of the choice.
    };

    /// Callback executed after the page has been popped.
    using Callback = std::function<void(const Selection &)>;

public:
    /// Create a choice page.
    Choice(String title, String description, ProtectedTag);
    ~Choice() override = default;

public:
    /// Create a choice page.
    /// @param title The prompt title.
    /// @param description The prompt description.
    /// @return The new choice page.
    [[nodiscard]] static auto create(String title, String description = {}) -> ChoicePtr;
    /// Create a choice page from UTF-8 text.
    /// @param title The prompt title.
    /// @param description The prompt description.
    /// @return The new choice page.
    [[nodiscard]] static auto create(std::string_view title, std::string_view description = {}) -> ChoicePtr;

public:
    /// Access the title.
    [[nodiscard]] auto title() const noexcept -> const String &;
    /// Replace the title.
    void setTitle(String title);
    /// Access the description.
    [[nodiscard]] auto description() const noexcept -> const String &;
    /// Replace the description.
    void setDescription(String description);
    /// Set the selection callback.
    void setCallback(Callback callback);
    /// Access the outer margins that keep the lower page visible.
    [[nodiscard]] auto margins() const noexcept -> Margins;
    /// Replace the outer margins.
    void setMargins(Margins margins) noexcept;
    /// Access the prompt content padding.
    [[nodiscard]] auto padding() const noexcept -> Margins;
    /// Replace the prompt content padding.
    void setPadding(Margins padding) noexcept;
    /// Access the preferred line width for the description text.
    [[nodiscard]] auto preferredLineWidth() const noexcept -> std::optional<Coordinate>;
    /// Replace the preferred line width for the description text.
    void setPreferredLineWidth(std::optional<Coordinate> width);
    /// Add one choice.
    /// @param id Stable application identifier.
    /// @param title Display title.
    /// @param keys Keyboard shortcuts for the choice.
    /// @param priority Display priority; larger values are shown first.
    /// @return The action backing the choice button.
    auto addChoice(std::string id, std::string title, Keys keys = {}, int priority = 0) -> ActionPtr;
    /// Add one choice using the title as stable identifier.
    /// @param title Display title and identifier.
    /// @param keys Keyboard shortcuts for the choice.
    /// @param priority Display priority; larger values are shown first.
    /// @return The action backing the choice button.
    auto addChoice(std::string title, Keys keys = {}, int priority = 0) -> ActionPtr;
    /// Show this page on the application display and focus the first enabled choice.
    void show();
    /// Pop this page from the application display.
    void dismiss();

public: // implement Surface
    [[nodiscard]] auto isOpaque() const noexcept -> bool override;
    void onLayout(LayoutScope &scope) noexcept override;
    void onPaint(WritableBuffer &buffer, const PaintContext &context) noexcept override;

private:
    struct ChoiceData final {
        std::string id;
        String title;
        int priority{0};
        std::size_t insertionIndex{0};
        ActionPtr action;
        surface::ButtonPtr button;
    };

private:
    /// Set up the UI
    void initializeUi() override;
    /// Select the choice with the given insertion index.
    void selectChoice(std::size_t insertionIndex);
    /// Focus the first enabled choice button.
    void focusDefaultChoice() noexcept;
    /// Get the display order index for a new choice.
    [[nodiscard]] auto insertionPositionFor(int priority, std::size_t insertionIndex) const noexcept -> std::size_t;
    /// Update child layout state from the current title, description and spacing settings.
    void updatePromptLayoutState();

private:
    static constexpr auto cMinimumDialogWidth = Coordinate{25};
    static constexpr auto cPreferredDialogWidth = Coordinate{60};
    static constexpr auto cMaximumDialogWidth = Coordinate{80};
    static constexpr auto cDefaultPadding = Margins{1, 2, 0, 2};

    String _title;                                     ///< Prompt title.
    String _description;                               ///< Prompt description.
    Callback _callback;                                ///< Selection callback.
    layout::CenteredPtr _centered;                     ///< Full-page centering layout.
    layout::FramePtr _frame;                           ///< The modal frame layout.
    layout::StackPtr _stack;                           ///< Vertical prompt body layout.
    surface::TextBoxPtr _descriptionText;              ///< Description text surface.
    layout::ButtonsPtr _buttons;                       ///< Choice button layout.
    std::vector<ChoiceData> _choices;                  ///< Choices in display order.
    std::size_t _nextInsertionIndex{0};                ///< Stable insertion index source.
    Margins _margins{4, 2};                            ///< Outer margins around the modal.
    Margins _padding{cDefaultPadding};                 ///< Prompt padding inside the frame.
    std::optional<Coordinate> _preferredLineWidth{78}; ///< Preferred readable description width.
};

}
