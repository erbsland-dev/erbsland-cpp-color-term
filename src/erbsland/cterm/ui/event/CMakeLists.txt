# Copyright (c) 2026 Tobias Erbsland - https://erbsland.dev
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28)

add_subdirectory(impl)
target_sources(erbsland-color-term PRIVATE
        Event.hpp
        EventData.hpp
        EventDriver.cpp
        EventDriver.hpp
        EventThread.cpp
        EventThread.hpp
        EventScheduler.cpp
        EventScheduler.hpp
        StopToken.hpp
        EventType.hpp
        KeyPressEvent.hpp
        QuitEvent.hpp
        ScheduledActionRef.hpp
        Scheduler.cpp
        Scheduler.hpp
)
