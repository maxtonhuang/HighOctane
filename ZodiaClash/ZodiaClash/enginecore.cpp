/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		EngineCore.cpp
*
*	@author		Maxton Huang Xinghua
*
*	@email		m.huang\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		22 September 2023
*
* *****************************************************************************
*
*	@brief		The Main starting point of the game engine
*
*	This file contains all the required initializations of all the different
*	systems of the game engine, and thereafter runs the main game loop until
*	the user chooses to quit the program.
*
******************************************************************************/

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
#include "Graphics.h"
#include "GUIManager.h"
#include "debugdiagnostic.h"
#include "DebugProfile.h"
#include "Global.h"
#include <random>
#include <Windows.h>
#include <chrono>
#include "physics.h"
#include "GUIManager.h"


using Vec2 = vmath::Vector2;

std::vector<float> testingVec;
#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
	DebugProfiling debugSysProfile;
#endif // 

std::vector<std::pair<std::shared_ptr<System>, std::string>> systemList;
	
//const uint32_t MAX_MODELS = 50'000;
	
EngineCore::EngineCore() {
	m_previousTime = 0;
	gameActive = true;
}

void EngineCore::Run() {

	////////// INITIALIZE //////////
	ECS::ecs().Init();
	ECS::ecs().RegisterComponent<Transform>();
	ECS::ecs().RegisterComponent<Color>();
	ECS::ecs().RegisterComponent<Texture>();
	ECS::ecs().RegisterComponent<Size>();
	ECS::ecs().RegisterComponent<Visible>();
	ECS::ecs().RegisterComponent<Tex>();
	ECS::ecs().RegisterComponent<MainCharacter>();
	ECS::ecs().RegisterComponent<Circle>();
	ECS::ecs().RegisterComponent<AABB>();
	ECS::ecs().RegisterComponent<Animation>();
	ECS::ecs().RegisterComponent<Model>();
	ECS::ecs().RegisterComponent<Clone>();
	ECS::ecs().RegisterComponent<Body>();
	ECS::ecs().RegisterComponent<Collider>();


	std::shared_ptr<MovementSystem> movementSystem = ECS::ecs().RegisterSystem<MovementSystem>();
	systemList.emplace_back(movementSystem, "Movement System");
	//std::shared_ptr<PhysicsSystem> physicsSystem = ECS::ecs().RegisterSystem<PhysicsSystem>();
	//systemList.emplace_back(physicsSystem);
	std::shared_ptr<ModelSystem> modelSystem = ECS::ecs().RegisterSystem<ModelSystem>();
	systemList.emplace_back(modelSystem, "Model System");
	std::shared_ptr<GraphicsSystem> graphicsSystem = ECS::ecs().RegisterSystem<GraphicsSystem>();
	systemList.emplace_back(graphicsSystem, "Graphics System");

	{
		Signature signature;
		//signature.set(ECS::ecs().GetComponentType<Transform>());
		//signature.set(ECS::ecs().GetComponentType<Color>());
		//signature.set(ECS::ecs().GetComponentType<Texture>());
		//signature.set(ECS::ecs().GetComponentType<Size>());
		//signature.set(ECS::ecs().GetComponentType<Visible>());
		//signature.set(ECS::ecs().GetComponentType<Tex>());
		//signature.set(ECS::ecs().GetComponentType<MainCharacter>());
		//signature.set(ECS::ecs().GetComponentType<Circle>());
		//signature.set(ECS::ecs().GetComponentType<AABB>());
		signature.set(ECS::ecs().GetComponentType<Animation>());
		//signature.set(ECS::ecs().GetComponentType<Model>());

		ECS::ecs().SetSystemSignature<ModelSystem>(signature);
	}

	{
		Signature signature;
		//signature.set(ECS::ecs().GetComponentType<Transform>());
		//signature.set(ECS::ecs().GetComponentType<Visible>());
		signature.set(ECS::ecs().GetComponentType<MainCharacter>());
		signature.set(ECS::ecs().GetComponentType<Clone>());

		ECS::ecs().SetSystemSignature<MovementSystem>(signature);
	}

	{
		Signature signature;
		//signature.set(ECS::ecs().GetComponentType<Transform>());
		//signature.set(ECS::ecs().GetComponentType<Color>());
		//signature.set(ECS::ecs().GetComponentType<Texture>());
		//signature.set(ECS::ecs().GetComponentType<Size>());
		//signature.set(ECS::ecs().GetComponentType<Visible>());
		//signature.set(ECS::ecs().GetComponentType<Tex>());
		//signature.set(ECS::ecs().GetComponentType<MainCharacter>());
		signature.set(ECS::ecs().GetComponentType<Model>());
		signature.set(ECS::ecs().GetComponentType<Clone>());
		//signature.set(ECS::ecs().GetComponentType<Circle>());
		//signature.set(ECS::ecs().GetComponentType<AABB>());

		ECS::ecs().SetSystemSignature<GraphicsSystem>(signature);
	}

	graphics.Initialize(GRAPHICS::defaultWidth, GRAPHICS::defaultHeight);
	LoadMasterModel();

	Serializer::SerializeCSV("../Assets/CSV/ZodiaClashCharacters.csv");

	Mail::mail().RegisterMailbox(ADDRESS::MOVEMENT);
	Mail::mail().RegisterMailbox(ADDRESS::INPUT);
	Mail::mail().RegisterMailbox(ADDRESS::MODEL);

	Entity background = CreateModel();
	ECS::ecs().GetComponent<Tex>(background).tex = texList.Add("background.jpeg");
	ECS::ecs().GetComponent<Size>(background).width = (float)ECS::ecs().GetComponent<Tex>(background).tex->GetWidth();
	ECS::ecs().GetComponent<Size>(background).height = (float)ECS::ecs().GetComponent<Tex>(background).tex->GetHeight();
		 
	//LoadModels(2500, false);
	std::default_random_engine rng;
	std::uniform_real_distribution<float> rand_width(-GRAPHICS::w, GRAPHICS::w);
	std::uniform_real_distribution<float> rand_height(-GRAPHICS::h, GRAPHICS::h);
	Entity tmp;
	for (int i = 0; i < 0; ++i) {
		Entity duck = CreateModel();
		ECS::ecs().GetComponent<Tex>(duck).texVariants.push_back(texList.Add("duck.png"));
		ECS::ecs().GetComponent<Tex>(duck).texVariants.push_back(texList.Add("duck2.png"));
		ECS::ecs().GetComponent<Tex>(duck).tex = ECS::ecs().GetComponent<Tex>(duck).texVariants[0];
		ECS::ecs().GetComponent<Animation>(duck).animationType = Animation::ANIMATION_EVENT_BASED;
		//ECS::ecs().GetComponent<Animation>(duck).animationType = Animation::ANIMATION_TIME_BASED;
		ECS::ecs().GetComponent<Animation>(duck).frameDisplayDuration = 0.2f;
		ECS::ecs().GetComponent<Size>(duck).width = (float)ECS::ecs().GetComponent<Tex>(duck).tex->GetWidth();
		ECS::ecs().GetComponent<Size>(duck).height = (float)ECS::ecs().GetComponent<Tex>(duck).tex->GetHeight();
		ECS::ecs().GetComponent<Transform>(duck).position = { rand_width(rng), rand_height(rng)};
		tmp = duck;
	}
	//LoadModels(2500, false);
	LoadModels(1, true);
	graphicsSystem->Initialize();
	//LoadModels(MAX_MODELS);


	//////////////////////////////////////////////
	///////////////// GAME LOOP //////////////////
	//////////////////////////////////////////////

	// update time calculations
	m_previousTime = GetTime();

	while (gameActive) {
		//debugSysProfile.StartTimer(systemList[0], GetTime());
		uint64_t l_currentTime = GetTime();
		g_dt = static_cast<float>(l_currentTime - m_previousTime) / 1'000'000.f; // g_dt is in seconds after dividing by 1,000,000
		m_previousTime = l_currentTime;
		
		//DebugPrint("FPS: %.2f", 1.0f / g_dt);
		//std::cout << "FPS: " << 1.0f / g_dt;
			// Debugging	
			//Print out only once every 5 seconds
			#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
				//static uint64_t l_lastTime = 0;
				//if (l_currentTime - l_lastTime > PRINT_INTERVAL) {
				//	l_lastTime = l_currentTime;
					//for (std::pair<std::shared_ptr<System>, std::string> & sys : systemList) {
						//std::cout << "Duration: " << debugSysProfile.GetResult(sys).duration << " millisec, Percentage: " << debugSysProfile.GetResult(sys).percentage << "%" << std::endl;
						//DebugPrint("Durations: %.2f millisec, Percentages: %.2f%%", debugSysProfile.GetDuration(sys.first), debugSysProfile.GetPercentage(sys.first));
						//std::cout << sys.second << " Duration: " << debugSysProfile.GetDuration(sys.first) << " millisec, Percentage: " << debugSysProfile.GetPercentage(sys.first) << "%" << std::endl;
						//DebugPrint("%.2f in millisec", g_dt * 1000);
					//}

					//std::cout << std::endl;
			//}
		#endif
			
		glfwPollEvents(); //TEMP, WILL PUT IN INPUT SYSTEM

		InputManager::KeyCheck();
		Mail::mail().SendMails();

			 
		for (std::pair<std::shared_ptr<System>, std::string> & sys : systemList) {

			#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
					debugSysProfile.StartTimer(sys.first, GetTime()); // change first to second to get string
			#endif
				
			sys.first->Update();

			#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
					debugSysProfile.StopTimer(sys.first, GetTime()); // change first to second to get string
			#endif
				
		}

		if (graphics.WindowClosed()) {
			gameActive = false;
		}

		#if ENABLE_DEBUG_DIAG && ENABLE_DEBUG_PROFILE
			Performance(GetTime());
		#endif

		//debugSysProfile.StopTimer(systemList[0], GetTime());
		}
	}



///////////////////////////////////////////////
//   _____    _____    __    __    ______    //
//  |__ __|  |__ __|  |  \  /  |  |  ____|   //
//    | |      | |    |   \/   |  |  |___    //
//    | |      | |    | |\  /| |  |  ____|   //
//    | |     _| |_   | | \/ | |  |  |___    //
//    |_|    |_____|  |_|    |_|  |______|   //
//	                                         //
///////////////////////////////////////////////
	
// Returns the time *SINCE START OF GAME* in MICROSECONDS
uint64_t EngineCore::GetTime() {
	return (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()).time_since_epoch().count()) - m_initialTime;
}
