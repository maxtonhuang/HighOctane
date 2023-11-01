#include <iostream>
#include <filesystem>
#include <string>
#include "ECS.h"
#include "Global.h"
#include "Message.h"
#include "Components.h"
#include "Debuglog.h"

#pragma once

extern "C" {
	typedef struct _MonoClass MonoClass;
	typedef struct _MonoObject MonoObject;
	typedef struct _MonoMethod MonoMethod;
	typedef struct _MonoAssembly MonoAssembly;
	typedef struct _MonoImage MonoImage;
	typedef struct _MonoDomain MonoDomain;
	typedef struct _MonoAssemblyName MonoAssemblyName;

}



class ScriptEngine {

public:
	static void Init();
	static void Shutdown();

	static void LoadAssembly(const std::filesystem::path& filePath);

	static bool EntityClassExists(const std::string& fullClassName);
	static void OnCreateEntity(Entity entity);
	static void OnUpdateEntity(const Entity& entity);
	static void RunTimeAddScript(Entity entity, const char* scriptName);
	static void RunTimeRemoveScript(Entity entity, const char* scriptName);

private:
	// No need to exist
	static void InitMono();
	static void ShutdownMono();

	static MonoObject* InstantiateClass(MonoClass* classToInstantiate);
	static void LoadAssemblyClasses(MonoAssembly* assembly);

	friend class ScriptClass;
};

class ScriptClass {
public:
	ScriptClass() = default;
	ScriptClass(const std::string& classNamespace, const std::string& className);

	MonoObject* Instantiate();

	MonoMethod* GetMethod(const std::string& name, int parameterCount);
	MonoObject* InvokeMethod(MonoObject* instance, MonoMethod* method, void** params = nullptr);

	std::string m_ClassNamespace;
	std::string m_ClassName;
private:


	MonoClass* m_MonoClass = nullptr;
};

class ScriptInstance {
public: 
	ScriptInstance(std::shared_ptr<ScriptClass> scriptClass, Entity entity);

	void InvokeOnCreate();
	void InvokeOnUpdate();
	std::string GetScriptName() const;

private:

	std::shared_ptr<ScriptClass> m_ScriptClass = nullptr;
	MonoObject* m_Instance = nullptr;
	MonoMethod* m_Constructor = nullptr;
	MonoMethod* m_OnCreateMethod = nullptr;
	MonoMethod* m_OnUpdateMethod = nullptr;

};

//void addInternalCalls();