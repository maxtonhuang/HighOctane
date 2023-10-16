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
*	@file		Message.h
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
*	@brief		Messaging System for all systems to communicate with each other
*
*	This file contains the Enums used for the Postcards (messages) information.
*	It also contains the Struct for a Postcard (message) and also the Class for
*	the Mail system itself.
*
******************************************************************************/

#pragma once

#include <Windows.h>
#include <vector>
#include <unordered_map>
#include <string>

const size_t MAILBOX_RESERVE_CAP = 4092;

enum class TYPE {

	// feel free to add more message types as required
	COLLISION,
	INPUT,
	GAME_EVENT,
	KEY_TRIGGERED,
	KEY_DOWN,
	KEY_UP,
	KEY_CHECK,
	MOUSE_CLICK,
	MOUSE_MOVE,
	WINDOW_RESIZE,
	CUSTOM_EVENT,
	QUIT
};

// Mailbox addresses
enum class ADDRESS { 
	INPUT,
	MOVEMENT,
	COLLISION,
	PHYSICS,
	GRAPHICS,
	CORE,
	WINDOWS,
	MODEL,
	SCRIPTING,
	ANIMATOR,
	NONE
};

// Follows numbering system of the GLFW Library
enum class INFO {
	KEY_SPACE = 32,
	KEY_0 = 48,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A = 65,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_ESC = 256,
	KEY_ENTER,
	KEY_RIGHT = 262,
	KEY_LEFT,
	KEY_DOWN,
	KEY_UP,
	MOUSE_LEFT = 501,
	MOUSE_RIGHT,
	NONE = 999
};

struct Postcard {

	TYPE		type;
	ADDRESS		from;	// who sent the message
	INFO		info;
	float		posX;	// used for mouse positions
	float		posY;	// used for mouse positions

};

class Mail {

public:

	// Disallow copying to prevent creation of more than one instance
	Mail(const Mail &) = delete;
	Mail & operator=(const Mail &) = delete;

	// Public accessor for the Singleton instance
	static Mail & mail() {
		static Mail postOffice;
		return postOffice;
	}

	void RegisterMailbox(ADDRESS system);

	void SendMails();

	void CreatePostcard(TYPE messageType, ADDRESS from, INFO info, float posX, float posY);

	std::unordered_map<ADDRESS, std::vector<Postcard>> mailbox;

private:

	std::vector<Postcard> mailQueue;
	
	Mail() {
		mailQueue.reserve(MAILBOX_RESERVE_CAP);
	}

};
