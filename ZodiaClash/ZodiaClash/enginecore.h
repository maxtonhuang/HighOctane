#pragma once

#include "System.h"
#include "graphics.h"
#include <vector>



namespace Architecture {

	class EngineCore {
	public:
		EngineCore();
		~EngineCore();

		//void LoadPreFabs();

		void Run();

		//void Initialize();

		//void GameLoop();
		
		// remove
		//void DestroySystems();
		
		//remove
		//void AddSystem(ISystem* system);

		uint64_t GetTime();

	private:

		// remove
		//std::vector<ISystem*> Systems;

		uint64_t m_previousTime;

		bool gameActive;


	};




}