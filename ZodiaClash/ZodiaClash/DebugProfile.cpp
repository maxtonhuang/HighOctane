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
    void DebugProfiling::StartTimer(std::shared_ptr<System> systemInput, uint64_t startTimeInput) {\

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
                double usedMemoryMB = static_cast<double>(usedMemory) / (1024 * 1024); // Convert bytes to megabytes
                DebugPrint("Used memory %.2f MB", usedMemoryMB);
            }

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