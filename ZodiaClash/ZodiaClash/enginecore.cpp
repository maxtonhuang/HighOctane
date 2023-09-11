
#include "enginecore.h"
#include <windows.h>


namespace Architecture {

	EngineCore* CORE;

	EngineCore::EngineCore() {
		previousTime = 0;
		gameActive = true;
		CORE = this;
	}

	EngineCore::~EngineCore() {
		// empty by design
	}

	void EngineCore::Initialize() {
		for (size_t i = 0; i < Systems.size(); ++i) {
			Systems[i]->Initialize();
		}

	}

	void EngineCore::GameLoop() {

		previousTime = timeGetTime();

		while (gameActive) {

			unsigned int currentTime = timeGetTime();

			double dt = (currentTime - previousTime) / 1000.0;

			previousTime = currentTime;

			for (size_t i = 0; i < Systems.size(); ++i) {
				Systems[i]->Update(dt);
			}

		}


	}


	/*void EngineCore::BroadcastMessage(Message* message) {
		if (message->type == MessageType::Quit) {
			gameActive = false;
		}

		for (size_t i = 0; i < Systems.size(); ++i) {
			Systems[i]->SendMessage(message);
		}
	}*/

	void EngineCore::AddSystem(ISystem* system) {
		Systems.emplace_back(system);
	}

	void EngineCore::DestroySystems() {
		for (size_t i = 0; i < Systems.size(); ++i) {
			delete Systems[Systems.size() - i - 1];
		}
	}




}