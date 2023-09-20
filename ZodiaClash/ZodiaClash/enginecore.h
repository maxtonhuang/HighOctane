#pragma once

#include "System.h"
#include "graphics.h"
#include <vector>
#include <chrono>




	class EngineCore {
	public:
		EngineCore();
		~EngineCore();


		void Run();


		uint64_t GetTime();

	private:

		const uint64_t m_initialTime = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();
		uint64_t m_previousTime;

		bool gameActive;


	};




