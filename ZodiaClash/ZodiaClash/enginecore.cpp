
#include "EngineCore.h"
#include "Movement.h"
#include "Message.h"
#include "Input.h"
#include <Windows.h>

float dt;

namespace Architecture {

	EngineCore* CORE;

	EngineCore::EngineCore() {
		previousTime = 0;
		gameActive = true;
		CORE = this;

		Initialize();

		//AddSystem(physics);

		mail.RegisterMailbox(ADDRESS::MOVEMENT);
		mail.RegisterMailbox(ADDRESS::INPUT);

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

			dt = (currentTime - previousTime) / 1000.0;

			previousTime = currentTime;

			mail.SendMails();

			glfwPollEvents(); //TEMP, WILL PUT IN INPUT SYSTEM
			InputManager::KeyCheck();

			UpdateModel();

			// ECS should update movement of objects here ?

			for (size_t i = 0; i < Systems.size(); ++i) {
				Systems[i]->Update(dt);
			}
			graphics.Update(dt);
			graphics.Draw();
			if (graphics.WindowClosed()) {
				gameActive = false;
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