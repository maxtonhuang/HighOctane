
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

using Vec2 = vmath::Vector2;

float g_dt;

namespace Architecture {

	EngineCore* CORE;
	ECS ecs;
	
	
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
		ecs.RegisterComponent<Vel>();

		std::shared_ptr<PhysicsSystem> physicsSystem = ecs.RegisterSystem<PhysicsSystem>();

		Signature signature;
		signature.set(ecs.GetComponentType<Transform>());
		signature.set(ecs.GetComponentType<Vel>());

		ecs.SetSystemSignature<PhysicsSystem>(signature);

		std::vector<Entity> entities(MAX_ENTITIES);

		for (Entity entity : entities) {
			entity = ecs.CreateEntity();

			ecs.AddComponent(entity, Transform{ Vec2(0.f, 0.f), Vec2(0.f, 0.f), Vec2(0.f, 0.f) });
			ecs.AddComponent(entity, Vel{ Vec2(0.f, 0.f) });
			//ecs.AddComponent(entity, PerformanceProfileComponent{}); // For debugging
			// Add components here
		}

		LoadPreFabs();

		Serializer::SerializeCSV("../Assets/CSV/ZodiaClashCharacters.csv");
	
		mail.RegisterMailbox(ADDRESS::MOVEMENT);
		mail.RegisterMailbox(ADDRESS::INPUT);



		////////// GAME LOOP //////////

		m_previousTime = GetTime();

		while (gameActive) {

			uint64_t l_currentTime = GetTime();
			g_dt = (l_currentTime - m_previousTime) / 1000.f; // dt is in milliseconds
			m_previousTime = l_currentTime;

			mail.SendMails(); // 1

			glfwPollEvents(); //TEMP, WILL PUT IN INPUT SYSTEM

			mail.SendMails(); // 2

			InputManager::KeyCheck();

			mail.SendMails(); // 3

			physicsSystem->Update(g_dt);

			//UpdateModel();

			graphics.Update(g_dt);
			graphics.Draw();
			if (graphics.WindowClosed()) {
				gameActive = false;
			}

		}
	}

	uint64_t EngineCore::GetTime() {
		return std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();
	}


}