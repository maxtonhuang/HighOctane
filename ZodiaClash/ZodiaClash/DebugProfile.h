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

namespace debugprofile {

    enum class DebugSystems {
        Physics,
        Graphics,
        count
    };

    struct ProfileResult {
        DebugSystems name;
        float duration;
        float percentage;
        int callCount;
    };

    class DebugProfiling {

    public:
        DebugProfiling() {
            
            // Reserve so that the vector has enough space
            results.reserve(static_cast<size_t>(DebugSystems::count));
        }

        void BeginSession(std::string sessionInput) {
            // Initialize a new profiling session
            currentSessionName = sessionInput;
            profileSessionActive = true;

            //std::cout << "Begin profiling session start" << std::endl;
        }

        void EndSession() {
            // Finalize the profiling session
            profileSessionActive = false;

            //std::cout << "End profiling session" << std::endl;
        }

        void StartTimer(DebugSystems systemInput, uint64_t timeStart) {
            if (!profileSessionActive) return;

            // Start a timer for a specific profiling event
            timers[systemInput] = timeStart;

            //callCounts[systemInput]++;
            //std::cout << "Start timer\n";
        }

        void StopTimer(DebugSystems systemInput, uint64_t timeEnd) {
            if (!profileSessionActive) return;

            // Stop the timer and record the duration
            uint64_t startTime = timers[systemInput];
            uint64_t duration = timeEnd - startTime;

            // Record the profiling event
            ProfileResult result{};
            result.name = systemInput;
            result.duration = static_cast<float>(duration) / 1000000.f;

            //result.percentage = (duration / totalFrameTime) * 100;
            //result.callCount = callCounts[timerName];
            results.push_back(result);

            //std::cout << "Stop timer: " << timerName << std::endl;
            //std::cout << "Stop timer: " << timerName << " (Call Count: " << callCounts[timerName] << ")" << std::endl;
        }

        void BeginFrame() {
            if (!profileSessionActive) return;

            // Start tracking frame time
            frameStartTime = std::chrono::high_resolution_clock::now();

            //std::cout << "Begin frame" << std::endl;
        }

        void EndFrame() {
            if (!profileSessionActive) return;

            // Calculate the total frame time
            auto frameEndTime = std::chrono::high_resolution_clock::now();
            totalFrameTime = std::chrono::duration<float>(frameEndTime - frameStartTime).count();

            //std::cout << "End frame" << std::endl;
        }

        ProfileResult GetResult(DebugSystems systemInput) {
			// Return the result of a specific profiling event
            for (auto& result : results) {
                if (result.name == systemInput) {
					return result;
				}
			}
			return ProfileResult();
		}

    private:
        std::vector<ProfileResult> results;
        std::string currentSessionName;
        bool profileSessionActive = false;
        std::unordered_map<DebugSystems , uint64_t> timers;
        //std::unordered_map<std::string, int> callCounts; // Keep track of callCount globally
        float totalFrameTime = 0.0f;
        std::chrono::time_point<std::chrono::high_resolution_clock> frameStartTime;
    };

} // namespace debugprofile
