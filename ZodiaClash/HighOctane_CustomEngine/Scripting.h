#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/exception.h>
#include <iostream>
#include <filesystem>
#include "debuglog.h"
#include "Message.h"
#include "Components.h"
#include <rttr/registration>
// Define the actions

#pragma once

//void InitMono();

class CsScript {
public:
	CsScript();
	~CsScript();
	void RunScript(Screen *s);
	static void AddInternalCall(const char* functionName, const void* methodPointer);

private:
	MonoDomain* domain{};
	MonoAssembly* assembly{};
	MonoImage* image{};
	MonoClass* scriptClass{};
	MonoObject* scriptInstance{};
	MonoMethod* scriptMethod{};

};

//bool GetKeyDown();
bool GetKeyDown(INFO key);
void GetKeyDownClear();

extern CsScript script;