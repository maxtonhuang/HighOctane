#include "ScriptEngine.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include "DebugDiagnostic.h"
#include "InternalCalls.cpp"

// Extern for the vector to contain the full name for ImGui
extern std::vector<std::string> fullNameVecImGUI;

// Forward declaration
static char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize);
static MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath);
void PrintAssemblyTypes(MonoAssembly* assembly);


struct ScriptEngineData {
    MonoDomain* RootDomain = nullptr;
    MonoDomain* AppDomain = nullptr;
    
    MonoAssembly* CoreAssembly = nullptr;
    MonoImage* CoreAssemblyImage = nullptr;

    ScriptClass EntityClass;

 //   void onCreate() {
 //       std::cout << "On create called\n";
	//}

    std::unordered_map<std::string, std::shared_ptr<ScriptClass>> EntityClasses;
    std::unordered_map<Entity, std::vector<std::shared_ptr<ScriptInstance>>> EntityInstances;
    //std::vector<std::shared_ptr<ScriptInstance>> EntityInstances;


};

static ScriptEngineData* s_Data = nullptr;

struct CSharpClassInfo {
    std::string namespaceName;
    std::string className;
    // Add more fields if necessary.
};

void ScriptEngine::Init() {
    //std::cout << "Hi this is initialized for scripting system\n";
    s_Data = new ScriptEngineData(); 
    InitMono();
    // If debug mode
#if (ENABLE_DEBUG_DIAG)
    // Relative path to the C# assembly
    const char* relativeAssemblyPath = "\\Debug-x64\\HighOctane_CSharpScript.dll";
#elif (!ENABLE_DEBUG_DIAG)
    const char* relativeAssemblyPath = "\\Release-x64\\HighOctane_CSharpScript.dll";
#endif

    std::string fullAssemblyPath = std::filesystem::current_path().replace_filename("bin").string() + relativeAssemblyPath;

    LoadAssembly(fullAssemblyPath);
    LoadAssemblyClasses(s_Data->CoreAssembly);
    //auto& classes = s_Data->EntityClasses;

    //printf("Classes size: %d\n", classes.size());

    // This is to add the internal calls
    internalcalls::AddInternalCall();
    s_Data->EntityClass = ScriptClass("", "Entity");
#if 0
    // Retrieve and insantiate class (with constructor)
    s_Data->EntityClass = ScriptClass("", "Entity");

    MonoObject* instance = s_Data->EntityClass.Instantiate();

    // Call function (method)
    MonoMethod* printMessageFunc = s_Data->EntityClass.GetMethod("PrintMessage", 0);
    s_Data->EntityClass.InvokeMethod(instance, printMessageFunc, nullptr);

    MonoMethod* printCustomMessageFunc = s_Data->EntityClass.GetMethod("PrintCustomMessage", 1);
    // Call function with param
    MonoString* monoString = mono_string_new(s_Data->AppDomain, "Hello World from C++!");

    void* stringParam = monoString;
    s_Data->EntityClass.InvokeMethod(instance, printCustomMessageFunc, &stringParam);
#endif
}

void ScriptEngine::Shutdown() {
    ShutdownMono();
    delete s_Data;
}

void ScriptEngine::InitMono() {
    
    // Setting the path to the mono
    //std::cout << "Scripting InitMono\n";
    std::string filePath = std::filesystem::current_path().replace_filename("Extern\\Mono\\lib\\mono\\4.5").string();
    //std::cout << filePath << std::endl;
    mono_set_assemblies_path(filePath.c_str());

    MonoDomain* rootDomain = mono_jit_init("HighOctaneRuntime");
    ASSERT(rootDomain == nullptr, "Root domain is null");

    // Store the root domain pointer
    s_Data->RootDomain = rootDomain;
}

