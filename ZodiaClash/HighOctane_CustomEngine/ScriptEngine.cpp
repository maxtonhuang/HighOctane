/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		ScriptEngine.cpp
*
*	@author		Koh Wen Yuan
*
*	@email		k.wenyuan\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		20 October 2023
*
* *****************************************************************************
*
*	@brief		cpp files for the scripting engine
*
*	This file contains the definition of the functions for the scripting engine
******************************************************************************/

#include "ScriptEngine.h"
#include "DebugDiagnostic.h"
#include "InternalCalls.cpp"

// Extern for the vector to contain the full name for ImGui
extern std::vector<std::string> fullNameVecImGUI;

struct ScriptEngineData {
    MonoDomain* RootDomain = nullptr;
    MonoDomain* AppDomain = nullptr;
    
    MonoAssembly* CoreAssembly = nullptr;
    MonoImage* CoreAssemblyImage = nullptr;

    ScriptClass EntityClass;

    std::unordered_map<std::string, std::shared_ptr<ScriptClass>> EntityClasses;
    std::unordered_map<Entity, std::vector<std::shared_ptr<ScriptInstance>>> EntityInstances;

};

static ScriptEngineData* scriptData = nullptr;

void ScriptEngine::Init() {
    scriptData = new ScriptEngineData(); 
    InitMono();

    // If debug mode
#if (ENABLE_DEBUG_DIAG)
    // Relative path to the C# assembly
    const char* relativeAssemblyPath = "\\Debug-x64\\HighOctane_CSharpScript.dll";
#elif (!ENABLE_DEBUG_DIAG)
    const char* relativeAssemblyPath = "\\Release-x64\\HighOctane_CSharpScript.dll";
#endif
    std::string fullAssemblyPath = std::filesystem::current_path().replace_filename("bin").string() + relativeAssemblyPath;

    if (!std::filesystem::exists(fullAssemblyPath)) {
        fullAssemblyPath = "HighOctane_CSharpScript.dll";
	}
    LoadAssembly(fullAssemblyPath);
    
    LoadAssemblyClasses(scriptData->CoreAssembly);

    // This is to add all the internal calls
    internalcalls::AddInternalCall();

    // Find all the methods that has entity
    scriptData->EntityClass = ScriptClass("", "Entity");
}

void ScriptEngine::Shutdown() {
    ShutdownMono();
    delete scriptData;
}

void ScriptEngine::InitMono() {
    // Setting the path to the mono
    std::string filePath = std::filesystem::current_path().replace_filename("Extern/Mono/lib/mono/4.5").string();

    if (std::filesystem::exists(filePath)) {
        mono_set_assemblies_path(filePath.c_str());
        
    }
    else {
        //filePath = std::filesystem::current_path().replace_filename("Debug-x64/Mono/lib/mono/4.5").string();
        //if (std::filesystem::exists(filePath)) {
        //    mono_set_assemblies_path(filePath.c_str());
        //}
        //else {
        //    filePath = std::filesystem::current_path().replace_filename("HighOctane_CustomEngine/Extern/Mono/lib/mono/4.5").string();
        //    mono_set_assemblies_path(filePath.c_str());
        //}
        DEBUG_PRINT("Mono path not found");
    }
    
    MonoDomain* rootDomain = mono_jit_init("HighOctaneRuntime");

    ASSERT(rootDomain == nullptr, "Root domain is null");

    // Store the root domain pointer
    scriptData->RootDomain = rootDomain;
}


void ScriptEngine::LoadAssembly(const std::filesystem::path& filePath)
{
    // Create an App Domain
    scriptData->AppDomain = mono_domain_create_appdomain((char*)("HighOctane"), nullptr);
    mono_domain_set(scriptData->AppDomain, true);
    if (!LoadMonoAssembly(filePath)) {
        scriptData->CoreAssembly = LoadMonoAssembly("HighOctane_CSharpScript.dll");
    }
    else scriptData->CoreAssembly = LoadMonoAssembly(filePath);
    scriptData->CoreAssemblyImage = mono_assembly_get_image(scriptData->CoreAssembly);
}

bool ScriptEngine::EntityClassExists(const std::string& fullClassName) {
	return scriptData->EntityClasses.find(fullClassName) != scriptData->EntityClasses.end();
}

void ScriptEngine::ScriptInit(Entity entity) {

    auto& sc = ECS::ecs().GetComponent<Script>(entity);
    // For each script associated with this entity
    for (const auto& fullClassName : sc.scriptNameVec) {

        // Check if such a script class exists in our system
        if (ScriptEngine::EntityClassExists(fullClassName)) {

            // Create an instance of this script class
            std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(scriptData->EntityClasses[fullClassName], entity);

            scriptData->EntityInstances[entity].push_back(instance);

            // Call the OnCreate method of this script instance
            instance->InvokeOnCreate();
        }
    }
}

void ScriptEngine::RunTimeAddScript(Entity entity, const char* scriptName) {

    auto& sc = ECS::ecs().GetComponent<Script>(entity);
    // Checks if the currentScriptForIMGUI is already in scriptNameVec
    for (int i = 0; i < sc.scriptNameVec.size(); i++) {
        if (sc.scriptNameVec[i] == scriptName) {
            DEBUG_PRINT("Script %s already exists in entity %d", scriptName, entity);
            return;
        }

        else {
            continue;
        }
    }

    // If not, add it to the vectors
    sc.scriptNameVec.push_back(scriptName);
    scriptNamesAttachedforIMGUI[entity].push_back(scriptName);

    auto& entityScripts = scriptData->EntityInstances[entity];
    std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(scriptData->EntityClasses[scriptName], entity);
    entityScripts.push_back(instance);
}

