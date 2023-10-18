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
#include <Windows.h>
#include <chrono>
//#include "Movement.h"
//#include "Message.h"
//#include "Input.h"
//#include "Serialization.h"
//#include "File.h"
//#include "ECS.h"
//#include "EntityFactory.h"
//#include "Components.h"
//#include "VMath.h"
//#include "Graphics.h"
//#include "debugdiagnostic.h"
//#include "DebugProfile.h"
//#include "Global.h"
//#include <random>
//#include "physics.h"
//#include "Font.h"
//#include "MultiThreading.h"
// #include "physics.h"
//#include "GUIManager.h"
// #include "Font.h"
// #include "MultiThreading.h"
// #include "Scripting.h"
//#include "FileWatcher.h"
//#include "Animator.h"

//using Vec2 = vmath::Vector2;
//
//#if ENABLE_DEBUG_PROFILE
//
//	DebugProfiling debugSysProfile;
//#endif //
//
//// Stores the list of systems from the ECS that will be used to cycle through
//std::vector<std::pair<std::shared_ptr<System>, std::string>> systemList;

// Instantiator for the Engine Core
EngineCore::EngineCore() : m_initialTime{ static_cast<uint64_t>(std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()).time_since_epoch().count()) }, m_previousTime{ 0 }, gameActive{ true } {
	// empty by design
}



////////////////////////////////////////////////
//    _____    _____    __    __    ______    //
//   |__ __|  |__ __|  |  \  /  |  |  ____|   //
//     | |      | |    |   \/   |  |  |___    //
//     | |      | |    | |\  /| |  |  ____|   //
//     | |     _| |_   | | \/ | |  |  |___    //
//     |_|    |_____|  |_|    |_|  |______|   //
//                                            //
////////////////////////////////////////////////
	
// Returns the time *SINCE START OF GAME* in MICROSECONDS
uint64_t GetTime() {
	return (std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()).time_since_epoch().count()) - EngineCore::engineCore().m_initialTime;
}
