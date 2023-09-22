#pragma once

#include <Windows.h>
//#include <list>
#include <vector>
#include <unordered_map>
#include <string>



const size_t MAILBOX_RESERVE_CAP = 4092;

    
// Define message types
enum class TYPE {

	// to add more message types

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

enum class ADDRESS {
	INPUT,
	MOVEMENT,
	COLLISION,
	PHYSICS,
	GRAPHICS,
	CORE,
	WINDOWS,
	MODEL,
	NONE
};

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
	TYPE type;
	ADDRESS from;
	INFO info;
	float posX;
	float posY;
};

class Mail {
public:

	Mail() {
		mailQueue.reserve(MAILBOX_RESERVE_CAP);
	}

	void RegisterMailbox(ADDRESS system);

	void SendMails();

	void CreatePostcard(TYPE messageType, ADDRESS from, INFO info, float posX, float posY);

	//void ConvertWindowsEventToPostcard(unsigned int uMsg);

	std::unordered_map<ADDRESS, std::vector<Postcard>> mailbox;

private:

	std::vector<Postcard> mailQueue;

};

extern Mail mail;