void ScriptEngine::RunTimeRemoveScript(Entity entity, const char* scriptName) {

    auto& sc = ECS::ecs().GetComponent<Script>(entity);
    
    for (std::vector<std::shared_ptr<ScriptInstance>>::iterator it = scriptData->EntityInstances[entity].begin(); it != scriptData->EntityInstances[entity].end(); ++it) {

        if ((*it)->GetScriptName() == scriptName) {

            // Remove the script instance from the vector
			scriptData->EntityInstances[entity].erase(it);

            for (int i = 0; i < sc.scriptNameVec.size(); i++) {

                if (sc.scriptNameVec[i] == scriptName) {

                    // Remove the script name from the vector
					sc.scriptNameVec.erase(sc.scriptNameVec.begin() + i);
                    scriptNamesAttachedforIMGUI[entity].erase(scriptNamesAttachedforIMGUI[entity].begin() + i);
				}
			}
			break;
		}
	}
}

void ScriptEngine::ScriptUpdate(const Entity& entity) {
    auto it = scriptData->EntityInstances.find(entity);
    if (it != scriptData->EntityInstances.end()) {
        // Iterate through all script instances associated with this entity.
        for (auto& scriptInstance : it->second) {
            scriptInstance->InvokeOnUpdate();
        }
    }
}

void ScriptEngine::LoadAssemblyClasses(MonoAssembly* assembly)
{
    scriptData->EntityClasses.clear();
    MonoImage* image = mono_assembly_get_image(assembly);
    const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

    // This is the one that will call the classes that inherit MonoBehaviour
    MonoClass* entityClass = mono_class_from_name(image, "", "MonoBehaviour");

    for (int32_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        // Get the namespace and the name of the class
        const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
        std::string fullName;

        // Check if there's namespace
        if (strlen(nameSpace) != 0) {
            fullName = std::string(nameSpace) + "." + std::string(name);
        }
        else {

            // If there's no namespace, just use the name
			fullName = std::string(name);
        }
        MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);

        if (monoClass == entityClass) {
            continue;
        }

        // Check if it is a subclass
        bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
        if (isEntity) {
            scriptData->EntityClasses[fullName] = std::make_shared<ScriptClass>(nameSpace, name);
            fullNameVecImGUI.emplace_back(fullName);
        } 
    }

}

void ScriptEngine::ShutdownMono() {
    scriptData->AppDomain = nullptr;
    mono_jit_cleanup(scriptData->RootDomain);
}

MonoObject* ScriptEngine::InstantiateClass(MonoClass* classToInstantiate) {
    MonoObject* instance = mono_object_new(scriptData->AppDomain, classToInstantiate);
    mono_runtime_object_init(instance);
    return instance;
}

ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className) : m_ClassNamespace(classNamespace), m_ClassName(className) {
    m_MonoClass = mono_class_from_name(scriptData->CoreAssemblyImage, classNamespace.c_str(), className.c_str());
}

MonoObject* ScriptClass::Instantiate() {
    return ScriptEngine::InstantiateClass(m_MonoClass);
}

MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount) {
    return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
}

MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params )
{
    return mono_runtime_invoke(method, instance, params, nullptr);
}

ScriptInstance::ScriptInstance(std::shared_ptr<ScriptClass> scriptClass, Entity entity) : m_ScriptClass(scriptClass) {
	m_Instance = scriptClass->Instantiate();

    // Get the constructor and the OnCreate method
    m_Constructor = scriptData->EntityClass.GetMethod(".ctor", 1);

    // Get the OnUpdate method
    m_StartMethod = scriptClass->GetMethod("Start", 0);
    m_UpdateMethod = scriptClass->GetMethod("Update", 0);

    // Call entity constructor
    {
        Entity entityID = entity;
        void* param = &entityID;
        m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
    }

}

void ScriptInstance::InvokeOnCreate() {
	m_ScriptClass->InvokeMethod(m_Instance, m_StartMethod);
}

void ScriptInstance::InvokeOnUpdate() {
	m_ScriptClass->InvokeMethod(m_Instance, m_UpdateMethod);
}

std::string ScriptInstance::GetScriptName() const {
    if (m_ScriptClass) {
        // Check if there's namespace
        if (m_ScriptClass->GetMClassNameSpace() == "") {
            return m_ScriptClass->GetMClassName();
        }
        else {
            // Assuming the full script name is composed of Namespace + "." + ClassName
            return m_ScriptClass->GetMClassNameSpace() + "." + m_ScriptClass->GetMClassName();
        }

    }
    return ""; // Return an empty string if m_ScriptClass is nullptr
}

std::string ScriptClass::GetMClassNameSpace() const {
    return m_ClassNamespace;
}

std::string ScriptClass::GetMClassName() const {
	return m_ClassName;
}

static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize)
{
    std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

    if (!stream)
    {
        // Failed to open the file
        return nullptr;
    }

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    uint64_t size = end - stream.tellg();

    if (size == 0)
    {
        // File is empty
        return nullptr;
    }

    char* buffer = new char[size];
    stream.read((char*)buffer, size);
    stream.close();

    *outSize = (uint32_t)size;
    return buffer;
}

static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath)
{
    uint32_t fileSize = 0;
    char* fileData = ReadBytes(assemblyPath, &fileSize);

    MonoImageOpenStatus status;
    MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    std::string pathString = assemblyPath.string();

    MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
    mono_image_close(image);

    // Free the file Data
    delete[] fileData;

    return assembly;
}
