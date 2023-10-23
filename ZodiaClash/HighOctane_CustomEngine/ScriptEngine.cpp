#include "ScriptEngine.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include "DebugDiagnostic.h"
#include "InternalCalls.cpp"

// Forward declaration
char* ReadBytes(const std::filesystem::path& filepath, uint32_t* outSize);
MonoAssembly* LoadMonoAssembly(const std::filesystem::path& assemblyPath);
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
    std::unordered_map<uint32_t, std::shared_ptr<ScriptInstance>> EntityInstances;

};

static ScriptEngineData* s_Data = nullptr;



void ScriptEngine::Init() {
    std::cout << "Hi this is initialized for scripting system\n";
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
    auto& classes = s_Data->EntityClasses;

    //printf("Classes size: %d\n", classes.size());

    // This is to add the internal calls
    internalcalls::addInternalCalls();

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
    std::cout << "Scripting InitMono\n";
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

    // This is the script name, if there is no script it won't run
    sc.scriptName = "Sandbox.Player";
    //std::cout << "ScriptEngine.cpp::OnCreateEntity, script name: " << sc.scriptName << std::endl;
    if (ScriptEngine::EntityClassExists(sc.scriptName)) {
        //std::cout << "ScriptEngine.cpp::OnCreateEntity, class exists" << std::endl;
        std::shared_ptr<ScriptInstance> instance = std::make_shared<ScriptInstance>(s_Data->EntityClasses[sc.scriptName]);
        s_Data->EntityInstances[entity] = instance;
        instance->InvokeOnCreate();
	}
}

void ScriptEngine::OnUpdateEntity(Entity entity) {
    //std::cout << "ScriptEngine.cpp::OnUpdateEntity:" << entity << std::endl;

    if (s_Data->EntityInstances.find(entity) != s_Data->EntityInstances.end())
    {
        std::shared_ptr<ScriptInstance> instance = s_Data->EntityInstances[entity];
        instance->InvokeOnUpdate();
    }
    else {
		std::cout << "ScriptEngine.cpp::OnUpdateEntity, instance not found" << std::endl;
    }

}

void ScriptEngine::LoadAssemblyClasses(MonoAssembly* assembly)
{
    s_Data->EntityClasses.clear();
    MonoImage* image = mono_assembly_get_image(assembly);
    const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);
    MonoClass* entityClass = mono_class_from_name(image, "", "Entity");

    for (int32_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

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
        bool isEntity = mono_class_is_subclass_of(monoClass, entityClass, false);
        if (isEntity) {
            s_Data->EntityClasses[fullName] = std::make_shared<ScriptClass>(nameSpace, name);
            std::cout << "Added class: " << fullName << std::endl;
        }

        printf("%s.%s\n", nameSpace, name);
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


ScriptInstance::ScriptInstance(std::shared_ptr<ScriptClass> scriptClass) : m_ScriptClass(scriptClass) {
	m_Instance = scriptClass->Instantiate();

    m_OnCreateMethod = scriptClass->GetMethod("OnCreate", 0);
    m_OnUpdateMethod = scriptClass->GetMethod("OnUpdate", 0);
}

void ScriptInstance::InvokeOnCreate() {
    std::cout << "ScriptEngine.cpp::InvokeOnCreate" << std::endl;
	m_ScriptClass->InvokeMethod(m_Instance, m_OnCreateMethod);
    std::cout << "ScriptEngine.cpp::InvokeOnCreate m_Instance: " << m_Instance << std::endl;
    std::cout << "ScriptEngine.cpp::InvokeOnCreate m_OnCreateMethod: " << m_OnCreateMethod << std::endl;
}

void ScriptInstance::InvokeOnUpdate() {
	m_ScriptClass->InvokeMethod(m_Instance, m_OnUpdateMethod);
}

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