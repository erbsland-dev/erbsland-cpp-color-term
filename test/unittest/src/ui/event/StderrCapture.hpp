// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <iostream>
#include <sstream>
#include <string>

class StderrCapture final {
public:
    StderrCapture() : _previousBuffer{std::cerr.rdbuf(_buffer.rdbuf())} {}

    ~StderrCapture() { std::cerr.rdbuf(_previousBuffer); }

    [[nodiscard]] auto output() const -> std::string { return _buffer.str(); }

private:
    std::ostringstream _buffer;
    std::streambuf *_previousBuffer = nullptr;
};
