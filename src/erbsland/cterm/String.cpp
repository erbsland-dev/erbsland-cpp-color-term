// Copyright © 2026 by Tobias Erbsland / Erbsland DEV.
// SPDX-License-Identifier: CC-BY-4.0
#include "String.hpp"

#include "impl/U8Buffer.hpp"
#include "impl/UnicodeWidth.hpp"


namespace erbsland::cterm {


String::String(const std::string_view str) : _chars{splitCharacters(str)} {
}


auto String::displayWidth() const noexcept -> int {
    auto result = 0;
    for (const auto &character : _chars) {
        result += character.displayWidth();
    }
    return result;
}


auto String::splitWords() const noexcept -> std::vector<String> {
    std::vector<String> words;
    String word;
    word.reserve(100);
    for (const auto &character : *this) {
        if (character.isSpacing()) {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word.append(character);
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}


auto String::wrapIntoLines(const int width) const noexcept -> std::vector<String> {
    if (width <= 0) {
        return {};
    }
    auto lines = BlockStringLines{};
    auto isFirstParagraph = true;
    for (const auto &paragraph : splitParagraphs()) {
        const auto paragraphLines = paragraph.wrapParagraphIntoLines(width);
        if (paragraphLines.empty()) {
            continue;
        }
        if (!isFirstParagraph) {
            lines.emplace_back();
        }
        lines.insert(lines.end(), paragraphLines.begin(), paragraphLines.end());
        isFirstParagraph = false;
    }
    return lines;
}


auto String::splitParagraphs() const noexcept -> std::vector<String> {
    auto paragraphs = std::vector<String>{};
    auto paragraph = String{};
    paragraph.reserve(_chars.size());
    for (const auto &character : _chars) {
        if (character.charStr() == "\r" || character.charStr() == "\n") {
            if (!paragraph.empty()) {
                paragraphs.push_back(paragraph);
                paragraph.clear();
            }
            continue;
        }
        paragraph.append(character);
    }
    if (!paragraph.empty()) {
        paragraphs.push_back(paragraph);
    }
    return paragraphs;
}


auto String::wrapParagraphIntoLines(const int width) const noexcept -> std::vector<String> {
    if (width <= 0) {
        return {};
    }
    BlockStringLines lines;
    String line;
    String pendingSpacing;
    int lineWidth = 0;
    auto appendSplitWord = [&](const String &word) -> void {
        const auto wordWidth = word.displayWidth();
        if (wordWidth > width) {
            if (!line.empty()) {
                lines.push_back(line);
                line.clear();
                lineWidth = 0;
            }
            std::size_t index = 0;
            while (index < word.size()) {
                String part;
                int partWidth = 0;
                while (index < word.size()) {
                    const auto characterWidth = word[index].displayWidth();
                    if (!part.empty() && partWidth + characterWidth > width) {
                        break;
                    }
                    if (part.empty() && characterWidth > width) {
                        part += word[index++];
                        partWidth += characterWidth;
                        break;
                    }
                    if (partWidth + characterWidth > width) {
                        break;
                    }
                    part += word[index++];
                    partWidth += characterWidth;
                }
                lines.push_back(part);
            }
            return;
        }
        line = word;
        lineWidth = wordWidth;
    };
    auto processWord = [&](const String &word) -> void {
        const auto wordWidth = word.displayWidth();
        const auto spacingWidth = pendingSpacing.displayWidth();
        if (lineWidth == 0) {
            pendingSpacing.clear();
            appendSplitWord(word);
            return;
        }
        if (lineWidth + spacingWidth + wordWidth <= width) {
            line += pendingSpacing;
            line += word;
            lineWidth += spacingWidth + wordWidth;
            pendingSpacing.clear();
            return;
        }
        lines.push_back(line);
        line.clear();
        lineWidth = 0;
        pendingSpacing.clear();
        appendSplitWord(word);
    };
    auto token = String{};
    auto isSpacingToken = false;
    auto processToken = [&](const String &currentToken, const bool currentIsSpacing) -> void {
        if (currentToken.empty()) {
            return;
        }
        if (currentIsSpacing) {
            if (!line.empty()) {
                pendingSpacing += currentToken;
            }
            return;
        }
        processWord(currentToken);
    };
    for (const auto &character : *this) {
        const auto currentIsSpacing = character.isSpacing();
        if (token.empty()) {
            token.append(character);
            isSpacingToken = currentIsSpacing;
            continue;
        }
        if (currentIsSpacing == isSpacingToken) {
            token.append(character);
            continue;
        }
        processToken(token, isSpacingToken);
        token.clear();
        token.append(character);
        isSpacingToken = currentIsSpacing;
    }
    processToken(token, isSpacingToken);
    if (!line.empty()) {
        lines.push_back(line);
    }
    return lines;
}


auto String::splitCharacters(const std::string_view str) -> std::vector<Char> {
    auto result = std::vector<Char>{};
    result.reserve(str.size());
    impl::U8Buffer{str}.decodeAll([&](const char32_t codePoint) -> void {
        std::string characterText;
        impl::U8Buffer<const char>::encodeChar(characterText, codePoint);
        if (impl::consoleCharacterWidth(codePoint) == 0 && codePoint >= 0x20 && !result.empty()) {
            result.back() = Char{result.back().charStr() + characterText, result.back().color()};
            return;
        }
        result.emplace_back(characterText, Color{});
    });
    return result;
}


}
