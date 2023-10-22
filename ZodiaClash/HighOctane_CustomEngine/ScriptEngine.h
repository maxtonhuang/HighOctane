#include <iostream>
#include <filesystem>
#include <string>

#pragma once

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
}



class ScriptEngine {

public:
	static void Init();
	static void Shutdown();

	static void LoadAssembly(const std::filesystem::path& filePath);

private:
	// No need to exist
	static void InitMono();
	static void ShutdownMono();

	static MonoObject* InstantiateClass(MonoClass* classToInstantiate);

	friend class ScriptClass;
};

class ScriptClass {
public:
	ScriptClass() = default;
	ScriptClass(const std::string& classNamespace, const std::string& className);

	MonoObject* Instantiate();
	MonoMethod* GetMethod(const std::string& name, int parameterCount);
	MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);


private:
	std::string m_ClassNamespace;
	std::string m_ClassName;

	MonoClass* m_MonoClass = nullptr;
};

//void addInternalCalls();