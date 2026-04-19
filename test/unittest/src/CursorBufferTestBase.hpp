// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "erbsland/cterm/CursorBuffer.hpp"

#include <erbsland/cterm/all.hpp>
#include <erbsland/unittest/UnitTest.hpp>

#include <format>
#include <string>
#include <string_view>
#include <vector>

// import everything into the global namespace
using namespace erbsland::cterm;

// create an alias to resolve name conflicts.
namespace term = erbsland::cterm;

class CursorBufferTestBase : public el::UnitTest {
public:
    using Lines = std::vector<std::string>;

public:
    CursorBuffer buffer{Size{20, 5}, CursorBuffer::OverflowMode::Wrap};

public:
    auto rawLinesFromBuffer() const -> Lines {
        std::vector<std::string> lines;
        for (int y = 0; y < buffer.size().height(); ++y) {
            std::string line;
            line.reserve(buffer.size().width() * 2);
            for (int x = 0; x < buffer.size().width(); ++x) {
                buffer.get(Position{x, y}).appendTo(line);
            }
            lines.emplace_back(std::move(line));
        }
        return lines;
    }
};
