#pragma once



namespace Architecture {
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
}