// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <cstdint>

namespace erbsland::cterm::text::impl {

enum class HtmlTokenType : uint8_t {
    End,            ///< Sentinel token that marks the end of the token stream.
    Text,           ///< Plain text content.
    TagOpen,        ///< An opening tag name like `strong`.
    TagClose,       ///< A closing tag name like `strong`.
    AttributeName,  ///< An attribute name like `class`.
    AttributeValue, ///< A decoded attribute value.
    DocType,        ///< A doctype declaration without the `<!DOCTYPE` prefix.
};

}
