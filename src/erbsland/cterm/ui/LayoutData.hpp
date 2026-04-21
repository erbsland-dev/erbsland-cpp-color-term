// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "ui_namespace.hpp"

#include <memory>

namespace erbsland::cterm::ui {

/// Base class for metadata attached to one parent-child surface relation.
class LayoutData {
public:
    // defaults
    virtual ~LayoutData() = default;
};

using LayoutDataPtr = std::shared_ptr<LayoutData>;

}
