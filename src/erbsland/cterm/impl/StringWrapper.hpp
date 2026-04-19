// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "../StringView.hpp"

#include <utility>
#include <vector>

namespace erbsland::cterm::impl {

/// Helper that keeps string wrapping and line splitting logic out of the `String` API type.
class StringWrapper final {
public:
    /// Create a wrapper tool for one terminal string.
    /// @param str The string view to process.
    explicit StringWrapper(const StringView &str) noexcept : _str{str} {}

    // default/delete
    ~StringWrapper() = default;
    StringWrapper(const StringWrapper &) = delete;
    StringWrapper(StringWrapper &&) = delete;
    auto operator=(const StringWrapper &) -> StringWrapper & = delete;
    auto operator=(StringWrapper &&) -> StringWrapper & = delete;

public:
    /// Wrap the referenced string into terminal-width limited lines.
    /// @param width The maximum terminal width in cells. Must be greater than zero.
    /// @param paragraphSpacing The spacing to use between newline-separated paragraphs.
    /// @return A sequence of wrapped lines.
    [[nodiscard]] auto wrapIntoLines(int width, ParagraphSpacing paragraphSpacing) const noexcept
        -> std::vector<String>;
    /// Split the referenced string at newline characters.
    /// @return A sequence of lines without the newline characters.
    [[nodiscard]] auto splitLines() const noexcept -> std::vector<String>;

private:
    [[nodiscard]] auto wrapParagraphIntoLines(int width) const noexcept -> std::vector<String>;
    /// Reset the pending spacing token that is only emitted when followed by another word on the same line.
    static void clearPendingSpacing(String &pendingSpacing, int &pendingSpacingWidth) noexcept;
    /// Finish the current wrapped line and append it to the result.
    static void finishWrappedLine(
        String &line,
        int &lineWidth,
        String &pendingSpacing,
        int &pendingSpacingWidth,
        std::vector<String> &lines) noexcept;
    /// Store a spacing token until the next word is known to fit on the current line.
    static void appendSpacingToken(
        String &spacing,
        int spacingWidth,
        const String &line,
        String &pendingSpacing,
        int &pendingSpacingWidth) noexcept;
    /// Append one word token to the current wrapped output.
    static void appendWordToken(
        String &word,
        int wordWidth,
        int width,
        String &line,
        int &lineWidth,
        String &pendingSpacing,
        int &pendingSpacingWidth,
        std::vector<String> &lines) noexcept;
    /// Start a new wrapped line with the given word or split it into standalone lines if it is too wide.
    static void startWrappedLine(
        String &word, int wordWidth, int width, String &line, int &lineWidth, std::vector<String> &lines) noexcept;
    /// Split one oversized word into separate wrapped lines.
    static void
    splitWordIntoWrappedLines(const String &word, int wordWidth, int width, std::vector<String> &lines) noexcept;
    /// Find the exclusive end index for the next part of an oversized word.
    [[nodiscard]] static auto
    findWrappedWordSplitIndex(const StringView &word, std::size_t startIndex, int width) noexcept -> std::size_t;

private:
    const StringView &_str;
};

}
