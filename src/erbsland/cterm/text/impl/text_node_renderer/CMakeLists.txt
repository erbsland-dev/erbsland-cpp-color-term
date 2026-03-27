# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

target_sources(erbsland-color-term PRIVATE
        BlockEmitter.cpp
        BlockEmitter.hpp
        InlineTextPlanner.cpp
        InlineTextPlanner.hpp
        ListBlockPlanner.cpp
        ListBlockPlanner.hpp
        ListItemLayout.cpp
        ListItemLayout.hpp
        ListPrefix.hpp
        PlanningContext.cpp
        PlanningContext.hpp
        PlanningStyleResolver.cpp
        PlanningStyleResolver.hpp
        RenderBlock.hpp
        RenderBlockFactory.cpp
        RenderBlockFactory.hpp
        RenderPlanner.cpp
        RenderPlanner.hpp
        RenderToState.hpp
        TextNodePlanningQueries.cpp
        TextNodePlanningQueries.hpp
)
