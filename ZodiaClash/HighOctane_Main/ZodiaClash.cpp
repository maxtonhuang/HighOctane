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
*	@file		ZodiaClash.cpp
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
*	@brief		Main Entry Point of Program
*
*	This file contains the main entry point of the program created using the
*   standard Visual Studio Desktop Application Wizard. A lot of unnecessary
*   functions that are not applicable for use in our game were removed from the
*   standard template.
*
******************************************************************************/

// ZodiaClash.cpp : Defines the entry point for the application.

////////// MAIN ///////////
#include <Windows.h>
#include "Framework.h"
#include "ZodiaClash.h"
#include "EngineCore.h"
#include "Graphics.h"
#include "GUIManager.h"
#include "DebugLog.h"
#include "DebugDiagnostic.h"
#include "EntityFactory.h"
#include "DebugProfile.h"
#include "Input.h"
#include "EngineCore.h" 
#include "Message.h"
#include "Texture.h"
#include "ECS.h"
#include "Components.h"
#include "physics.h"
#include "AudioManager.h"
#include "Serialization.h"
#include "ScriptEngine.h"
#include "Scripting.h"
#include "ImGuiPerformance.h"
#include "Animator.h"
#include "AssetManager.h"
#include "CharacterStats.h"
#include "Battle.h"
#include "EnemyAction.h"
#include "PlayerAction.h"
#include "UIComponents.h"
#include "Reflections.h"
#include "Events.h"
#include "Layering.h"
#include "Animation.h"
//#include <rttr/type.h>

bool gConsoleInitalized{ false };
constexpr bool GAME_MODE{ false }; // Do not edit this
constexpr bool EDITOR_MODE{ true }; // Do not edit this



////////// Set Loading Mode here. /////////////////////////////////////////////

// Set to GAME_MODE to run in game mode.
// Set to EDITOR_MODE to run in editor mode.
constexpr bool game_mode{ EDITOR_MODE };

///////////////////////////////////////////////////////////////////////////////


/******************************************************************************
*
*	@brief Force Dedicated GPU
*
*	The following 2 snippets of code will force the OS to choose Dedicated GPU.
*
******************************************************************************/

// NVIDIA Optimus dedicated GPU hint.
extern "C" { _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }

// AMD PowerXpress high-performance request.
extern "C" { _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1; }


#if ENABLE_DEBUG_PROFILE
DebugProfiling debugSysProfile;
#endif //

// Stores the list of systems from the ECS that will be used to cycle through
std::vector<std::pair<std::shared_ptr<System>, std::string>> runSystemList;
std::vector<std::pair<std::shared_ptr<System>, std::string>> editSystemList;
std::vector<std::pair<std::shared_ptr<System>, std::string>> systemList;



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{   

	/**************************************************************************
	*	The following code will prevent window from going out of screen of low
	*	resolution screens. Modern monitors that are 1080P or higher should not
	*	have this problem.
	**************************************************************************/
	//SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    LoadConfig();
    nCmdShow = nCmdShow; //unused variable
    hInstance = hInstance; //unused variable
    // Enable run-time memory check for debug builds.
    #if (_DEBUG)
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	
    // To enable the console
    Console();

	/*-----------THIS IS FOR REFLECTION------------*/
	
	// A simple test to see reflection in action
	// Use the macro to declare variables
	//DECLARE(int, test, 3);
	//DECLARE(float, test2, 3.14f);
	//DECLARE(std::string, test3, "Hello");
	//DECLARE(bool, test4, true);

	// Iterate over registered variables and print their values
	for (const auto& var : variablesTEST) {
		std::cout << var.name << " = " << var.read() << std::endl;
	}

	/*-----------THIS IS FOR REFLECTION------------*/

    LOG_INFO("Program started");

    /*--------------FOR DEBUGGING PLEASE DO NOT TOUCH FIRST THANK YOU VERY MUCH--------------------*/
    LOG_SET_LEVEL(debuglog::LOG_LEVEL::Trace);

    LOG_TRACE("This is a test trace message");
    LOG_DEBUG("This is a test debug message");
    LOG_INFO("This is a test info message");
    LOG_WARNING("This is a test warning message");
    LOG_ERROR("This is a test error message");
    LOG_FATAL("This is a test fatal message");
    /*---------------------------------------------------------------------------------------------*/
    // TODO: Place code here.
    //graphics.Initialize(GRAPHICS::defaultWidth, GRAPHICS::defaultHeight);

    LOG_INFO("Graphics started");

    EngineCore::engineCore(); // Instantiate Engine Core

    //////////////////////////////
    ////////// Run Game //////////
    //////////////////////////////

	EngineCore::engineCore().Run(game_mode);

    return 0;
}

 // TESTING FOR CONSOLE MODE FOR SCRIPTING - WEN YUAN
