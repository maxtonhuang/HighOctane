/*
\copyright
		All content(C) 2023 DigiPen Institute of Technology Singapore.All rights
		reserved.Reproduction or disclosure of this file or its contents without the prior
		written consent of DigiPen Institute of Technology is prohibited.
*/
/*!
@file		DebugProfile.cpp
@author		Koh Wen Yuan
@Email		k.wenyuan@digipen.edu
@course		CSD 2401
@section	Section A
@date		18 September 2023
@brief		This file contains the functions declaration for debugging

*//*______________________________________________________________________*/
#include "DebugProfile.h"

#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
    void DebugProfiling::StartTimer(std::shared_ptr<System> systemInput, uint64_t startTimeInput) {

        // To get the time taken for each system
        startTimers[systemInput] = static_cast<float>(startTimeInput);
    }

    void DebugProfiling::StopTimer(std::shared_ptr<System> systemInput, uint64_t endTimeInput) {

        // To get the time taken for each system
        endTimers[systemInput] = static_cast<float>(endTimeInput - startTimers[systemInput]) / 1000.0f;
    }

    float DebugProfiling::GetPercentage(std::shared_ptr<System> systemInput) {
        
        // To get the percentage of time taken for each system
        return percentages[systemInput] = (endTimers[systemInput] / (g_dt * 1000.f) * 100.f);
	}

    float DebugProfiling::GetDuration(std::shared_ptr<System> systemInput) {
        
        // To get the time taken for each system
        return endTimers[systemInput];
	}

    // To get memory usage for each system
    float GetMemoryUsage(uint64_t time) {
        PROCESS_MEMORY_COUNTERS pmc;

        // To get the memory usage in bytes
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            ASSERT(!GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)), "Unable to get memory");

            // Working set size in bytes
            SIZE_T usedMemory = pmc.WorkingSetSize;
            float usedMemoryMB = static_cast<float>(usedMemory) / (1024 * 1024); // Convert bytes to megabytes

            return usedMemoryMB;
        }
		return 0.f;
    }

    // To get CPU usage
    float GetCPUUsage(uint64_t time) {

        FILETIME idleTime, kernelTime, userTime;

        // To get the CPU percentage usage
        if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
            ULARGE_INTEGER idle{}, kernel{}, user{};
            idle.LowPart = idleTime.dwLowDateTime;
            idle.HighPart = idleTime.dwHighDateTime;
            kernel.LowPart = kernelTime.dwLowDateTime;
            kernel.HighPart = kernelTime.dwHighDateTime;
            user.LowPart = userTime.dwLowDateTime;
            user.HighPart = userTime.dwHighDateTime;

            // Calculate CPU usage percentage
            float total = static_cast<float>(kernel.QuadPart + user.QuadPart);
            float idlePercent = (static_cast<float>(idle.QuadPart) / total) * 100.0;
            float usagePercent = 100.0 - idlePercent;

            // If CPU usage is more than 50%
            ASSERT(usagePercent > 50.f, "CPU usage is more than 50%!");
            return usagePercent;
        }
        return 0.f;
    }
#endif