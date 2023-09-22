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


// ENABLE/DISABLE DEBUG DIAGNOSTICS
#define ENABLE_DEBUG_PROFILE 0

// Print interval in milliseconds
#define PRINT_INTERVAL 5'000'000


#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
extern float g_dt;

#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>
#include <iostream>
#include "enginecore.h"

    struct ProfileResult {
        std::shared_ptr<System> name{};
        float duration{};
        float percentage{};
    };

    struct FPSResult {
        float fps{};
    };

    class DebugProfiling {
    public:

        void StartTimer(std::shared_ptr<System> systemInput, uint64_t startTimeInput);

        void StopTimer(std::shared_ptr<System> systemInput, uint64_t endTimeInput);

        ProfileResult GetResult(std::shared_ptr<System> systemInput);
        void clear();

    private:
        std::vector<ProfileResult> results;
        std::shared_ptr<System> currentSessionName;
        std::unordered_map<std::shared_ptr<System>, uint64_t> timers;

    };

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
