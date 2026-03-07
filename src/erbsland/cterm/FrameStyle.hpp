// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once


#include <cstdint>


namespace erbsland::cterm {


/// Various box styles
enum class FrameStyle : uint8_t {
    // base styles
    Light,                ///< Light box `┌─┐`
    LightDoubleDash,      ///< Light box with double-dashed lines `┌╌┐`
    LightTripleDash,      ///< Light box with triple-dashed lines `┌┄┐`
    LightQuadrupleDash,   ///< Light box with quadruple-dashed lines `┌┈┐`
    Heavy,                ///< Heavy box `┏━┓`
    HeavyDoubleDash,      ///< Heavy box with double-dashed lines `┏╍┓`
    HeavyTripleDash,      ///< Heavy box with triple-dashed lines `┏┅┓`
    HeavyQuadrupleDash,   ///< Heavy box with quadruple-dashed lines `┏┉┓`
    Double,               ///< Double box `╔═╗`
    FullBlock,            ///< Solid block frame `█`
    FullBlockWithChamfer, ///< Solid block frame with chamfered corners `◢█◣`
    OuterHalfBlock,       ///< Half-block frame drawn on the outer cell edges.
    InnerHalfBlock,       ///< Half-block frame drawn on the inner cell edges.
    // combined styles
    LightWithRoundedCorners, ///< Light box with rounded corners `╭─╮`
};


}
