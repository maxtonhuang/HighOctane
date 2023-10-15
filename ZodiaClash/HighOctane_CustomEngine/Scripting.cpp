#include "Scripting.h"

// String contains function this one is for oliver in the future
//bool StringContains(std::string const& fullString, std::string const& subString) {
//    if (fullString.find(subString) != std::string::npos) {
//		return true;
//	}
//    else {
//		return false;
//	}
//}

//#define MyFunctions _declspec(dllexport)
//
//extern "C" {
//    MyFunctions int AddNumbers(int a, int b) {
//        return a + b;
//    }
//
//    MyFunctions int SubtractNumbers(int a, int b) {
//		return a - b;
//	}
//}
CsScript script;

// This function is to check whether the key is pressed or not on C# side
bool GetKeyDown(INFO key) {
    for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::SCRIPTING]) {
        switch (msg.type) {
        case TYPE::KEY_TRIGGERED:
            if (msg.info == key) {
                return true;
            }
            break;
        }
    }
    return false;
}

// This function is to clear the key pressed but should be in c++ side
void GetKeyDownClear() {
    Mail::mail().mailbox[ADDRESS::SCRIPTING].clear();
}

void testPrintFunction() {
    std::cout << "this is a test" << std::endl;
}

// This function is to log things into my console on C# side
void LogHandler(MonoString* managedMessage)
{
    // Convert a C# string in to a C++ string
    char* message = mono_string_to_utf8(managedMessage);
    // Print message
    std::cout << message << std::endl;
}

//bool testFunc() {
//
//}
//CsScript::CsScript() {
//
//}

CsScript::CsScript() {
    std::cout << "Scripting intialized\n";
    std::string filePath = std::filesystem::current_path().replace_filename("Extern\\Mono\\lib\\mono\\4.5").string();
    //std::cout << filePath << std::endl;


    mono_set_assemblies_path(filePath.c_str());
    domain = mono_jit_init(filePath.c_str());
    
    // Add to the internal call table
    //mono_add_internal_call("CSharpScript.MyScriptClass::Print", (const void*)testPrintFunction);
    ////mono_add_internal_call("HighOctane_CSharpScript.Logging::Log", (const void*)LogHandler);
    //mono_add_internal_call("CSharpScript.MyScriptClass::Log", (const void*)LogHandler);

    // This should be moved somewhere else eventually
    AddInternalCall("InternalCalls::Print", (const void*)testPrintFunction);
    AddInternalCall("InternalCalls::Log", (const void*)LogHandler);
    AddInternalCall("InternalCalls::GetKeyDown", (const void*)GetKeyDown);
    // This should be moved somewhere else eventually

    if (!domain) {
		std::cout << "Domain is null" << std::endl;
		return;
	}
    // If debug mode
    #if (ENABLE_DEBUG_DIAG)
    // Relative path to the C# assembly
        const char* relativeAssemblyPath = "\\Debug-x64\\HighOctane_CSharpScript.dll";
    #elif (!ENABLE_DEBUG_DIAG)
        const char* relativeAssemblyPath = "\\Release-x64\\HighOctane_CSharpScript.dll";
    #endif

    std::string fullAssemblyPath = std::filesystem::current_path().replace_filename("bin").string() + relativeAssemblyPath;
    //std::cout << fullAssemblyPath << std::endl;
    assembly = mono_domain_assembly_open(domain, fullAssemblyPath.c_str());

    if (!assembly) {
		std::cout << "Cannot open assembly" << std::endl;
		return;
	}

    image = mono_assembly_get_image(assembly);

    scriptClass = mono_class_from_name(image, "", "MyScriptClass");
    scriptInstance = mono_object_new(domain, scriptClass);
    mono_runtime_object_init(scriptInstance);

    MonoMethod* scriptStartMethod = mono_class_get_method_from_name(scriptClass, "Start", -1);
    MonoMethod* scriptStopMethod = mono_class_get_method_from_name(scriptClass, "Stop", -1);
    MonoMethod* scriptCallMethod = mono_class_get_method_from_name(scriptClass, "PrintFromCSharp", -1);
    MonoMethod* method = mono_class_get_method_from_name(scriptClass, "TestInterop", 1);
    MonoObject* instance = mono_object_new(mono_domain_get(), scriptClass);
    mono_runtime_object_init(instance);
    int input = 50;
    void* parameters = { &input }; // Passed parameters are an array of pointers
    MonoObject* exception = nullptr; // Exception gets stored in this object
    mono_runtime_invoke(method, instance, &parameters, &exception);
    if (exception) mono_print_unhandled_exception(exception);

    mono_runtime_invoke(scriptStartMethod, scriptInstance, nullptr, nullptr);
    mono_runtime_invoke(scriptStopMethod, scriptInstance, nullptr, nullptr);
    mono_runtime_invoke(scriptCallMethod, scriptInstance, nullptr, nullptr);
}

