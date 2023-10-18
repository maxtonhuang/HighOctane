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
*	@file		EngineCore.h
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
*	@brief		The Class of the Engine Core
*
*	-
*
******************************************************************************/

#pragma once

#include <chrono>
#include "ECS.h"

class EngineCore {

public:

	// Disallow copying to prevent creation of more than one instance
	EngineCore(const EngineCore &) = delete;
	EngineCore & operator=(const EngineCore &) = delete;

	// Public accessor for the Singleton instance
	static EngineCore & engineCore() {
		static EngineCore ec;
		return ec;
	}

	void set_m_previousTime(uint64_t time) {
		m_previousTime = time;
	}

	uint64_t get_m_previousTime() {
		return m_previousTime;
	}

	void setGameActive(bool active) {
		gameActive = active;
	}

	bool getGameActive() {
		return gameActive;
	}
	
	void Run(bool const & mode);

	friend uint64_t GetTime();

private:

	EngineCore();

	const uint64_t m_initialTime;
		
	uint64_t m_previousTime;

	bool gameActive;

};

uint64_t GetTime();