void ScriptEngine::LoadAssembly(const std::filesystem::path& filePath)
{
    // Create an App Domain
    s_Data->AppDomain = mono_domain_create_appdomain((char*)("HighOctane"), nullptr);
    mono_domain_set(s_Data->AppDomain, true);

    s_Data->CoreAssembly = LoadMonoAssembly(filePath);
    s_Data->CoreAssemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);
    //PrintAssemblyTypes(s_Data->CoreAssembly);
}

void ScriptEngine::OnRuntimeStart() {
    /*-------THIS PART SHOULD MOVE OUT INTO ECS-------*/
    //s_Data->EntityClass = ScriptClass("Sandbox", "Player");

    //MonoObject* instance = s_Data->EntityClass.Instantiate();
    //mono_runtime_invoke(method, scriptInstance, nullptr, nullptr);
    //MonoMethod* method = s_Data->EntityClass.GetMethod("OnCreate", 0);
    /*-------THIS PART SHOULD MOVE OUT INTO ECS-------*/


    //s_Data->EntityClass.InvokeMethod(instance, method, nullptr);

    // Clear the key down afterwards
    //GetKeyDownClear();
    //s_Data->onCreate();
}

void ScriptEngine::OnRuntimeStop() {
    // Empty by default now
}

bool ScriptEngine::EntityClassExists(const std::string& fullClassName) {
	return s_Data->EntityClasses.find(fullClassName) != s_Data->EntityClasses.end();
}

// Not sure if this is working or not haha (Trying to refactor)
void ScriptEngine::OnCreateEntity(Entity entity) {

    auto& sc = ECS::ecs().GetComponent<Script>(entity);
    // For each script associated with this entity
    for (const auto& fullClassName : sc.scriptNameVec) {

        // Check if such a script class exists in our system
        if (ScriptEngine::EntityClassExists(fullClassName)) {

            // Create an instance of this script class
            std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(s_Data->EntityClasses[fullClassName], entity);

            s_Data->EntityInstances[entity].push_back(instance);

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
            //printf("Script %s already exists in entity %d\n", scriptName, entity);
            return;
        }

        else {
            continue;
        }
    }
 //   for (const auto& test : sc.scriptNameVec) {
 //       std::cout << "RunTimeAddScript:: BEFORE scriptNameVec pushback" << test << std::endl;
	//}

    // If not, add it to the vector
    sc.scriptNameVec.push_back(scriptName);
    //std::cout << "RunTimeAddScript:: AFTER scriptNameVec pushback" << std::endl;

    auto& entityScripts = s_Data->EntityInstances[entity];
    std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(s_Data->EntityClasses[scriptName], entity);
    entityScripts.push_back(instance);

    // Debugging
    //for (const auto& testing : entityScripts) {
    //    std::cout << "RunTimeAddScript:: AFTER entityScripts pushback" << testing->GetScriptName() << std::endl;
    //    std::cout << "RunTimeAddScript:: AFTER entityScripts pushback" << entity << std::endl;
    //}
}

// Helper function
std::string ScriptInstance::GetScriptName() const {
    if (m_ScriptClass) {
        // Assuming the full script name is composed of Namespace + "." + ClassName
        return m_ScriptClass->m_ClassNamespace + "." + m_ScriptClass->m_ClassName;
    }
    return ""; // Return an empty string if m_ScriptClass is nullptr
}


// Run time remove script
void ScriptEngine::RunTimeRemoveScript(Entity entity, const char* scriptName) {
    std::cout << "RunTimeRemoveScript called\n";
    // Find the script instance using std::remove_if and lambda function
  //  for (auto& test : s_Data->EntityInstances[entity]) {
  //      if (test->GetScriptName() == scriptName) {
  //          std::cout << "RunTimeRemoveScript:: BEFORE scriptNameVec deletion" << test->GetScriptName() << std::endl;
		//}
  //  }



    // Previous code that is working
    //auto& sc = ECS::ecs().GetComponent<Script>(entity);

    // Clear the instance vector for that entity
    s_Data->EntityInstances[entity].clear();
}

