#pragma once

#include <string>

#include "message.h"

#define UNREFERENCED_PARAMETER(P) (P)



namespace Architecture {
	
	class ISystem {

	public:

		virtual void SendMessage(Message* message) {
			UNREFERENCED_PARAMETER(message);
		};

		virtual void Update(float timeslice) = 0;

		virtual std::string GetName() = 0;

		virtual void Initialize() {};
		
		virtual ~ISystem() {};

	};

}