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
PUT THE PERFORMANCE() AT THE END OF THE GAME LOOP

*//*______________________________________________________________________*/
#pragma once

#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>
#include <iostream>
#include "enginecore.h"

extern float g_dt;

namespace debugprofile {

    enum class DebugSystems {
        GameLoop,
        Physics,
        Graphics,
        count
    };


    struct ProfileResult {
        std::shared_ptr<System> name{};
        float duration{};
        float percentage{};
    };

    class DebugProfiling {
    public:

        //DebugProfiling() {
        //    currentSessionName = DebugSystems::count;
        //    results.reserve(static_cast<size_t>(DebugSystems::count));
        //    loopCount = 0;
        //    totalFrameTimeMillis = 0.0f;
        //}

        //void BeginSession(DebugSystems SessionInput) {
        //    currentSessionName = SessionInput;
        //    profileSessionActive = true;
        //    loopCount = 0;
        //    totalFrameTimeMillis = 0.0f;
        //}

        //void EndSession() {
        //    profileSessionActive = false;
        //}

        //void StartTimer(DebugSystems systemInput) {
        //    if (!profileSessionActive) return;
        //    timers[systemInput] = std::chrono::high_resolution_clock::now();
        //}

        //void StopTimer(DebugSystems systemInput) {
        //    if (!profileSessionActive) return;
        //    auto startTime = timers[systemInput];
        //    auto endTime = std::chrono::high_resolution_clock::now();
        //    auto duration = endTime - startTime;
        //    ProfileResult result{};
        //    result.name = systemInput;
        //    result.durationMillis = std::chrono::duration<float, std::milli>(duration).count();
        //    results.push_back(result);
        //}

        //void BeginFrame() {
        //    if (!profileSessionActive) return;
        //    frameStartTime = std::chrono::high_resolution_clock::now();
        //}

        //void EndFrame() {
        //    if (!profileSessionActive) return;
        //    loopCount++;
        //    auto frameEndTime = std::chrono::high_resolution_clock::now();
        //    auto frameDuration = frameEndTime - frameStartTime;
        //    float frameTimeMillis = std::chrono::duration<float, std::milli>(frameDuration).count();
        //    totalFrameTimeMillis += frameTimeMillis;

        //    // Calculate and store the percentage for each system
        //    for (auto& result : results) {
        //        result.percentage = (result.durationMillis / totalFrameTimeMillis) * 100.0f;
        //    }
        //}

        //ProfileResult GetResult(DebugSystems systemInput) {
        //    for (auto& result : results) {
        //        if (result.name == systemInput) {
        //            return result;
        //        }
        //    }
        //    return ProfileResult();
        //}


        DebugProfiling() {
            //currentSessionName = DebugSystems::count;
            results.reserve(static_cast<size_t>(DebugSystems::count));
        }


        void StartTimer(std::shared_ptr<System> systemInput, uint64_t startTimeInput) {
            timers[systemInput] = startTimeInput;
		}

        void StopTimer(std::shared_ptr<System> systemInput, uint64_t endTimeInput) {
            auto startTime = timers[systemInput];
            auto duration = static_cast<float>(endTimeInput - startTime) / 1000.0f;

            ProfileResult result{};
            result.name = systemInput;
            result.duration = duration;

            results.push_back(result);
        }

        ProfileResult GetResult(std::shared_ptr<System> systemInput) {
            for (auto& result : results) {
                if (result.name == systemInput) {
                    result.percentage = (result.duration / (g_dt * 1000.f) * 100.f);
                    return result;
                }
            }
            return ProfileResult();
        }

        void clear() {
			results.clear();
		}

    private:
        std::vector<ProfileResult> results;
        std::shared_ptr<System> currentSessionName;
        std::unordered_map<std::shared_ptr<System>, uint64_t> timers;

    };

} // namespace debugprofile



/*

startTime = 1000;
endTime = 1000000;

timeTaken = static_cast<float>(endTime - startTime) / 1'000.f
std::cout << "Time Taken for System :" << timeTaken << " ms." << std::endl;
std::cout << "Percentage of Loop Time: " << (timeTaken / (g_dt * 1'000)) << std::endl;


*/

