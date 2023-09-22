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

TO DO 
PERCENTAGE AND THE TIME GOT PROBLEM WHEN FPS IS UNCAPPED

*//*______________________________________________________________________*/
#include "DebugProfile.h"



#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
    void DebugProfiling::StartTimer(std::shared_ptr<System> systemInput, uint64_t startTimeInput) {
        timers[systemInput] = startTimeInput;
    }

    void DebugProfiling::StopTimer(std::shared_ptr<System> systemInput, uint64_t endTimeInput) {
        auto startTime = timers[systemInput];
        auto duration = static_cast<float>(endTimeInput - startTime) / 1000000.0f;

        ProfileResult result{};
        result.name = systemInput;
        result.duration = duration;

        results.emplace_back(result);
    }

    ProfileResult DebugProfiling::GetResult(std::shared_ptr<System> systemInput) {
        for (auto& result : results) {
            if (result.name == systemInput) {
                result.percentage = (result.duration / (g_dt ) * 100.f);
                return result;
            }
    }
    return ProfileResult();
    }

    void DebugProfiling::clear() {
        results.clear();
    }

    // Function to print out the memory usage
    void PerformanceDataHandler(uint64_t time) {

        PROCESS_MEMORY_COUNTERS pmc;

        // For the CPU usage
        FILETIME idleTime, kernelTime, userTime;

        static uint64_t lLastTime = 0;
        // Prints it out once at intervals
        if (time - lLastTime > PRINT_INTERVAL) {
            lLastTime = time;
            // To get the memory usage in bytes
            if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
                Assert(!GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)), "Unable to get memory");
                // Working set size in bytes
                SIZE_T usedMemory = pmc.WorkingSetSize;
                std::cout << "Used memory: " << usedMemory / (MAX_FILE_SIZE) << " MB" << std::endl;

            }

            // To get the CPU percentage usage
            if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
                ULARGE_INTEGER idle, kernel, user;
                idle.LowPart = idleTime.dwLowDateTime;
                idle.HighPart = idleTime.dwHighDateTime;
                kernel.LowPart = kernelTime.dwLowDateTime;
                kernel.HighPart = kernelTime.dwHighDateTime;
                user.LowPart = userTime.dwLowDateTime;
                user.HighPart = userTime.dwHighDateTime;

                // Calculate CPU usage percentage
                double total = static_cast<double>(kernel.QuadPart + user.QuadPart);
                double idlePercent = (static_cast<double>(idle.QuadPart) / total) * 100.0;
                double usagePercent = 100.0 - idlePercent;
                std::cout << "CPU Usage: " << usagePercent << "%" << std::endl;

                // If CPU usage is more than 50%
                Assert(usagePercent > 50.f, "CPU usage is more than 50%!");
            }
        }
    }
#endif