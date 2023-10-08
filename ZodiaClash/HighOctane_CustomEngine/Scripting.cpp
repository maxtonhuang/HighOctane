#include "Scripting.h"
// TO DO: NEED TO COPY THE DLL INTO THE DEBUG FOLDER IN THE POST BUILD I THINK


void InitMono()
{
    std::cout << "InitMono() called" << std::endl;
	//std::cout << std::filesystem::current_path() << std::endl;
    
    // Get the current path but replace the end with the mono path
    std::string filePath = std::filesystem::current_path().replace_filename("Extern\\Mono\\lib\\mono\\4.5").string();
    //std::cout << filePath << std::endl;

    mono_set_assemblies_path(filePath.c_str());
    //mono_set_assemblies_path("C:\\Users\\wenyu\\OneDrive\\Documents\\GitHub\\HighOctane\\ZodiaClash\\Extern\\Mono\\lib\\mono\\4.5");
    MonoDomain* rootDomain = mono_jit_init("MyScriptRuntime");
}

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

void LoadCSharpScript()
{

    MonoDomain* domain;
    std::string filePath = std::filesystem::current_path().replace_filename("Extern\\Mono\\lib\\mono\\4.5").string();
    //std::cout << filePath << std::endl;

    mono_set_assemblies_path(filePath.c_str());
    domain = mono_jit_init("testing");
    MonoAssembly* assembly;


    const char* assemblyPath = "C:\\Users\\wenyu\\OneDrive\\Documents\\GitHub\\HighOctane\\ZodiaClash\\bin\\Debug-x64\\CSharpScript.dll";
    assembly = mono_domain_assembly_open(domain, assemblyPath);

    if (!assembly)
    {
		std::cout << "Cannot open assembly" << std::endl;
		return;
	}

    MonoImage* image;

	image = mono_assembly_get_image(assembly);

    if (!image)
    {
		std::cout << "Cannot get image" << std::endl;
		return;
	}
    else {
        std::cout << "Got image" << std::endl;
    }

    MonoClass* scriptClass;

    scriptClass = mono_class_from_name(image, "CSharpScript", "MyScriptClass");

    if (!scriptClass)
    {
        std::cout << "Cannot get class" << std::endl;
        return;
    }
    else {
        std::cout << "Got class" << std::endl;
    }


	MonoObject* scriptInstance;

	scriptInstance = mono_object_new(domain, scriptClass);

    if (!scriptInstance)
    {
		std::cout << "Cannot create instance" << std::endl;
		return;
	}
    else {
        std::cout << "Got instance" << std::endl;
    }

	mono_runtime_object_init(scriptInstance);

	MonoMethod* scriptMethod;

	scriptMethod = mono_class_get_method_from_name(scriptClass, "MyScriptMethod", -1);

    if (!scriptMethod)
    {
		std::cout << "Cannot get method" << std::endl;
		return;
	}
    else {
        std::cout << "Got method" << std::endl;
    }

    mono_runtime_invoke(scriptMethod, scriptInstance, nullptr, nullptr);

	mono_jit_cleanup(domain);
}