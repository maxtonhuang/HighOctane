#pragma once

#include "System.h"
#include "graphics.h"
#include <vector>


namespace Architecture {

	class EngineCore {
	public:
		EngineCore();
		~EngineCore();



		void GameLoop();

		void DestroySystems();

		//void BroadcastMessage(Message* m);

		void AddSystem(ISystem* system);

		void Initialize();

	private:

		std::vector<ISystem*> Systems;

		unsigned int previousTime;

		bool gameActive;


	};




}