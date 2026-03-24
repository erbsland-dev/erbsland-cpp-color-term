// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>


namespace erbsland::cterm {

/// How paragraph rendering extends the background color beyond the visible text.
///
/// These modes are most visible when the rendered text uses a non-default background color and the paragraph wraps
/// across multiple physical lines. Depending on the chosen mode, the renderer can extend that background into the
/// unused cells on the right side of a wrapped line, into the indentation area of the continuation line, or both.
enum class ParagraphBackgroundMode : uint8_t {
    /// Keep the buffer background outside the rendered characters.
    ///
    /// No extra cells are painted on the right side of wrapped lines, and continuation indents keep their existing
    /// background color.
    Default,
    /// Extend the wrapped-line background into the left indentation area only.
    ///
    /// The indentation area of each wrapped continuation line uses the background color of the last visible
    /// character on the previous physical line. The right side of the line is left unchanged.
    /// @code
    /// | This is a very long               |
    /// | <fill here> wrapped line.         |
    /// @endcode
    WrappedLeft,
    /// Extend the wrapped-line background into the remaining cells on the right side only.
    ///
    /// Each wrapped physical line fills the unused cells up to the available width with the background color of its
    /// last visible character. Indentation keeps the existing buffer background.
    /// @code
    /// | This is a very long <filled here> |
    /// |     wrapped line.                 |
    /// @endcode
    WrappedRight,
    /// Extend the wrapped-line background on both sides of wrapped lines.
    ///
    /// Wrapped lines fill the remaining cells on the right side, and the continuation indent on the next physical
    /// line uses that same background color.
    /// @code
    /// | This is a very long <filled here> |
    /// | <and here> wrapped line.          |
    /// @endcode
    WrappedBoth,
    /// Extend the background into the right-side remainder on every physical line.
    ///
    /// This behaves like `WrappedRight`, but also fills the last physical line of the paragraph.
    /// @code
    /// | This is a very long  <filled here> |
    /// |     wrapped line.       <and here> |
    /// @endcode
    FullRight,
    /// Extend the background on both sides for every physical line.
    ///
    /// This behaves like `WrappedBoth`, but also keeps filling the right-side remainder on the last physical line.
    /// @code
    /// | This is a very long   <filled here> |
    /// | <and here> wrapped line. <and here> |
    /// @endcode
    FullBoth,
};

}
