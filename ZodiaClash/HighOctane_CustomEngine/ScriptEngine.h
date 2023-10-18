#include <iostream>
#include <filesystem>

#pragma once


class ScriptEngine {

public:
	static void Init();
	static void Shutdown();

private:
	// No need to exist
	static void InitMono();
	static void ShutdownMono();
};