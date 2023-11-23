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
#include "InternalCalls.h"

// Unordered map to map the string to the ScriptFieldType
static std::unordered_map<std::string, ScriptFieldType> s_ScriptFieldTypeMap =
{
    { "System.Single", ScriptFieldType::Float },
    { "System.Double", ScriptFieldType::Double },
    { "System.Boolean", ScriptFieldType::Bool },
    { "System.Char", ScriptFieldType::Char },
    { "System.Int16", ScriptFieldType::Short },
    { "System.Int32", ScriptFieldType::Int },
    { "System.Int64", ScriptFieldType::Long },
    { "System.Byte", ScriptFieldType::Byte },
    { "System.UInt16", ScriptFieldType::UShort },
    { "System.UInt32", ScriptFieldType::UInt },
    { "System.UInt64", ScriptFieldType::ULong },

    { "Vector2", ScriptFieldType::Vector2 },
    { "Vector3", ScriptFieldType::Vector3 },
    { "Vector4", ScriptFieldType::Vector4 },

    { "Entity", ScriptFieldType::Entity },
};

/*-------FORWARD DECLARATIONS--------*/
ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType);


static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize);
/*-------FORWARD DECLARATIONS--------*/



// Extern for the vector to contain the full name for ImGui
extern std::vector<std::string> fullNameVecImGUI;

//static ScriptEngineData* scriptData = nullptr;
ScriptEngineData* ScriptEngine::scriptData = nullptr;
std::string ScriptEngine::defaultPath = "";

void ScriptEngine::Init() 
{
    scriptData = GetInstance();
    InitMono();

    std::string fullAssemblyPath = std::filesystem::current_path().string() + "\\HighOctane_CSharpScript.dll";

    if (!std::filesystem::exists(fullAssemblyPath)) 
    {
        fullAssemblyPath = "HighOctane_CSharpScript.dll";
	}
    LoadAssembly(fullAssemblyPath);
    
    LoadAssemblyClasses(scriptData->CoreAssembly);

    // This is to add all the internal calls
    internalcalls::AddInternalCall();

    // Find all the methods that has entity
    scriptData->EntityClass = ScriptClass("", "Entity");
}

void ScriptEngine::Shutdown() 
{
    ShutdownMono();
    delete scriptData;
}

void ScriptEngine::InitMono() 
{
    std::string filePath = std::filesystem::current_path().string() + "\\Mono\\lib\\mono\\4.5";
    mono_set_assemblies_path(filePath.c_str());

    MonoDomain* rootDomain = mono_jit_init("HighOctaneRuntime");

    ASSERT(rootDomain == nullptr, "Root domain is null");

    // Store the root domain pointer
    scriptData->RootDomain = rootDomain;
}


void ScriptEngine::LoadAssembly(const std::filesystem::path& filePath)
{
    // Create an App Domain
    scriptData->AppDomain = mono_domain_create_appdomain((char*)("HighOctane"), nullptr);
    mono_domain_set(scriptData->AppDomain, false);
    if (!LoadMonoAssembly(filePath)) 
    {
        scriptData->CoreAssembly = LoadMonoAssembly("HighOctane_CSharpScript.dll");
    }
    else scriptData->CoreAssembly = LoadMonoAssembly(filePath);
    scriptData->CoreAssemblyImage = mono_assembly_get_image(scriptData->CoreAssembly);
}

void ScriptEngine::UnloadAssembly() 
{
    if (scriptData->AppDomain) 
    {
        scriptData->AppDomain = nullptr;
    }
}

void ScriptEngine::HotReloadScript() {
    std::cout << "HotRealoadScript called" << std::endl;

    // If debug mode
#if (ENABLE_DEBUG_DIAG)
    // Relative path to the C# assembly
    const char* relativeAssemblyPath = "\\Debug-x64\\HighOctane_CSharpScript.dll";
#elif (!ENABLE_DEBUG_DIAG)
    const char* relativeAssemblyPath = "\\Release-x64\\HighOctane_CSharpScript.dll";
#endif
    std::string fullAssemblyPath = std::filesystem::current_path().replace_filename("bin").string() + relativeAssemblyPath;


    // This is still not working
    if (isHotReload)
    {
        std::cout << "Hot reload" << std::endl;
		UnloadAssembly();
		LoadAssembly(fullAssemblyPath);
		//LoadAssemblyClasses(scriptData->CoreAssembly);
		isHotReload = false;
	}
}

bool ScriptEngine::EntityClassExists(const std::string& fullClassName) 
{
	return scriptData->EntityClasses.find(fullClassName) != scriptData->EntityClasses.end();
}

