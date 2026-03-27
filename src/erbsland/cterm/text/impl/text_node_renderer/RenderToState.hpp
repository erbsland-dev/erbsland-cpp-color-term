// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


namespace erbsland::cterm::text::impl::text_node_renderer {


/// State used while streaming planned blocks to a cursor writer.
struct RenderToState final {
    /// `true` until the first block has been emitted.
    bool firstBlock{true};
    /// The bottom margin of the previously emitted block.
    int previousBottomMargin{0};
};


}
