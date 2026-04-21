// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "BlockStream.hpp"
#include "InlineTextRenderer.hpp"
#include "ListItemLayout.hpp"
#include "RenderContext.hpp"
#include "StyleResolver.hpp"

#include "../../Style.hpp"
#include "../../TextNode.hpp"

#include <optional>
#include <vector>

namespace erbsland::cterm::text::impl::text_node_renderer {

/// Streams a `TextNode` tree into render blocks.
class TreeRenderer final {
public:
    /// Create a tree renderer.
    /// @param style The style to use for selector resolution.
    /// @param target The block target to write to.
    TreeRenderer(StyleConstPtr style, BlockTarget &target);

    // defaults
    ~TreeRenderer() = default;
    TreeRenderer(const TreeRenderer &) = delete;
    TreeRenderer(TreeRenderer &&) = delete;
    auto operator=(const TreeRenderer &) -> TreeRenderer & = delete;
    auto operator=(TreeRenderer &&) -> TreeRenderer & = delete;

public:
    /// Render one document tree.
    /// @param document The document node to render. `nullptr` produces empty output.
    void render(const TextNodeConstPtr &document);

private:
    struct BlockScope final {
        Margins verticalMargins;
        std::optional<ListItemLayout> listItemLayout;
        bool hasBlocks{false};
    };

private:
    void appendNode(const TextNode &node, const RenderContext &context);
    void appendContainer(const TextNode &node, StyleRole role, const RenderContext &context);
    void appendList(const TextNode &node, bool numbered, const RenderContext &context);
    void appendListItem(
        const TextNode &node,
        const StyleRule &listItemRule,
        const ListItemLayout &listItemLayout,
        const RenderContext &context);
    void appendListItemChild(const TextNode &node, const RenderContext &context);
    void appendParagraphLikeNode(const TextNode &node, StyleRole role, const RenderContext &context);

    [[nodiscard]] auto paragraph(const TextNode &node, const StyleRule &blockRule, const RenderContext &context)
        -> RenderBlock;
    [[nodiscard]] auto paragraph(String text, const StyleRule &blockRule, const RenderContext &context) -> RenderBlock;
    [[nodiscard]] auto heading(const TextNode &node, const RenderContext &context) -> RenderBlock;
    [[nodiscard]] auto horizontalRule(const RenderContext &context) -> RenderBlock;
    [[nodiscard]] auto emptyListItemBlock(const RenderContext &context) -> RenderBlock;
    [[nodiscard]] auto
    makeListItemLayout(const StyleRule &listItemRule, const RenderContext &context, std::size_t number)
        -> ListItemLayout;

    void emitBlock(RenderBlock &&block);
    void openScope(const Margins &verticalMargins, std::optional<ListItemLayout> listItemLayout = std::nullopt);
    void closeScope();
    [[nodiscard]] auto currentListItemHasBlocks() const noexcept -> bool;
    void collapseVerticalMargin(RenderBlock &block, Margins::Side side, Coordinate margin);
    void appendResolvedDecoration(
        cterm::impl::StringBuilder &builder, const std::optional<String> &decoration, const CharStyle &textStyle);
    [[nodiscard]] auto resolvedDecoration(const std::optional<String> &decoration, const CharStyle &textStyle)
        -> std::optional<String>;
    [[nodiscard]] static auto collapsedVerticalMarginValue(Coordinate first, Coordinate second) noexcept -> Coordinate;
    [[nodiscard]] static auto listItemParagraphRule() -> StyleRule;
    [[nodiscard]] static auto emptyListItemRule() -> StyleRule;

private:
    StyleResolver _styleResolver;           ///< Resolves all block and inline style rules.
    InlineTextRenderer _inlineTextRenderer; ///< Renders inline text subtrees.
    BlockStream _stream;                    ///< Streams blocks to the target.
    std::vector<BlockScope> _scopes;        ///< Open container and list-item scopes.
};

}
