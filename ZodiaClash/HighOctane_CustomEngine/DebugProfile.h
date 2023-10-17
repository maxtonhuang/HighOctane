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
*   ENABLE_DEBUG_PROFILE is a macro that can be used to enable/disable debug,
*   it will eventually be changed to a macro that controls whether ImGui will
*   be on in the future
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


#if ENABLE_DEBUG_PROFILE

#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>
#include <iostream>
#include "enginecore.h"

    class DebugProfiling {
    public:

        void StartTimer(/*std::shared_ptr<System> systemInput*/std::string systemName, uint64_t startTimeInput);
        void StopTimer(/*std::shared_ptr<System> systemInput*/std::string systemName, uint64_t endTimeInput);

        float GetPercentage(/*std::shared_ptr<System> systemInput*/std::string systemName);
        float GetDuration(/*std::shared_ptr<System> systemInput*/std::string systemName);

    private:
        std::shared_ptr<System> currentSessionName;
        std::unordered_map<std::shared_ptr<System>, uint64_t> timers;
        std::unordered_map</*std::shared_ptr<System>*/std::string, float> startTimers;
        std::unordered_map</*std::shared_ptr<System>*/std::string, float> endTimers;
        std::unordered_map</*std::shared_ptr<System>*/std::string, float> percentages;

    };

    extern DebugProfiling debugSysProfile;

    float GetMemoryUsage();

    float GetCPUUsage();

#if _WIN32
#define Performance() PerformanceDataHandler();

#else
#define Performance() ((void)0)
#endif

#else
#define Performance() ((void)0);
#endif