void CsScript::RunScript() {
    MonoMethod* method = mono_class_get_method_from_name(scriptClass, "Testing", -1);
    mono_runtime_invoke(method, scriptInstance, nullptr, nullptr);

    // Clear the key down afterwards
    GetKeyDownClear();
}

void CsScript::AddInternalCall(const char* functionName, const void* methodPointer) {
	mono_add_internal_call(functionName, methodPointer);
}

CsScript::~CsScript() {
    mono_jit_cleanup(domain);
}

//void InitMono()
//{
//    std::cout << "InitMono() called" << std::endl;
//	//std::cout << std::filesystem::current_path() << std::endl;
//    
//    // Get the current path but replace the end with the mono path
//    std::string filePath = std::filesystem::current_path().replace_filename("Extern\\Mono\\lib\\mono\\4.5").string();
//    //std::cout << filePath << std::endl;
//
//    mono_set_assemblies_path(filePath.c_str());
//    //mono_set_assemblies_path("C:\\Users\\wenyu\\OneDrive\\Documents\\GitHub\\HighOctane\\ZodiaClash\\Extern\\Mono\\lib\\mono\\4.5");
//    MonoDomain* rootDomain = mono_jit_init("MyScriptRuntime");
//}

//void LoadCSharpScript() {
//    MonoDomain* domain;
//    std::string filePath = std::filesystem::current_path().replace_filename("Extern\\Mono\\lib\\mono\\4.5").string();
//
//    mono_set_assemblies_path(filePath.c_str());
//    domain = mono_jit_init("testing");
//
//    const char* assemblyPath = "C:\\Users\\wenyu\\OneDrive\\Documents\\GitHub\\HighOctane\\ZodiaClash\\bin\\Debug-x64\\CSharpScript.dll";
//    MonoAssembly* assembly = mono_domain_assembly_open(domain, assemblyPath);
//
//    if (!assembly) {
//        std::cout << "Cannot open assembly" << std::endl;
//        return;
//    }
//
//    MonoImage* image = mono_assembly_get_image(assembly);
//
//    if (!image) {
//        std::cout << "Cannot get image" << std::endl;
//        return;
//    }
//
//    MonoClass* scriptClass = mono_class_from_name(image, "CSharpScript", "MyScriptClass");
//
//    if (!scriptClass) {
//        std::cout << "Cannot get class" << std::endl;
//        return;
//    }
//
//    MonoObject* scriptInstance = mono_object_new(domain, scriptClass);
//
//    if (!scriptInstance) {
//        std::cout << "Cannot create instance" << std::endl;
//        return;
//    }
//
//    mono_runtime_object_init(scriptInstance);
//
//    MonoMethod* scriptMethod = mono_class_get_method_from_name(scriptClass, "MyScriptMethod", -1);
//
//    if (!scriptMethod) {
//        std::cout << "Cannot get method" << std::endl;
//        return;
//    }
//
//    MonoObject* exc = nullptr; // Exception object
//
//    mono_runtime_invoke(scriptMethod, scriptInstance, nullptr, &exc);
//
//    if (exc) {
//        // Handle any exception that occurred in the C# method
//        MonoString* excMessage = mono_object_to_string(exc, nullptr);
//        const char* errorMessage = mono_string_to_utf8(excMessage);
//        std::cout << "Exception in MyScriptMethod: " << errorMessage << std::endl;
//        mono_free(nullptr);
//    }
//    else {
//        std::cout << "Invoked MyScriptMethod" << std::endl;
//    }
//
//    mono_jit_cleanup(domain);
//}

