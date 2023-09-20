
#include "EngineCore.h"
#include "Movement.h"
#include "Message.h"
#include "Input.h"
#include "Serialization.h"
#include "File.h"
#include "ECS.h"
#include "EntityFactory.h"
#include "Components.h"
#include "VMath.h"
#include <Windows.h>
#include <chrono>
#include "GUIManager.h"
#include "debugdiagnostic.h"

using Vec2 = vmath::Vector2;

float g_dt;

namespace Architecture {

	EngineCore* CORE;
	ECS ecs;
	std::vector<std::shared_ptr<System>> systemList;
	
	const uint32_t MAX_MODELS = 5000;
	
	EngineCore::EngineCore() {
		m_previousTime = 0;
		gameActive = true;
		CORE = this;
	}

	EngineCore::~EngineCore() {
		// empty by design
	}

	void EngineCore::Run() {

		
		
		////////// INITIALIZE //////////
		ecs.Init();
		ecs.RegisterComponent<Transform>();
		ecs.RegisterComponent<Color>();
		ecs.RegisterComponent<Matrix>();
		ecs.RegisterComponent<Texture>();
		ecs.RegisterComponent<Size>();
		ecs.RegisterComponent<Visible>();
		ecs.RegisterComponent<Tex>();
		ecs.RegisterComponent<MainCharacter>();

		std::shared_ptr<MovementSystem> movementSystem = ecs.RegisterSystem<MovementSystem>();
		systemList.emplace_back(movementSystem);
		std::shared_ptr<PhysicsSystem> physicsSystem = ecs.RegisterSystem<PhysicsSystem>();
		systemList.emplace_back(physicsSystem);
		std::shared_ptr<ModelSystem> modelSystem = ecs.RegisterSystem<ModelSystem>();
		systemList.emplace_back(modelSystem);
		std::shared_ptr<GraphicsManager> graphicsSystem = ecs.RegisterSystem<GraphicsManager>();
		systemList.emplace_back(graphicsSystem);


		{
			Signature signature;
			signature.set(ecs.GetComponentType<Transform>());
			signature.set(ecs.GetComponentType<Color>());
			signature.set(ecs.GetComponentType<Matrix>());
			signature.set(ecs.GetComponentType<Texture>());
			signature.set(ecs.GetComponentType<Size>());
			signature.set(ecs.GetComponentType<Visible>());
			signature.set(ecs.GetComponentType<Tex>());
			signature.set(ecs.GetComponentType<MainCharacter>());

			ecs.SetSystemSignature<ModelSystem>(signature);
		}

		{
			Signature signature;
			signature.set(ecs.GetComponentType<Transform>());
			signature.set(ecs.GetComponentType<Visible>());
			signature.set(ecs.GetComponentType<MainCharacter>());

			ecs.SetSystemSignature<MovementSystem>(signature);
		}

		{
			Signature signature;
			signature.set(ecs.GetComponentType<Transform>());
			signature.set(ecs.GetComponentType<Color>());
			signature.set(ecs.GetComponentType<Matrix>());
			signature.set(ecs.GetComponentType<Texture>());
			signature.set(ecs.GetComponentType<Size>());
			signature.set(ecs.GetComponentType<Visible>());
			signature.set(ecs.GetComponentType<Tex>());
			//signature.set(ecs.GetComponentType<MainCharacter>());

			ecs.SetSystemSignature<GraphicsManager>(signature);
		}

		LoadMasterModel();

		Serializer::SerializeCSV("../Assets/CSV/ZodiaClashCharacters.csv");
	
		mail.RegisterMailbox(ADDRESS::MOVEMENT);
		mail.RegisterMailbox(ADDRESS::INPUT);

		// Create Main Character
		LoadModels(1, true);

		//LoadModels(MAX_MODELS);

		////////// GAME LOOP //////////

		// update time calculations
		m_previousTime = GetTime();

		while (gameActive) {

			uint64_t l_currentTime = GetTime();
			g_dt = (l_currentTime - m_previousTime) / 1'000'000.f; // g_dt is in microseconds
			m_previousTime = l_currentTime;

			mail.SendMails(); // 1

			glfwPollEvents(); //TEMP, WILL PUT IN INPUT SYSTEM

			mail.SendMails(); // 2

			InputManager::KeyCheck();

			mail.SendMails(); // 3

			//movementSystem->Update();
			//PhysicaSystem->Update();

			for (std::shared_ptr<System> & sys : systemList) {
				sys->Update();
			}

			//physicsSystem->Update();

			//UpdateModel();
			
			graphics.Update(); // Put into ECS to update and draw Entities <<<--------
			graphics.Draw();
			if (graphics.WindowClosed()) {
				gameActive = false;
			}

			//Performance();
			//gui.Update(graphics.window);
		}
	}

	uint64_t EngineCore::GetTime() {
		return (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()).time_since_epoch().count()) - m_initialTime;
	}

}