//int main() {
//	//InitMono();
//	LoadConfig();
//	// Enable run-time memory check for debug builds.
//#if (_DEBUG)
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif
//
//	// To enable the console
//	Console();
//	LOG_INFO("Program started");
//
//	/*--------------FOR DEBUGGING PLEASE DO NOT TOUCH FIRST THANK YOU VERY MUCH--------------------*/
//	LOG_SET_LEVEL(debuglog::LOG_LEVEL::Trace);
//
//	LOG_TRACE("This is a test trace message");
//	LOG_DEBUG("This is a test debug message");
//	LOG_INFO("This is a test info message");
//	LOG_WARNING("This is a test warning message");
//	LOG_ERROR("This is a test error message");
//	LOG_FATAL("This is a test fatal message");
//	/*---------------------------------------------------------------------------------------------*/
//	// TODO: Place code here.
//	//graphics.Initialize(GRAPHICS::defaultWidth, GRAPHICS::defaultHeight);
//	audio.Initialize();
//	audio.AddSound("../Assets/Sound/ping.wav");
//	audio.AddSound("../Assets/Sound/bonk.wav");
//	LOG_INFO("Graphics started");
//
//	EngineCore::engineCore(); // Instantiate Engine Core
//
//	//////////////////////////////
//	////////// Run Game //////////
//	//////////////////////////////
//
//	EngineCore::engineCore().Run(game_mode);
//
//	return 0;
//}




