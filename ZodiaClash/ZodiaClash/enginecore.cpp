
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

#include <random>

using Vec2 = vmath::Vector2;

float g_dt;

	#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
		DebugProfiling debugSysProfile;
	#endif // 

	//EngineCore* CORE;
	ECS ecs;
	std::vector<std::shared_ptr<System>> systemList;
	
	const uint32_t MAX_MODELS = 5000;
	
	EngineCore::EngineCore() {
		m_previousTime = 0;
		gameActive = true;
		//CORE = this;
	}

	void EngineCore::Run() {

		////////// INITIALIZE //////////
		ecs.Init();
		ecs.RegisterComponent<Transform>();
		ecs.RegisterComponent<Color>();
		ecs.RegisterComponent<Texture>();
		ecs.RegisterComponent<Size>();
		ecs.RegisterComponent<Visible>();
		ecs.RegisterComponent<Tex>();
		ecs.RegisterComponent<MainCharacter>();
		ecs.RegisterComponent<Circle>();
		ecs.RegisterComponent<AABB>();
		ecs.RegisterComponent<Animation>();
		ecs.RegisterComponent<Model>();
		ecs.RegisterComponent<Clone>();


		std::shared_ptr<MovementSystem> movementSystem = ecs.RegisterSystem<MovementSystem>();
		systemList.emplace_back(movementSystem);
		std::shared_ptr<PhysicsSystem> physicsSystem = ecs.RegisterSystem<PhysicsSystem>();
		//systemList.emplace_back(physicsSystem);
		std::shared_ptr<ModelSystem> modelSystem = ecs.RegisterSystem<ModelSystem>();
		systemList.emplace_back(modelSystem);
		std::shared_ptr<GraphicsSystem> graphicsSystem = ecs.RegisterSystem<GraphicsSystem>();
		systemList.emplace_back(graphicsSystem);

		//graphicsSystem->Initialize(GRAPHICS::defaultWidth, GRAPHICS::defaultHeight);

		{
			Signature signature;
			//signature.set(ecs.GetComponentType<Transform>());
			//signature.set(ecs.GetComponentType<Color>());
			//signature.set(ecs.GetComponentType<Texture>());
			//signature.set(ecs.GetComponentType<Size>());
			//signature.set(ecs.GetComponentType<Visible>());
			//signature.set(ecs.GetComponentType<Tex>());
			//signature.set(ecs.GetComponentType<MainCharacter>());
			//signature.set(ecs.GetComponentType<Circle>());
			//signature.set(ecs.GetComponentType<AABB>());
			signature.set(ecs.GetComponentType<Animation>());
			//signature.set(ecs.GetComponentType<Model>());

			ecs.SetSystemSignature<ModelSystem>(signature);
		}

		{
			Signature signature;
			//signature.set(ecs.GetComponentType<Transform>());
			//signature.set(ecs.GetComponentType<Visible>());
			signature.set(ecs.GetComponentType<MainCharacter>());
			signature.set(ecs.GetComponentType<Clone>());

			ecs.SetSystemSignature<MovementSystem>(signature);
		}

		{
			Signature signature;
			//signature.set(ecs.GetComponentType<Transform>());
			//signature.set(ecs.GetComponentType<Color>());
			//signature.set(ecs.GetComponentType<Texture>());
			//signature.set(ecs.GetComponentType<Size>());
			//signature.set(ecs.GetComponentType<Visible>());
			//signature.set(ecs.GetComponentType<Tex>());
			//signature.set(ecs.GetComponentType<MainCharacter>());
			signature.set(ecs.GetComponentType<Model>());
			signature.set(ecs.GetComponentType<Clone>());
			//signature.set(ecs.GetComponentType<Circle>());
			//signature.set(ecs.GetComponentType<AABB>());

			ecs.SetSystemSignature<GraphicsSystem>(signature);
		}

		graphics.Initialize(GRAPHICS::defaultWidth, GRAPHICS::defaultHeight);
		LoadMasterModel();

		Serializer::SerializeCSV("../Assets/CSV/ZodiaClashCharacters.csv");

		mail.RegisterMailbox(ADDRESS::MOVEMENT);
		mail.RegisterMailbox(ADDRESS::INPUT);
		mail.RegisterMailbox(ADDRESS::MODEL);

		Entity background = CreateModel();
		ecs.GetComponent<Tex>(background).tex = texList.Add("background.jpeg");
		ecs.GetComponent<Size>(background).width = (float)ecs.GetComponent<Tex>(background).tex->GetWidth();
		ecs.GetComponent<Size>(background).height = (float)ecs.GetComponent<Tex>(background).tex->GetHeight();
		 
		//LoadModels(2500, false);
		std::default_random_engine rng;
		std::uniform_real_distribution<float> rand_width(-GRAPHICS::w, GRAPHICS::w);
		std::uniform_real_distribution<float> rand_height(-GRAPHICS::h, GRAPHICS::h);
		for (int i = 0; i < 2500; ++i) {
			Entity duck = CreateModel();
			ecs.GetComponent<Tex>(duck).tex = texList.Add("duck.png");
			ecs.GetComponent<Animation>(duck).animationType = Animation::ANIMATION_TIME_BASED;
			ecs.GetComponent<Animation>(duck).frameDisplayDuration = 0.3f;
			ecs.GetComponent<Size>(duck).width = (float)ecs.GetComponent<Tex>(duck).tex->GetWidth();
			ecs.GetComponent<Size>(duck).height = (float)ecs.GetComponent<Tex>(duck).tex->GetHeight();
			ecs.GetComponent<Transform>(duck).position = { rand_width(rng), rand_height(rng)};
		}

		LoadModels(1, true);

		graphicsSystem->Initialize();
		//LoadModels(MAX_MODELS);



		////////// GAME LOOP //////////

		// update time calculations
		m_previousTime = GetTime();

		while (gameActive) {
			//debugSysProfile.StartTimer(systemList[0], GetTime());
			uint64_t l_currentTime = GetTime();
			g_dt = (l_currentTime - m_previousTime) / 1'000'000.f; // g_dt is in microseconds

			// Debugging
			
			// Print out only once every 5 seconds
			#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
				static uint64_t l_lastTime = 0;
				//if (l_currentTime - l_lastTime > PRINT_INTERVAL) {
					l_lastTime = l_currentTime;
					for (std::shared_ptr<System>& sys : systemList) {
						std::cout << "Duration: " << debugSysProfile.GetResult(sys).duration << ", Percentage: " << debugSysProfile.GetResult(sys).percentage << "%" << std::endl;
					}
					std::cout << std::endl;

					debugSysProfile.clear();
				//}
			#endif
			m_previousTime = l_currentTime;

			//mail.SendMails(); // 1

			glfwPollEvents(); //TEMP, WILL PUT IN INPUT SYSTEM

			//mail.SendMails(); // 2

			InputManager::KeyCheck();

			mail.SendMails(); // 3

			//movementSystem->Update();

			///////////// MOVEMENT /////////////

			///////////// PHYSICS /////////////

			//graphicsSystem->Update();

			
			for (std::shared_ptr<System> & sys : systemList) {

				#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
					debugSysProfile.StartTimer(sys, GetTime());
				#endif // 
				
				sys->Update();

				#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
					debugSysProfile.StopTimer(sys, GetTime());
				#endif // 
				
				//mail.SendMails();

			}
			
			//physicsSystem->Update();
			
			///////////// PHYSICS /////////////

			//UpdateModel();
			
			//graphics.Draw();


			///////////// GRAPHICS /////////////

			//graphics.Update(); // Put into ECS to update and draw Entities <<<--------
			//graphics.Draw();
			if (graphics.WindowClosed()) {
				gameActive = false;
			}

			///////////// GRAPHICS /////////////
			#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
				Performance(GetTime());
			#endif //
			//gui.Update(graphics.window);

			std::cout << g_dt << std::endl;
			//DebugPrint("Total Frame Time: %f milliseconds", g_dt * 1'000.f);
			//debugSysProfile.StopTimer(systemList[0], GetTime());
		}
	}

	uint64_t EngineCore::GetTime() {
		return (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()).time_since_epoch().count()) - m_initialTime;
	}