void ScriptEngine::ScriptInit(Entity entity) 
{

    auto& sc = ECS::ecs().GetComponent<Script>(entity);

    // For each script associated with this entity
    for (const auto& fullClassName : sc.scriptNameVec) 
    {

        // Check if such a script class exists in our system
        if (ScriptEngine::EntityClassExists(fullClassName)) 
        {

            // Create an instance of this script class
            std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(scriptData->EntityClasses[fullClassName], entity);

            scriptData->EntityInstances[entity].push_back(instance);

            // Call the OnCreate method of this script instance
            instance->InvokeOnCreate();
        }
    }
    //// For each script associated with this entity
    //for (const auto& fullClassName : sc.scriptNameVec)
    //{
    //    if (ScriptEngine::EntityClassExists(fullClassName))
    //    {
    //        Entity entityID = entity;
    //        std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(scriptData->EntityClasses[fullClassName], entity);
    //        scriptData->EntityInstances[entity].push_back(instance);
    //        // Copy the field values
    //        if (scriptData->EntityScriptFields.find(entity) != scriptData->EntityScriptFields.end())
    //        {
    //            const ScriptFieldMap& fieldMap = scriptData->EntityScriptFields[entity];
    //            for (const auto& field : fieldMap)
    //            {
				//	instance->SetFieldValueInternal(field.first, field.second.m_Buffer);
				//}

    //            instance->InvokeOnCreate();
    //        }
    //    }
    //}
}

void ScriptEngine::ScriptUpdate(const Entity& entity)
{
    auto it = scriptData->EntityInstances.find(entity);
    if (it != scriptData->EntityInstances.end())
    {
        // Iterate through all script instances associated with this entity.
        for (auto& scriptInstance : it->second)
        {
            scriptInstance->InvokeOnUpdate();
        }
    }
}

void ScriptEngine::AttachScriptToEntity(Entity entity, std::string scriptName) 
{

    auto& sc = ECS::ecs().GetComponent<Script>(entity);
    // Checks if the currentScriptForIMGUI is already in scriptNameVec
    for (int i = 0; i < sc.scriptNameVec.size(); i++) 
    {
        if (sc.scriptNameVec[i] == scriptName) 
        {
            DEBUG_PRINT("Script %s already exists in entity %d", scriptName.c_str(), entity);
            return;
        }

        else 
        {
            continue;
        }
    }

    // If not, add it to the vectors
    sc.scriptNameVec.push_back(scriptName);
    scriptNamesAttachedforIMGUI[entity].push_back(scriptName);
    DEBUG_PRINT("Script %s added to entity %d", scriptName.c_str(), entity)

    auto& entityScripts = scriptData->EntityInstances[entity];
    std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(scriptData->EntityClasses[scriptName], entity);
    entityScripts.push_back(instance);
}

void ScriptEngine::SetDefaultPath(std::string path)
{
    defaultPath = "Mono/" + path;
    //std::string path{ defaultPath + initFilePath };
    //Serializer serializer;
    //if (!serializer.Open(path)) {
    //    defaultPath = "../Assets/";
    //    path = defaultPath + initFilePath;
    //    if (!serializer.Open(path)) {
    //        ASSERT(1, "Unable to initialize asset manager!");
    //        return;
    //    }
    //}
}

void ScriptEngine::RemoveScriptFromEntity(Entity entity, std::string scriptName)
{
    auto& sc = ECS::ecs().GetComponent<Script>(entity);

    // Remove the script instances from the vector
    auto& instances = scriptData->EntityInstances[entity];
    for (int i = 0; i < instances.size(); ) {
        if (instances[i]->GetScriptName() == scriptName) 
        {
            instances.erase(instances.begin() + i);
            // Do not increment i, as the next element has shifted into the current position
        }
        else 
        {
            // Increment i only if an element was not erased
            ++i;
        }
    }
    // Now remove the script name from scriptNameVec and scriptNamesAttachedforIMGUI
    for (int i = 0; i < sc.scriptNameVec.size(); )
    {
        if (sc.scriptNameVec[i] == scriptName) 
        {
            sc.scriptNameVec.erase(sc.scriptNameVec.begin() + i);
            scriptNamesAttachedforIMGUI[entity].erase(scriptNamesAttachedforIMGUI[entity].begin() + i);
            DEBUG_PRINT("Script %s removed from entity %d", scriptName.c_str(), entity);
            break;
        }
        else {
            // Increment i only if an element was not erased
            ++i;
        }
    }
}

std::shared_ptr<ScriptInstance> ScriptEngine::GetEntityScriptInstance(Entity entity, int i)
{
    auto it = scriptData->EntityInstances.find(entity);
    if (it == scriptData->EntityInstances.end())
		return nullptr;
    return it->second[i];
}



