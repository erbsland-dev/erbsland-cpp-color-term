// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <string>
#include <utility>

namespace erbsland::cterm::text::impl {

/// One parsed HTML attribute.
class HtmlAttribute final {
public:
    /// Create one parsed attribute.
    /// @param name The lower-case ASCII attribute name.
    /// @param value The attribute value.
    HtmlAttribute(std::u32string name = {}, std::u32string value = {}) :
        _name{std::move(name)}, _value{std::move(value)} {}

public:
    /// Access the lower-case ASCII attribute name.
    [[nodiscard]] auto name() const noexcept -> const std::u32string & { return _name; }
    /// Access the attribute value.
    [[nodiscard]] auto value() const noexcept -> const std::u32string & { return _value; }
    /// Replace the attribute value.
    /// @param value The new attribute value.
    void setValue(std::u32string value) { _value = std::move(value); }

private:
    std::u32string _name;  ///< The lower-case ASCII attribute name.
    std::u32string _value; ///< The attribute value.
};

}