void ScriptEngine::OnUpdateEntity(const Entity& entity) {
    auto it = s_Data->EntityInstances.find(entity);
    if (it != s_Data->EntityInstances.end()) {
        // Iterate through all script instances associated with this entity.
        for (auto& scriptInstance : it->second) {
            scriptInstance->InvokeOnUpdate();
        }
    }
}



void ScriptEngine::LoadAssemblyClasses(MonoAssembly* assembly)
{
    s_Data->EntityClasses.clear();
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
        if (strlen(nameSpace) != 0) {
            fullName = std::string(nameSpace) + "." + std::string(name);
        }
        else {
			fullName = std::string(name);
        }
        MonoClass* monoClass = mono_class_from_name(image, nameSpace, name);

        if (monoClass == entityClass) {
            continue;
        }

        // Check if it is a subclass
        bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
        if (isEntity) {
            s_Data->EntityClasses[fullName] = std::make_shared<ScriptClass>(nameSpace, name);
            fullNameVecImGUI.emplace_back(fullName);
            //std::cout << "Added class: " << fullName << std::endl;
            printf("LoadAssemblyClassesssssssssss: %s.%s\n", nameSpace, name);
        } 
    }

}

void ScriptEngine::ShutdownMono() {
    //mono_domain_unload(s_Data->AppDomain);
    s_Data->AppDomain = nullptr;
    mono_jit_cleanup(s_Data->RootDomain);
	//mono_jit_cleanup(s_Data->AppDomain);
}

MonoObject* ScriptEngine::InstantiateClass(MonoClass* classToInstantiate) {
    MonoObject* instance = mono_object_new(s_Data->AppDomain, classToInstantiate);
    mono_runtime_object_init(instance);
    return instance;
}

ScriptClass::ScriptClass(const std::string& classNamespace, const std::string& className) : m_ClassNamespace(classNamespace), m_ClassName(className) {
    m_MonoClass = mono_class_from_name(s_Data->CoreAssemblyImage, classNamespace.c_str(), className.c_str());
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

    m_Constructor = s_Data->EntityClass.GetMethod(".ctor", 1);
    m_OnCreateMethod = scriptClass->GetMethod("Start", 0);
    m_OnUpdateMethod = scriptClass->GetMethod("Update", 0);



    // Call entity constructor
    {
        Entity entityID = entity;
        void* param = &entityID;
        m_ScriptClass->InvokeMethod(m_Instance, m_Constructor, &param);
    }

}

void ScriptInstance::InvokeOnCreate() {
    //std::cout << "ScriptEngine.cpp::InvokeOnCreate" << std::endl;
	m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
    //std::cout << "ScriptEngine.cpp::InvokeOnCreate m_Instance: " << m_Instance << std::endl;
    //std::cout << "ScriptEngine.cpp::InvokeOnCreate m_OnCreateMethod: " << m_OnCreateMethod << std::endl;
}

void ScriptInstance::InvokeOnUpdate() {
	m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod);
}

//void AddScripts(std::string nameSpace, std::string className, Entity entity) {
//    auto& sc = ECS::ecs().GetComponent<Script>(entity);
//    sc.scriptName = nameSpace + "." + className;
//    sc.monoScriptName.push_back(nameSpace + "." + className);
//}

// Helper functions

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

    // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
    MonoImageOpenStatus status;
    MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    //if (status != MONO_IMAGE_OK)
    //{
    //    const char* errorMessage = mono_image_strerror(status);
    //    // Log some error message using the errorMessage data
    //    return nullptr;
    //}
    std::string pathString = assemblyPath.string();

    MonoAssembly* assembly = mono_assembly_load_from_full(image, pathString.c_str(), &status, 0);
    mono_image_close(image);

    // Don't forget to free the file data
    delete[] fileData;

    return assembly;
}

void PrintAssemblyTypes(MonoAssembly* assembly)
{
    MonoImage* image = mono_assembly_get_image(assembly);
    const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

    for (int32_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
         
        printf("%s.%s\n", nameSpace, name);
    }
}