void ScriptEngine::LoadAssemblyClasses(MonoAssembly* assembly)
{
    scriptData->EntityClasses.clear();
    MonoImage* image = mono_assembly_get_image(assembly);
    const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

    // This is the one that will call the classes that inherit MonoBehaviour
    MonoClass* entityClass = mono_class_from_name(image, "", "MonoBehaviour");

    // Unordered map of class name to vector of bool?
	std::unordered_map<std::string, std::vector<bool>> classToBoolMap;

    for (int32_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        // Get the namespace and the name of the class
        const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
        std::string fullName;

        /*-------CHECK IF HAVE NAMESPACE--------*/
        if (strlen(nameSpace) != 0) 
        {
            fullName = std::string(nameSpace) + "." + std::string(name);
        }
        else 
        {

            // If there's no namespace, just use the name
			fullName = std::string(name);
        }
        MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);

        if (monoClass == entityClass) 
        {
            continue;
        }  
        /*-------CHECK IF HAVE NAMESPACE--------*/

        /*-------CHECK IF SUBCLASS OR NOT--------*/
        bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);

        if (!isEntity) 
        {
			continue;
		}

        std::shared_ptr<ScriptClass> scriptClass = std::make_shared<ScriptClass>(nameSpace, name);
        scriptData->EntityClasses[fullName] = scriptClass;
        fullNameVecImGUI.emplace_back(fullName);

        /*-------CHECK IF PRIVATE OR PUBLIC--------*/
        void* iter = nullptr;
        MonoClassField* field;

        while ((field = mono_class_get_fields(monoClass, &iter)) != nullptr) 
        {
            const char* fieldName = mono_field_get_name(field);
            uint32_t flags = mono_field_get_flags(field);

            // If is public
            if (flags & MONO_FIELD_ATTR_PUBLIC)
            {
                MonoType* type = mono_field_get_type(field);
                ScriptFieldType fieldType = MonoTypeToScriptFieldType(type);

                scriptClass->m_Fields[fieldName] = { fieldType, fieldName, field };
                
                //DEBUG_PRINT("Field: %s, Type: %s", fieldName, ScriptFieldTypeToString(fieldType));
            }
        }
        /*-------CHECK IF PRIVATE OR PUBLIC--------*/

        /*-------CHECK IF SUBCLASS OR NOT--------*/
    }
}

void ScriptEngine::ShutdownMono() 
{
    scriptData->AppDomain = nullptr;
    mono_jit_cleanup(scriptData->RootDomain);
}

MonoObject* ScriptEngine::InstantiateClass(MonoClass* classToInstantiate) 
{
    MonoObject* instance = mono_object_new(scriptData->AppDomain, classToInstantiate);
    mono_runtime_object_init(instance);
    return instance;
}

ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className) : m_ClassNamespace(classNamespace), m_ClassName(className) 
{
    m_MonoClass = mono_class_from_name(ScriptEngine::scriptData->CoreAssemblyImage, classNamespace.c_str(), className.c_str());
}

MonoObject* ScriptClass::Instantiate() 
{
    return ScriptEngine::InstantiateClass(m_MonoClass);
}

MonoMethod* ScriptClass::GetMethod(const std::string& name, int parameterCount) 
{
    return mono_class_get_method_from_name(m_MonoClass, name.c_str(), parameterCount);
}

MonoObject* ScriptClass::InvokeMethod(MonoObject* instance, MonoMethod* method, void** params )
{
    return mono_runtime_invoke(method, instance, params, nullptr);
}

ScriptInstance::ScriptInstance(std::shared_ptr<ScriptClass> scriptClass, Entity entity) : m_ScriptClass(scriptClass) 
{
	m_Instance = scriptClass->Instantiate();

    // Get the constructor and the OnCreate method
    m_Constructor = ScriptEngine::scriptData->EntityClass.GetMethod(".ctor", 1);

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

void ScriptInstance::InvokeOnCreate() 
{
	m_ScriptClass->InvokeMethod(m_Instance, m_StartMethod);
}

void ScriptInstance::InvokeOnUpdate() 
{
	m_ScriptClass->InvokeMethod(m_Instance, m_UpdateMethod);
}

std::string ScriptInstance::GetScriptName() const 
{
    if (m_ScriptClass) {
        // Check if there's namespace
        if (m_ScriptClass->GetMClassNameSpace() == "") 
        {
            return m_ScriptClass->GetMClassName();
        }
        else 
        {
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

bool ScriptInstance::GetFieldValueInternal(const std::string& name, void* buffer)
{
    const auto& fields = m_ScriptClass->GetFields();
    auto it = fields.find(name);
    if (it == fields.end()) {
        return false;
    }
    const ScriptField& field = it->second;
    mono_field_get_value(m_Instance, field.ClassField, buffer);
    return true;
}

bool ScriptInstance::SetFieldValueInternal(const std::string& name, const void* value)
{
    const auto& fields = m_ScriptClass->GetFields();
    auto it = fields.find(name);
    if (it == fields.end())
        return false;

    const ScriptField& field = it->second;
    mono_field_set_value(m_Instance, field.ClassField, (void*)value);
    return true;
}

ScriptFieldType MonoTypeToScriptFieldType(MonoType* monoType)
{
    std::string typeName = mono_type_get_name(monoType);

    auto it = s_ScriptFieldTypeMap.find(typeName);
    if (it == s_ScriptFieldTypeMap.end())
    {
        printf("Unknown type: %s\n", typeName.c_str());
        return ScriptFieldType::None;
    }

    return it->second;
}



// I need a serialization function here to get the things