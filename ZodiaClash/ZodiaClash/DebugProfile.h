/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		DebugProfile.h
*
*	@author		Koh Wen Yuan
*
*	@email		k.wenyuan\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		18 September 2023
*
* *****************************************************************************
*
*	@brief		Debug profiling of different systems
* 
*   This file contains all the declaration of the functions for debug profiling
*
******************************************************************************/
#pragma once
#include "debuglog.h"
#include "ECS.h"
#include "Global.h"


// ENABLE/DISABLE DEBUG DIAGNOSTICS
#define ENABLE_DEBUG_PROFILE 1

// Print interval in milliseconds
#define PRINT_INTERVAL 5'000'000


#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE

#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>
#include <iostream>
#include "enginecore.h"

    class DebugProfiling {
    public:

        void StartTimer(std::shared_ptr<System> systemInput, uint64_t startTimeInput);
        void StopTimer(std::shared_ptr<System> systemInput, uint64_t endTimeInput);

        float GetPercentage(std::shared_ptr<System> systemInput);
        float GetDuration(std::shared_ptr<System> systemInput);

    private:
        std::shared_ptr<System> currentSessionName;
        std::unordered_map<std::shared_ptr<System>, uint64_t> timers;
        std::unordered_map<std::shared_ptr<System>, float> startTimers;
        std::unordered_map<std::shared_ptr<System>, float> endTimers;
        std::unordered_map<std::shared_ptr<System>, float> percentages;

    };

    extern DebugProfiling debugSysProfile;

    float GetMemoryUsage(uint64_t time);

    float GetCPUUsage(uint64_t time);

#if _WIN32
#define Performance(time) PerformanceDataHandler(time);

#else
#define Performance(time) ((void)0)
#endif

#else
#define Performance(time) ((void)0);
#endif