//void LoadCSharpScript()
//{
//
//    MonoDomain* domain;
//    std::string filePath = std::filesystem::current_path().replace_filename("Extern\\Mono\\lib\\mono\\4.5").string();
//    //std::cout << filePath << std::endl;
//
//    mono_set_assemblies_path(filePath.c_str());
//    domain = mono_jit_init("testing");
//    MonoAssembly* assembly;
//
//    
//    //const char* assemblyPath = "C:\\Users\\wenyu\\OneDrive\\Documents\\GitHub\\HighOctane\\ZodiaClash\\bin\\Debug-x64\\CSharpScript.dll";
//
//    // If debug mode
//    #if (ENABLE_DEBUG_DIAG)
//    // Relative path to the C# assembly
//    const char* relativeAssemblyPath = "\\Debug-x64\\HighOctane_CSharpScript.dll";
//    #elif (!ENABLE_DEBUG_DIAG)
//    const char* relativeAssemblyPath = "\\Release-x64\\HighOctane_CSharpScript.dll";
//    #endif
//
//    std::string fullAssemblyPath = std::filesystem::current_path().replace_filename("bin").string() + relativeAssemblyPath;
//    std::cout << fullAssemblyPath << std::endl;
//    assembly = mono_domain_assembly_open(domain, fullAssemblyPath.c_str());
//
//    if (!assembly)
//    {
//		std::cout << "Cannot open assembly" << std::endl;
//		return;
//	}
//
//    MonoImage* image;
//
//	image = mono_assembly_get_image(assembly);
//
//    if (!image)
//    {
//		std::cout << "Cannot get image" << std::endl;
//		return;
//	}
//    else {
//        std::cout << "Got image" << std::endl;
//    }
//
//    MonoClass* scriptClass;
//
//    scriptClass = mono_class_from_name(image, "CSharpScript", "MyScriptClass");
//
//    if (!scriptClass)
//    {
//        std::cout << "Cannot get class" << std::endl;
//        return;
//    }
//    else {
//        std::cout << "Got class" << std::endl;
//    }
//
//
//	MonoObject* scriptInstance;
//
//	scriptInstance = mono_object_new(domain, scriptClass);
//
//    if (!scriptInstance)
//    {
//		std::cout << "Cannot create instance" << std::endl;
//		return;
//	}
//    else {
//        std::cout << "Got instance" << std::endl;
//    }
//
//	mono_runtime_object_init(scriptInstance);
//
//    MonoMethod* scriptStartMethod;
//    MonoMethod* scriptStopMethod;
//
//    scriptStartMethod = mono_class_get_method_from_name(scriptClass, "Start", -1);
//    scriptStopMethod = mono_class_get_method_from_name(scriptClass, "Stop", -1);
//
//
//    mono_runtime_invoke(scriptStartMethod, scriptInstance, nullptr, nullptr);
//    mono_runtime_invoke(scriptStopMethod, scriptInstance, nullptr, nullptr);
// //   MonoMethod* scriptMethod;
// //   MonoMethod* testMethod;
//
//	//scriptMethod = mono_class_get_method_from_name(scriptClass, "MyScriptMethod", -1);
// //   testMethod = mono_class_get_method_from_name(scriptClass, "TestMethod", -1);
// //   if (!scriptMethod || !testMethod)
// //   {
//	//	std::cout << "Cannot get method" << std::endl;
//	//	return;
//	//}
// //   else {
// //       std::cout << "Got method" << std::endl;
// //   }
//
// //   mono_runtime_invoke(scriptMethod, scriptInstance, nullptr, nullptr);
// //   mono_runtime_invoke(testMethod, scriptInstance, nullptr, nullptr);
//
//    // This should be in the stopscript function next time
//	mono_jit_cleanup(domain);
//}