// Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
// SPDX-License-Identifier: Apache-2.0
#pragma once

// import all color term headers and libraries for the unittests.

#include <erbsland/cterm/all.hpp>
#include <erbsland/cterm/impl/paragraph/LayoutResult.hpp>
#include <erbsland/cterm/text/all.hpp>
#include <erbsland/cterm/text/impl/text_node_renderer/BlockKind.hpp>
#include <erbsland/cterm/ui/all.hpp>

// import everything into the global namespace
using namespace erbsland::cterm;

// Aliases
namespace tnr = text::impl::text_node_renderer;
namespace textimpl = text::impl;
namespace paragraph = impl::paragraph;
namespace termimpl = impl;
