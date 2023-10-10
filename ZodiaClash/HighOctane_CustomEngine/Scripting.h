#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/exception.h>
#include <iostream>
#include <filesystem>
#include "debuglog.h"
#include "ImGuiLib.h"

#pragma once

//void InitMono();

class CsScript {
public:
	CsScript();
	~CsScript();

private:
	MonoDomain* domain{};
	MonoAssembly* assembly{};
	MonoImage* image{};
	MonoClass* scriptClass{};
	MonoObject* scriptInstance{};
	MonoMethod* scriptMethod{};

};