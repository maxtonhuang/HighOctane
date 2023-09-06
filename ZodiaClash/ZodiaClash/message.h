#pragma once

#include <Windows.h>
#include <list>


namespace Architecture {

	
    
	// Define message types
    enum class MessageType { Collision, Input, GameEvent, KeyDown, KeyUp, MouseClick, WindowResize, CustomEvent, Quit };


    // Define a message structure
    struct Message {
        MessageType type;
        int senderEntityID;
        int recipientEntityID;
        // Additional message data
    };

	void ConvertWindowsEventToMessage(unsigned int uMsg);

	




    






	/*

	enum MessageIdType {
		None,
		Quit,
		Collide,
		ToggleDebugInfo,
		CharacterKey,
		MouseButton,
		MouseMove,
		FileDrop
	};

	class Message {
	public:
		Message(MessageIdType id) : MessageId(id) {};
		MessageIdType MessageId;
		virtual ~Message() {};
	};

	*/
}