void EngineCore::Run(bool const& mode) {

	////////// INITIALIZE //////////
	ScriptEngine::Init(); // Script Engine should be same level as ECS

// Register components to be used in the ECS
	ECS::ecs().Init();
	ECS::ecs().RegisterComponent<Transform>();
	ECS::ecs().RegisterComponent<Color>();
	ECS::ecs().RegisterComponent<Size>();
	ECS::ecs().RegisterComponent<Visible>();
	ECS::ecs().RegisterComponent<Tex>();
	ECS::ecs().RegisterComponent<MainCharacter>();
	//ECS::ecs().RegisterComponent<Animator>();
	ECS::ecs().RegisterComponent<Model>();
	ECS::ecs().RegisterComponent<Master>();
	ECS::ecs().RegisterComponent<Clone>();
	ECS::ecs().RegisterComponent<Collider>();
	ECS::ecs().RegisterComponent<Name>();
	ECS::ecs().RegisterComponent<Tag>();
	ECS::ecs().RegisterComponent<Movable>();
	ECS::ecs().RegisterComponent<CharacterStats>();
	ECS::ecs().RegisterComponent<Script>();
	ECS::ecs().RegisterComponent<AnimationSet>();
	//ECS::ecs().RegisterComponent<PlayerAction>();
	//ECS::ecs().RegisterComponent<EnemyAction>();
	ECS::ecs().RegisterComponent<TextLabel>();
	ECS::ecs().RegisterComponent<Button>();
	ECS::ecs().RegisterComponent<HealthBar>();
	ECS::ecs().RegisterComponent<HealthRemaining>();
	ECS::ecs().RegisterComponent<SkillPointHUD>();
	ECS::ecs().RegisterComponent<SkillPoint>();
	ECS::ecs().RegisterComponent<Parent>();
	ECS::ecs().RegisterComponent<Child>();

	// Register systems to be used in the ECS
	std::shared_ptr<MovementSystem> movementSystem = ECS::ecs().RegisterSystem<MovementSystem>();
	runSystemList.emplace_back(movementSystem, "Movement System");
	systemList.emplace_back(movementSystem, "Movement System");

	std::shared_ptr<PhysicsSystem> physicsSystem = ECS::ecs().RegisterSystem<PhysicsSystem>();
	runSystemList.emplace_back(physicsSystem, "Physics System");
	systemList.emplace_back(physicsSystem, "Physics System");

	std::shared_ptr<CollisionSystem> collisionSystem = ECS::ecs().RegisterSystem<CollisionSystem>();
	runSystemList.emplace_back(collisionSystem, "Collison System");
	systemList.emplace_back(collisionSystem, "Collison System");	

	//std::shared_ptr<AnimatorSystem> animatorSystem = ECS::ecs().RegisterSystem<AnimatorSystem>();
	//runSystemList.emplace_back(animatorSystem, "Animator System");
	//systemList.emplace_back(animatorSystem, "Animator System");

	std::shared_ptr<AnimationSystem> animationSystem = ECS::ecs().RegisterSystem<AnimationSystem>();
	runSystemList.emplace_back(animationSystem, "Animation System");
	systemList.emplace_back(animationSystem, "Animation System");

	std::shared_ptr<ScriptSystem> scriptingSystem = ECS::ecs().RegisterSystem<ScriptSystem>();
	runSystemList.emplace_back(scriptingSystem, "Scripting System");
	systemList.emplace_back(scriptingSystem, "Scripting System");

	std::shared_ptr<BattleSystem> battleSystem = ECS::ecs().RegisterSystem<BattleSystem>();
	runSystemList.emplace_back(battleSystem, "Battle System");
	systemList.emplace_back(battleSystem, "Battle System");

	// Not in System List, will only be called when needed
	std::shared_ptr<SerializationSystem> serializationSystem = ECS::ecs().RegisterSystem<SerializationSystem>();
	systemList.emplace_back(serializationSystem, "Serialization System");
	s_ptr = serializationSystem;

	std::shared_ptr<UIButtonSystem> uiButtonSystem = ECS::ecs().RegisterSystem<UIButtonSystem>();
	runSystemList.emplace_back(uiButtonSystem, "UI Button System");
	editSystemList.emplace_back(uiButtonSystem, "UI Button System");
	systemList.emplace_back(uiButtonSystem, "UI Button System");

	std::shared_ptr<UIHealthBarSystem> uiHealthBarSystem = ECS::ecs().RegisterSystem<UIHealthBarSystem>();
	runSystemList.emplace_back(uiHealthBarSystem, "UI Health Bar System");
	editSystemList.emplace_back(uiHealthBarSystem, "UI Health Bar System");
	systemList.emplace_back(uiHealthBarSystem, "UI Health Bar System");

	std::shared_ptr<UISkillPointSystem> uiSkillPointSystem = ECS::ecs().RegisterSystem<UISkillPointSystem>();
	runSystemList.emplace_back(uiSkillPointSystem, "UI Skill Point System");
	editSystemList.emplace_back(uiSkillPointSystem, "UI Skill Point System");
	systemList.emplace_back(uiSkillPointSystem, "UI Skill Point System");

	std::shared_ptr<UITextLabelSystem> uiTextLabelSystem = ECS::ecs().RegisterSystem<UITextLabelSystem>();
	runSystemList.emplace_back(uiTextLabelSystem, "UI Text Label System");
	editSystemList.emplace_back(uiTextLabelSystem, "UI Text Label System");
	systemList.emplace_back(uiTextLabelSystem, "UI Text Label System");

	std::shared_ptr<EditingSystem> editingSystem = ECS::ecs().RegisterSystem<EditingSystem>();
	editSystemList.emplace_back(editingSystem, "Editing System");
	systemList.emplace_back(editingSystem, "Editing System");
	edit_ptr = editingSystem;

	std::shared_ptr<ParentSystem> parentSystem = ECS::ecs().RegisterSystem<ParentSystem>();
	runSystemList.emplace_back(parentSystem, "Parent System");
	editSystemList.emplace_back(parentSystem, "Parent System");
	systemList.emplace_back(parentSystem, "Parent System");

	std::shared_ptr<ChildSystem> childSystem = ECS::ecs().RegisterSystem<ChildSystem>();
	runSystemList.emplace_back(childSystem, "Child System");
	editSystemList.emplace_back(childSystem, "Child System");
	systemList.emplace_back(childSystem, "Child System");

	std::shared_ptr<ModelSystem> modelSystem = ECS::ecs().RegisterSystem<ModelSystem>();
	runSystemList.emplace_back(modelSystem, "Model System");
	systemList.emplace_back(modelSystem, "Model System");

	std::shared_ptr<AudioSystem> audioSystem = ECS::ecs().RegisterSystem<AudioSystem>();
	runSystemList.emplace_back(audioSystem, "Audio System");
	editSystemList.emplace_back(audioSystem, "Audio System");
	systemList.emplace_back(audioSystem, "Audio System");

	std::shared_ptr<GraphicsSystem> graphicsSystem = ECS::ecs().RegisterSystem<GraphicsSystem>();
	runSystemList.emplace_back(graphicsSystem, "Graphics System");
	editSystemList.emplace_back(graphicsSystem, "Graphics System");
	systemList.emplace_back(graphicsSystem, "Graphics System");

	// Set Entity's Component combination signatures for each System 
	{
		Signature signature;
		signature.set(ECS::ecs().GetComponentType<Collider>());
		signature.set(ECS::ecs().GetComponentType<Transform>());
		signature.set(ECS::ecs().GetComponentType<Clone>());

		ECS::ecs().SetSystemSignature<PhysicsSystem>(signature);

	}

	{
		Signature signature;
		signature.set(ECS::ecs().GetComponentType<Transform>());
		signature.set(ECS::ecs().GetComponentType<Collider>());
		signature.set(ECS::ecs().GetComponentType<Clone>());

		ECS::ecs().SetSystemSignature<CollisionSystem>(signature);
	}

	{
		Signature signature;
		signature.set(ECS::ecs().GetComponentType<MainCharacter>());
		signature.set(ECS::ecs().GetComponentType<Clone>());
		signature.set(ECS::ecs().GetComponentType<Model>());
		//signature.set(ECS::ecs().GetComponentType<Animator>());
		signature.set(ECS::ecs().GetComponentType<Tex>());
		signature.set(ECS::ecs().GetComponentType<Size>());

		ECS::ecs().SetSystemSignature<MovementSystem>(signature);
	}

	//{
	//	Signature signature;
	//	signature.set(ECS::ecs().GetComponentType<Size>());
	//	signature.set(ECS::ecs().GetComponentType<Tex>());
	//	signature.set(ECS::ecs().GetComponentType<Animator>());
	//	signature.set(ECS::ecs().GetComponentType<Model>());
	//	signature.set(ECS::ecs().GetComponentType<Clone>());

	//	ECS::ecs().SetSystemSignature<AnimatorSystem>(signature);
	//}

	{
		Signature signature;
		signature.set(ECS::ecs().GetComponentType<AnimationSet>());
		signature.set(ECS::ecs().GetComponentType<Clone>());

		ECS::ecs().SetSystemSignature<AnimationSystem>(signature);
	}

	{
		Signature signature;
		signature.set(ECS::ecs().GetComponentType<Parent>());
		signature.set(ECS::ecs().GetComponentType<Clone>());

		ECS::ecs().SetSystemSignature<ParentSystem>(signature);
	}

	{
		Signature signature;
		signature.set(ECS::ecs().GetComponentType<Child>());
		signature.set(ECS::ecs().GetComponentType<Transform>());
		signature.set(ECS::ecs().GetComponentType<Clone>());

		ECS::ecs().SetSystemSignature<ChildSystem>(signature);
	}

	{
		Signature signature;
		//signature.set(ECS::ecs().GetComponentType<Size>());
		//signature.set(ECS::ecs().GetComponentType<Tex>());
		//signature.set(ECS::ecs().GetComponentType<Animator>());
		//signature.set(ECS::ecs().GetComponentType<Model>());
		//signature.set(ECS::ecs().GetComponentType<Clone>());

		ECS::ecs().SetSystemSignature<ModelSystem>(signature);
	}

	{
		Signature signature;
		ECS::ecs().SetSystemSignature<AudioSystem>(signature);
	}

	{
		Signature signature;
		signature.set(ECS::ecs().GetComponentType<Transform>());
		signature.set(ECS::ecs().GetComponentType<Size>());
		//signature.set(ECS::ecs().GetComponentType<Tex>());
		signature.set(ECS::ecs().GetComponentType<Model>());
		signature.set(ECS::ecs().GetComponentType<Clone>());
		//signature.set(ECS::ecs().GetComponentType<Animator>());

		ECS::ecs().SetSystemSignature<GraphicsSystem>(signature);
	}

	{
		Signature signature;
		signature.set(ECS::ecs().GetComponentType<Script>());
		signature.set(ECS::ecs().GetComponentType<Name>());
		signature.set(ECS::ecs().GetComponentType<Clone>());

		ECS::ecs().SetSystemSignature<ScriptSystem>(signature);
	}

	{
		Signature signature;
		ECS::ecs().SetSystemSignature<SerializationSystem>(signature);
	}

	{
		Signature signature;
		signature.set(ECS::ecs().GetComponentType<Name>());
		signature.set(ECS::ecs().GetComponentType<Transform>());
		//signature.set(ECS::ecs().GetComponentType<Model>());
		signature.set(ECS::ecs().GetComponentType<Size>());
		signature.set(ECS::ecs().GetComponentType<Clone>());
		signature.set(ECS::ecs().GetComponentType<Movable>());
		ECS::ecs().SetSystemSignature<EditingSystem>(signature);
	}

	{
		Signature signature;
		//signature.set(ECS::ecs().GetComponentType<Tex>());
		//signature.set(ECS::ecs().GetComponentType<Animator>());
		//signature.set(ECS::ecs().GetComponentType<Model>());
		signature.set(ECS::ecs().GetComponentType<Clone>());
		signature.set(ECS::ecs().GetComponentType<CharacterStats>());
		//signature.set(ECS::ecs().GetComponentType<Tag>());
		ECS::ecs().SetSystemSignature<BattleSystem>(signature);
	}

	{
		Signature signature;
		signature.set(ECS::ecs().GetComponentType<Transform>());
		signature.set(ECS::ecs().GetComponentType<Size>());
		//signature.set(ECS::ecs().GetComponentType<Tex>());
		signature.set(ECS::ecs().GetComponentType<Model>());
		signature.set(ECS::ecs().GetComponentType<Clone>());
		signature.set(ECS::ecs().GetComponentType<Name>());
		signature.set(ECS::ecs().GetComponentType<TextLabel>());

		ECS::ecs().SetSystemSignature<UITextLabelSystem>(signature);
	}

	{
		Signature signature;
		signature.set(ECS::ecs().GetComponentType<Transform>());
		signature.set(ECS::ecs().GetComponentType<Size>());
		//signature.set(ECS::ecs().GetComponentType<Tex>());
		signature.set(ECS::ecs().GetComponentType<Model>());
		signature.set(ECS::ecs().GetComponentType<Clone>());
		signature.set(ECS::ecs().GetComponentType<Name>());
		signature.set(ECS::ecs().GetComponentType<TextLabel>());
		signature.set(ECS::ecs().GetComponentType<Button>());

		ECS::ecs().SetSystemSignature<UIButtonSystem>(signature);
	}

	{
		Signature signature;
		signature.set(ECS::ecs().GetComponentType<Transform>());
		signature.set(ECS::ecs().GetComponentType<Size>());
		//signature.set(ECS::ecs().GetComponentType<Tex>());
		signature.set(ECS::ecs().GetComponentType<Model>());
		signature.set(ECS::ecs().GetComponentType<Clone>());
		signature.set(ECS::ecs().GetComponentType<Name>());
		signature.set(ECS::ecs().GetComponentType<CharacterStats>());
		signature.set(ECS::ecs().GetComponentType<HealthBar>());
		signature.set(ECS::ecs().GetComponentType<Parent>());

		ECS::ecs().SetSystemSignature<UIHealthBarSystem>(signature);
	}

	{
		Signature signature;
		signature.set(ECS::ecs().GetComponentType<Transform>());
		signature.set(ECS::ecs().GetComponentType<Size>());
		signature.set(ECS::ecs().GetComponentType<Model>());
		signature.set(ECS::ecs().GetComponentType<Clone>());
		signature.set(ECS::ecs().GetComponentType<Name>());
		signature.set(ECS::ecs().GetComponentType<TextLabel>());
		signature.set(ECS::ecs().GetComponentType<SkillPointHUD>());
		signature.set(ECS::ecs().GetComponentType<Parent>());

		ECS::ecs().SetSystemSignature<UISkillPointSystem>(signature);
	}

	//////////////////////////////////////////////////////
	//////////                                  //////////
	//////////   Initialize all other systems   //////////
	//////////                                  //////////
	//////////////////////////////////////////////////////

	CreateNewLayer();

	edit_mode = mode;

	physics::PHYSICS = new physics::PhysicsManager{ ECS::ecs(),graphics };

	graphics.Initialize(GRAPHICS::defaultWidth, GRAPHICS::defaultHeight);


	assetmanager.Initialize();

	events.InitialiseFunctions();

	GUIManager guiManager;
	if (game_mode) {

		// LOAD IMGUI HERE !!!!!

		guiManager.Init();
		inEditing = game_mode;

	}

	// Mailbox Registrations
	Mail::mail().RegisterMailbox(ADDRESS::MOVEMENT);
	Mail::mail().RegisterMailbox(ADDRESS::INPUT);
	Mail::mail().RegisterMailbox(ADDRESS::SCRIPTING);
	Mail::mail().RegisterMailbox(ADDRESS::ANIMATOR);
	Mail::mail().RegisterMailbox(ADDRESS::EDITING);
	Mail::mail().RegisterMailbox(ADDRESS::UICOMPONENT);



	///////////////////////////////////
	//////////               //////////
	//////////   GAME LOOP   //////////
	//////////               //////////
	///////////////////////////////////

	// update time calculations
	EngineCore::engineCore().set_m_previousTime(GetTime());

	// Script engine that creates a new domain and loads the assembly (singleton)

	//SaveEntityToJson("testEntity.json", tmp);

	// Game loop will contain the others
	while (EngineCore::engineCore().getGameActive()) {

		if (initLevel) {
			graphicsSystem->Initialize();
			scriptingSystem->Initialize();
			battleSystem->Initialize();
			animationSystem->Initialize();
			initLevel = false;
			newScene = false;
		}

		uint64_t l_currentTime = GetTime();
		g_dt = static_cast<float>(l_currentTime - EngineCore::engineCore().get_m_previousTime()) / 1'000'000.f; // g_dt is in seconds after dividing by 1,000,000
		EngineCore::engineCore().set_m_previousTime(l_currentTime);


		glfwPollEvents(); //TEMP, WILL PUT IN INPUT SYSTEM

		// Activates the Input Manager to check for Inputs
		// and inform all relavant systems

		InputManager::KeyCheck();
		InputManager::MouseCheck();
		Mail::mail().SendMails();

		// Call each system in the System List
		accumulatedTime += g_dt;
		if (accumulatedTime > MAX_ACCUMULATED_TIME) {
			accumulatedTime = MAX_ACCUMULATED_TIME; // Prevents "spiral of death".
		}
		while (accumulatedTime >= FIXED_DT) {

			for (std::pair<std::shared_ptr<System>, std::string>& sys : (edit_mode ? editSystemList : runSystemList)) {

				#if ENABLE_DEBUG_PROFILE
				debugSysProfile.ResetTimer(sys.second);
				debugSysProfile.StartTimer(sys.second, GetTime()); // Get the string of the system
				//std::cout << sys.second << std::endl;
				#endif
				sys.first->Update();

				#if ENABLE_DEBUG_PROFILE
				debugSysProfile.StopTimer(sys.second, GetTime()); // Get the string of the system
				#endif

			}
			Mail::mail().ClearMails();
			accumulatedTime -= FIXED_DT;
		}

		for (std::pair<std::shared_ptr<System>, std::string>& sys : (edit_mode ? editSystemList : runSystemList)) {

			#if ENABLE_DEBUG_PROFILE
			debugSysProfile.StartTimer(sys.second, GetTime()); // Get the string of the system
			#endif
			sys.first->Draw();

			#if ENABLE_DEBUG_PROFILE
			debugSysProfile.StopTimer(sys.second, GetTime()); // Get the string of the system
			#endif

		}

		if (game_mode) {
			debugSysProfile.StartTimer("Level Editor", GetTime());
			guiManager.Update();
			debugSysProfile.ResetTimer("Level Editor");
			debugSysProfile.StopTimer("Level Editor", GetTime());
		}

		if (graphics.WindowClosed()) {
			EngineCore::engineCore().setGameActive(false);
		}
		graphics.EndDraw();

		// ImGUI button to activate serialization function
		if (button_clicked) {
			button_clicked = false;
			debugSysProfile.StartTimer("Serialization System", GetTime());
			serializationSystem->Update();
			debugSysProfile.StartTimer("Serialization System", GetTime());
		}

	}

	///////////////////////////////////////
	//////////                   //////////
	//////////   GAME LOOP END   //////////
	//////////                   //////////
	///////////////////////////////////////



	// Quit the script engine, maybe move it somewhere else in the future idk or no need
	ScriptEngine::Shutdown();

	delete physics::PHYSICS; //maybe put this somewhere else



	//////////////////////////////
	////////// End Game //////////
	//////////////////////////////



}