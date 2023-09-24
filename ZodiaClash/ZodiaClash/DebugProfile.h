/*
\copyright
		All content(C) 2023 DigiPen Institute of Technology Singapore.All rights
		reserved.Reproduction or disclosure of this file or its contents without the prior
		written consent of DigiPen Institute of Technology is prohibited.
*/
/*!
@file		DebugProfile.h
@author		Koh Wen Yuan
@Email		k.wenyuan@digipen.edu
@course		CSD 2401
@section	Section A
@date		18 September 2023
@brief		This file contains the functions declaration for debugging


TODO :
DEFINE IN CPP FILE
Add the macro to run only in debug mode

*//*______________________________________________________________________*/
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

    // Performance data
    void PerformanceDataHandler(uint64_t time);

#if _WIN32
#define Performance(time) PerformanceDataHandler(time);

#else
#define Performance(time) ((void)0)
#endif

#else
#define Performance(time) ((void)0);
#endif
