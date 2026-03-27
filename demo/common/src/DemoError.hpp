// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <exception>
#include <stdexcept>

namespace demo {

class DemoError : public std::runtime_error {
public:
    explicit DemoError(const std::string &message) noexcept : std::runtime_error(message) {}
    ~DemoError() noexcept override = default;
};

}
