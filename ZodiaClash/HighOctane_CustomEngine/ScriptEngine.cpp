#include "ScriptEngine.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include "DebugDiagnostic.h"

// Forward declaration
char* ReadBytes(const std::string& filepath, uint32_t* outSize);
MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);
void PrintAssemblyTypes(MonoAssembly* assembly);


struct ScriptEngineData {
    MonoDomain* RootDomain = nullptr;
    MonoDomain* AppDomain = nullptr;
    
    MonoAssembly* CoreAssembly = nullptr;
};

static ScriptEngineData* s_Data = nullptr;


void ScriptEngine::Init() {
    s_Data = new ScriptEngineData();
	InitMono();
}

void ScriptEngine::Shutdown() {
    ShutdownMono();
    delete s_Data;
}

void ScriptEngine::InitMono() {
    
    // Setting the path to the mono
    std::cout << "Scripting intialized\n";
    std::string filePath = std::filesystem::current_path().replace_filename("Extern\\Mono\\lib\\mono\\4.5").string();
    //std::cout << filePath << std::endl;
    mono_set_assemblies_path(filePath.c_str());

    MonoDomain* rootDomain = mono_jit_init("HighOctaneRuntime");
    ASSERT(rootDomain == nullptr, "Root domain is null");

    // Store the root domain pointer
    s_Data->RootDomain = rootDomain;

    // Create an App Domain
    s_Data->AppDomain = mono_domain_create_appdomain((char*)("HighOctane") , nullptr);
    mono_domain_set(s_Data->AppDomain, true);

    // Move this maybe
    // If debug mode
#if (ENABLE_DEBUG_DIAG)
    // Relative path to the C# assembly
    const char* relativeAssemblyPath = "\\Debug-x64\\HighOctane_CSharpScript.dll";
#elif (!ENABLE_DEBUG_DIAG)
    const char* relativeAssemblyPath = "\\Release-x64\\HighOctane_CSharpScript.dll";
#endif

    std::string fullAssemblyPath = std::filesystem::current_path().replace_filename("bin").string() + relativeAssemblyPath;
    s_Data->CoreAssembly = LoadCSharpAssembly(fullAssemblyPath);
    PrintAssemblyTypes(s_Data->CoreAssembly);

    MonoImage* assemblyImage = mono_assembly_get_image(s_Data->CoreAssembly);

    
    MonoClass* monoClass = mono_class_from_name(assemblyImage, "", "Main");
       
    // Create an object
    MonoObject* instance = mono_object_new(s_Data->AppDomain, monoClass);
    mono_runtime_object_init(instance);

    // Call function
    MonoMethod* printMessageFunc = mono_class_get_method_from_name(monoClass, "PrintMessage", 0);
    mono_runtime_invoke(printMessageFunc, instance, nullptr, nullptr);

    // Call function with param
    MonoString* monoString = mono_string_new(s_Data->AppDomain, "Hello World from C++!");

    MonoMethod* printCustomMessageFunc = mono_class_get_method_from_name(monoClass, "PrintCustomMessage",1);
    void* stringParam = monoString;
    mono_runtime_invoke(printCustomMessageFunc, instance, &stringParam, nullptr);

}


void ScriptEngine::ShutdownMono() {
    //mono_domain_unload(s_Data->AppDomain);
    s_Data->AppDomain = nullptr;
    mono_jit_cleanup(s_Data->RootDomain);
	//mono_jit_cleanup(s_Data->AppDomain);
}

char* ReadBytes(const std::string& filepath, uint32_t* outSize)
{
    std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

    if (!stream)
    {
        // Failed to open the file
        return nullptr;
    }

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    uint32_t size = end - stream.tellg();

    if (size == 0)
    {
        // File is empty
        return nullptr;
    }

    char* buffer = new char[size];
    stream.read((char*)buffer, size);
    stream.close();

    *outSize = size;
    return buffer;
}

MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath)
{
    uint32_t fileSize = 0;
    char* fileData = ReadBytes(assemblyPath, &fileSize);

    // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
    MonoImageOpenStatus status;
    MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    if (status != MONO_IMAGE_OK)
    {
        const char* errorMessage = mono_image_strerror(status);
        // Log some error message using the errorMessage data
        return nullptr;
    }

    MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
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