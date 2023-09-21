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
        timers[systemInput] = startTimeInput;
    }

    void DebugProfiling::StopTimer(std::shared_ptr<System> systemInput, uint64_t endTimeInput) {
        auto startTime = timers[systemInput];
        auto duration = static_cast<float>(endTimeInput - startTime) / 1000.0f;

        ProfileResult result{};
        result.name = systemInput;
        result.duration = duration;

        results.emplace_back(result);
    }

    ProfileResult DebugProfiling::GetResult(std::shared_ptr<System> systemInput) {
        for (auto& result : results) {
            if (result.name == systemInput) {
                result.percentage = (result.duration / (g_dt * 1000.f) * 100.f);
                return result;
            }
    }
    return ProfileResult();
    }

    void DebugProfiling::clear() {
        results.clear();
    }
#endif