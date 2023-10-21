#include <iostream>
#include <filesystem>

#pragma once


class ScriptEngine {

public:
	static void Init();
	static void Shutdown();
	static void LoadAssembly(const std::filesystem::path& filePath);

private:
	// No need to exist
	static void InitMono();
	static void ShutdownMono();
};

void addInternalCalls();