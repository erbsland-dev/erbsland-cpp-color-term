// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "support/TestHelper.hpp"

#include <memory>
#include <string>

class ReadableBufferDispatchProbe final : public ReadableBuffer {
public:
    [[nodiscard]] auto size() const noexcept -> Size override { return _buffer.size(); }

    [[nodiscard]] auto rect() const noexcept -> Rectangle override { return _buffer.rect(); }

    [[nodiscard]] auto get(const Position pos) const noexcept -> const Char & override { return _buffer.get(pos); }

    [[nodiscard]] auto clone() const -> WritableBufferPtr override { return std::make_shared<Buffer>(_buffer); }

public:
    mutable std::u32string _characters;
    mutable bool _invert = false;

protected:
    [[nodiscard]] auto toMaskImpl(const std::u32string &characters, const bool invert) -> Bitmap override {
        _characters = characters;
        _invert = invert;
        return Bitmap{_buffer.size()};
    }

private:
    Buffer _buffer{Size{2, 1}};
};
