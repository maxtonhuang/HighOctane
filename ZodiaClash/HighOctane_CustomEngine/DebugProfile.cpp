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
*	@file		DebugProfile.cpp
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
*   This file contains all the definition of the functions for debug profiling
*
******************************************************************************/

#include "DebugProfile.h"

#if ENABLE_DEBUG_PROFILE
/*!
 * \brief Start timer for profiling a system.
 *
 * This function starts a timer to measure the time taken by a system for profiling.
 *
 * \param systemInput : A shared pointer to the system being profiled.
 * \param startTimeInput : The start time for profiling.
 *
 */
void DebugProfiling::StartTimer(std::string systemName, uint64_t startTimeInput) {

    // To get the time taken for each system
    startTimers[systemName] = startTimeInput;
}

/*!
 * \brief Stop timer for profiling a system.
 *
 * This function stops the timer and calculates the time taken by a system for profiling.
 *
 * \param systemInput : A shared pointer to the system being profiled.
 * \param endTimeInput : The end time for profiling.
 *
 */
void DebugProfiling::StopTimer(std::string systemName, uint64_t endTimeInput) {

    // To get the time taken for each system
    duration[systemName] += endTimeInput - startTimers[systemName];
}

/*!
 * \brief Resets timer for profiling a system
 *
 * This function resets the time used by the system at the start of each frame
 *
 * \param systemInput : A shared pointer to the system being profiled.
 *
 */
void DebugProfiling::ResetTimer(std::string systemName) {
    duration[systemName] = 0;
}

/*!
 * \brief Get the percentage of time taken by a system.
 *
 * This function calculates and returns the percentage of time taken by a system
 * in relation to the frame time.
 *
 * \param systemInput : A shared pointer to the system being profiled.
 *
 * \return The percentage of time taken by the system.
 */
float DebugProfiling::GetPercentage(std::string systemName) {
    // To get the percentage of time taken for each system
    return (static_cast<float>(duration[systemName]) / (g_dt * 10'000.f));;
}

/*!
 * \brief Get the duration of time taken by a system.
 *
 * This function returns the time taken by a system for profiling.
 *
 * \param systemInput : A shared pointer to the system being profiled.
 *
 * \return The time taken by the system.
 */
float DebugProfiling::GetDuration(std::string systemName) {

    // To get the time taken for each system
    return static_cast<float>(duration[systemName]);
}

void DebugProfiling::ResetTimers() {
    for (std::pair<const std::string, uint64_t> & val : startTimers) {
        val.second = ((val.first == "Level Editor") ? val.second : 0);
    }
    for (std::pair<const std::string, uint64_t> & val : duration) {
        val.second = ((val.first == "Level Editor") ? val.second : 0);
    }
}

/*!
 * \brief Get memory usage.
 *
 * This function retrieves and returns the memory usage of the process at a given time.
 *
 * \param time : The time at which memory usage is measured.
 *
 * \return The memory usage in megabytes.
 */
float GetMemoryUsage() {
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

/*!
 * \brief Get CPU usage.
 *
 * This function retrieves and returns the CPU usage of the process at a given time.
 *
 * \param time : The time at which CPU usage is measured.
 *
 * \return The CPU usage percentage.
 */
float GetCPUUsage() {

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
        float idlePercent = static_cast<float>(((idle.QuadPart) / total) * 100.0);
        float usagePercent = static_cast<float>(100.0 - idlePercent);

        // If CPU usage is more than 90%
        //ASSERT(usagePercent > 90.f, "CPU usage is more than 90%!");
        return usagePercent;
    }
    return 0.f;
}
#endif
