
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
#include "DebugProfile.h"

using Vec2 = vmath::Vector2;

float g_dt;


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

		debugprofile::DebugSystems debug_p;
		
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
		ecs.RegisterComponent<Circle>();
		ecs.RegisterComponent<AABB>();
		ecs.RegisterComponent<Animation>();
		ecs.RegisterComponent<Model>();


		std::shared_ptr<MovementSystem> movementSystem = ecs.RegisterSystem<MovementSystem>();
		systemList.emplace_back(movementSystem);
		std::shared_ptr<PhysicsSystem> physicsSystem = ecs.RegisterSystem<PhysicsSystem>();
		systemList.emplace_back(physicsSystem);
		std::shared_ptr<ModelSystem> modelSystem = ecs.RegisterSystem<ModelSystem>();
		systemList.emplace_back(modelSystem);
		std::shared_ptr<GraphicsSystem> graphicsSystem = ecs.RegisterSystem<GraphicsSystem>();
		systemList.emplace_back(graphicsSystem);

		//graphicsSystem->Initialize(GRAPHICS::defaultWidth, GRAPHICS::defaultHeight);

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
			signature.set(ecs.GetComponentType<Circle>());
			signature.set(ecs.GetComponentType<AABB>());
			signature.set(ecs.GetComponentType<Animation>());

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
			signature.set(ecs.GetComponentType<MainCharacter>());
			signature.set(ecs.GetComponentType<Circle>());
			signature.set(ecs.GetComponentType<AABB>());

			ecs.SetSystemSignature<GraphicsSystem>(signature);
		}

		graphics.Initialize(GRAPHICS::defaultWidth, GRAPHICS::defaultHeight);
		LoadMasterModel();

		Serializer::SerializeCSV("../Assets/CSV/ZodiaClashCharacters.csv");
	
		mail.RegisterMailbox(ADDRESS::MOVEMENT);
		mail.RegisterMailbox(ADDRESS::INPUT);

		LoadModels(1, true);

		//LoadModels(MAX_MODELS);

		debugprofile::DebugProfiling debugSysProfile;
		
		

		////////// GAME LOOP //////////

		// update time calculations
		m_previousTime = GetTime();

		while (gameActive) {

			uint64_t l_currentTime = GetTime();
			g_dt = (l_currentTime - m_previousTime) / 1'000'000.f; // g_dt is in microseconds

			// Testing this thing first
			for (std::shared_ptr<System>& sys : systemList) {
				std::cout << "Duration: " << debugSysProfile.GetResult(sys).duration << ", Percentage: " << debugSysProfile.GetResult(sys).percentage << std::endl;

			}

			debugSysProfile.clear();
			m_previousTime = l_currentTime;

			mail.SendMails(); // 1

			glfwPollEvents(); //TEMP, WILL PUT IN INPUT SYSTEM

			mail.SendMails(); // 2

			InputManager::KeyCheck();

			mail.SendMails(); // 3

			//movementSystem->Update();

			///////////// MOVEMENT /////////////


			//debugSysProfile.StartTimer(debugprofile::DebugSystems::Physics, GetTime());
			///////////// PHYSICS /////////////

			for (std::shared_ptr<System> & sys : systemList) {
				//debug_p.
				debugSysProfile.StartTimer(sys, GetTime());
				sys->Update();
				debugSysProfile.StopTimer(sys, GetTime());

			}

			//physicsSystem->Update();
			
			///////////// PHYSICS /////////////
			//debugSysProfile.StopTimer(debugprofile::DebugSystems::Physics, GetTime());

			//debugSysProfile.EndSession();

			//UpdateModel();
			
			//graphics.Draw();


			//debugSysProfile.StartTimer(debugprofile::DebugSystems::Graphics, GetTime());
			///////////// GRAPHICS /////////////

			//graphics.Update(); // Put into ECS to update and draw Entities <<<--------
			//graphics.Draw();
			if (graphics.WindowClosed()) {
				gameActive = false;
			}

			///////////// GRAPHICS /////////////
			//debugSysProfile.StopTimer(debugprofile::DebugSystems::Graphics, GetTime());

			//Performance();
			//gui.Update(graphics.window);


			//DebugPrint("Total Frame Time: %f milliseconds", g_dt * 1'000.f);

		}
	}

	uint64_t EngineCore::GetTime() {
		return (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()).time_since_epoch().count()) - m_initialTime;
	}

