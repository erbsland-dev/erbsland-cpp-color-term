// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "StaticText.hpp"

#include <memory>
#include <string_view>
#include <utility>

namespace erbsland::cterm::ui::surface {

class Label;
using LabelPtr = std::shared_ptr<Label>;

/// A one-line label surface.
class Label : public StaticText {
public:
    /// Create a label with an initial text and alignment.
    /// @param text The initial text.
    /// @param alignment The alignment used when painting.
    Label(String text, Alignment alignment, ProtectedTag) :
        StaticText{theme::Element::Label, std::move(text), alignment, ProtectedTag{}} {}

    // defaults
    ~Label() override = default;

public:
    /// Create a label with an initial terminal string.
    /// @param text The initial text.
    /// @param alignment The alignment used when painting.
    /// @return The new label.
    [[nodiscard]] static auto create(String text = {}, Alignment alignment = Alignment::TopLeft) -> LabelPtr {
        auto result = std::make_shared<Label>(std::move(text), alignment, ProtectedTag{});
        result->initializeUi();
        return result;
    }
    /// Create a label with an initial UTF-8 text.
    /// @param text The initial text.
    /// @param alignment The alignment used when painting.
    /// @return The new label.
    [[nodiscard]] static auto create(const std::string_view text, const Alignment alignment = Alignment::TopLeft)
        -> LabelPtr {
        return create(String{text, EncodingErrors::Replace}, alignment);
    }
};

}
