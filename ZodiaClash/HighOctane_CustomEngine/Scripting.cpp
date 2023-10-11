#include "Scripting.h"

CsScript::CsScript() {
    std::cout << "this is working\n";
    std::string filePath = std::filesystem::current_path().replace_filename("Extern\\Mono\\lib\\mono\\4.5").string();
    //std::cout << filePath << std::endl;

    mono_set_assemblies_path(filePath.c_str());
    domain = mono_jit_init("testing");

    if (domain) {
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

        if (assembly) {
            image = mono_assembly_get_image(assembly);

            if (image) {
                scriptClass = mono_class_from_name(image, "CSharpScript", "MyScriptClass");
                
                if (scriptClass) {
                    scriptInstance = mono_object_new(domain, scriptClass);
                    mono_runtime_object_init(scriptInstance);

                    MonoMethod* scriptStartMethod = mono_class_get_method_from_name(scriptClass, "Start", -1);
                    MonoMethod* scriptStopMethod = mono_class_get_method_from_name(scriptClass, "Stop", -1);
                    MonoMethod* scriptTestMethod = mono_class_get_method_from_name(scriptClass, "StringTest", -1);


                    // Calling the function
                    mono_runtime_invoke(scriptStartMethod, scriptInstance, nullptr, nullptr);
                    mono_runtime_invoke(scriptStopMethod, scriptInstance, nullptr, nullptr);
                    MonoObject* result = mono_runtime_invoke(scriptTestMethod, scriptInstance, nullptr, nullptr);
                    /*-----------THIS IS A STUPID WAY TO GET THE STRING IN THE CONSOLE-----------*/
                    // Convert the result to a C string
                    const char* bufferData = mono_string_to_utf8((MonoString*)result);
                    // Print or use the bufferData
                    std::cout << "Buffer Data from C#: " << bufferData << std::endl;
                    //printf("Buffer Data from C#: %s\n", bufferData);
                    // Free the memory used by the C string
                    mono_free((char*)bufferData);
                    /*-----------THIS IS A STUPID WAY TO GET THE STRING IN THE CONSOLE-----------*/


                    //mono_runtime_invoke(scriptTestMethod, scriptInstance, nullptr, nullptr);
                
                }
                // Else for scriptClass
                else {
					std::cout << "ScriptClass is null" << std::endl;
				}
            }
            // Else for image
            else {
                std::cout << "Image is null" << std::endl;
            }
        }
        // Else for assembly
        else {
            std::cout << "Assembly is null" << std::endl;
        }
    }
    // Else for domain
    else {
        std::cout << "Domain is null" << std::endl;
    }


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