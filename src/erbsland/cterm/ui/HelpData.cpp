// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#include "HelpData.hpp"

#include <utility>

namespace erbsland::cterm::ui {

HelpData::HelpData(std::string name) : _name{std::move(name)} {
}

auto HelpData::setName(std::string name) -> HelpData & {
    _name = std::move(name);
    return *this;
}

auto HelpData::setDescription(std::string description) -> HelpData & {
    _description = std::move(description);
    return *this;
}

auto HelpData::setPriority(const int priority) noexcept -> HelpData & {
    _priority = priority;
    return *this;
}

auto HelpData::setVisibility(const HelpVisibility visibility) noexcept -> HelpData & {
    _visibility = visibility;
    return *this;
}

auto HelpData::isVisibleInFooter() const noexcept -> bool {
    return _visibility == HelpVisibility::Footer || _visibility == HelpVisibility::All;
}

auto HelpData::isVisibleOnHelpPage() const noexcept -> bool {
    return _visibility == HelpVisibility::HelpPage || _visibility == HelpVisibility::All;
}

}
