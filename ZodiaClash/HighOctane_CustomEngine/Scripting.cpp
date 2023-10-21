//#include "Scripting.h"
//
//// String contains function this one is for oliver in the future
////bool StringContains(std::string const& fullString, std::string const& subString) {
////    if (fullString.find(subString) != std::string::npos) {
////		return true;
////	}
////    else {
////		return false;
////	}
////}
//
////#define MyFunctions _declspec(dllexport)
////
////extern "C" {
////    MyFunctions int AddNumbers(int a, int b) {
////        return a + b;
////    }
////
////    MyFunctions int SubtractNumbers(int a, int b) {
////		return a - b;
////	}
////}
//
////CsScript script;
//
//
//
//// This function is to clear the key pressed but should be in c++ side
//void GetKeyDownClear() {
//    Mail::mail().mailbox[ADDRESS::SCRIPTING].clear();
//}
//
//// This function is to set active like unity
////void SetActive(bool active) {
////	//std::cout << "Set active called" << std::endl;
////	//std::cout << "Active: " << active << std::endl;
////	//std::cout << "Script instance: " << script.scriptInstance << std::endl;
////	MonoMethod* method = mono_class_get_method_from_name(script.scriptClass, "SetActive", 1);
////	void* parameters = { &active }; // Passed parameters are an array of pointers
////	MonoObject* exception = nullptr; // Exception gets stored in this object
////	mono_runtime_invoke(method, script.scriptInstance, &parameters, &exception);
////	if (exception) mono_print_unhandled_exception(exception);
////}
//
//void testPrintFunction() {
//    std::cout << "this is a test" << std::endl;
//}
//
//
//
////bool testFunc() {
////
////}
////CsScript::CsScript() {
////
////}
//
//// 1 script class, stores the class itself
//// class instance store the instance
//
//CsScript::CsScript() {
//    // This part should be ran one time in the whole engine
//    std::cout << "Scripting intialized\n";
//    std::string filePath = std::filesystem::current_path().replace_filename("Extern\\Mono\\lib\\mono\\4.5").string();
//    //std::cout << filePath << std::endl;
//
//
//    mono_set_assemblies_path(filePath.c_str());
//    domain = mono_jit_init(filePath.c_str());
//     //This part should be ran one time in the whole engine
//
//
//    // This should be moved somewhere else eventually
//    addInternalCalls();
//    // This should be moved somewhere else eventually
//
//    if (!domain) {
//		std::cout << "Domain is null" << std::endl;
//		return;
//	}
//    // If debug mode
//    #if (ENABLE_DEBUG_DIAG)
//    // Relative path to the C# assembly
//        const char* relativeAssemblyPath = "\\Debug-x64\\HighOctane_CSharpScript.dll";
//    #elif (!ENABLE_DEBUG_DIAG)
//        const char* relativeAssemblyPath = "\\Release-x64\\HighOctane_CSharpScript.dll";
//    #endif
//
//    std::string fullAssemblyPath = std::filesystem::current_path().replace_filename("bin").string() + relativeAssemblyPath;
//    //std::cout << fullAssemblyPath << std::endl;
//    assembly = mono_domain_assembly_open(domain, fullAssemblyPath.c_str());
//
//    if (!assembly) {
//		std::cout << "Cannot open assembly" << std::endl;
//		return;
//	}
//
//    image = mono_assembly_get_image(assembly);
//
//    scriptClass = mono_class_from_name(image, "", "MyScriptClass");
//    scriptInstance = mono_object_new(domain, scriptClass);
//    mono_runtime_object_init(scriptInstance);
//
//    //MonoMethod* scriptStartMethod = mono_class_get_method_from_name(scriptClass, "Start", -1);
//    //MonoMethod* scriptStopMethod = mono_class_get_method_from_name(scriptClass, "Stop", -1);
//    //MonoMethod* scriptCallMethod = mono_class_get_method_from_name(scriptClass, "PrintFromCSharp", -1);
//
//    // Script class will store this
//    MonoMethod* method = mono_class_get_method_from_name(scriptClass, "TestInterop", 1);
//
//    // Script class instance will store this
//    MonoObject* instance = mono_object_new(mono_domain_get(), scriptClass);
//
//
//    mono_runtime_object_init(instance);
//    int input = 50;
//    void* parameters = { &input }; // Passed parameters are an array of pointers
//    MonoObject* exception = nullptr; // Exception gets stored in this object
//    mono_runtime_invoke(method, instance, &parameters, &exception);
//    if (exception) mono_print_unhandled_exception(exception);
//
//    //mono_runtime_invoke(scriptStartMethod, scriptInstance, nullptr, nullptr);
//    //mono_runtime_invoke(scriptStopMethod, scriptInstance, nullptr, nullptr);
//    //mono_runtime_invoke(scriptCallMethod, scriptInstance, nullptr, nullptr);
//}
//
//void CsScript::RunScript(Screen *s) {
//
//    //if (s->isActive) {
//    //    // Show pause screen.
//    //}
//    //else {
//    //    // Do nothing.
//    //}
//
//
//    MonoMethod* method = mono_class_get_method_from_name(scriptClass, "Testing", -1);
//    mono_runtime_invoke(method, scriptInstance, nullptr, nullptr);
//
//    // Clear the key down afterwards
//    GetKeyDownClear();
//}
//
//
//CsScript::~CsScript() {
//    mono_jit_cleanup(domain);
//}
