#include "Scripting.h"
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
    //if (rootDomain == nullptr)
    //{
    //    // Maybe log some error here
    //    return;
    //}

    //// Store the root domain pointer
    //auto s_RootDomain = rootDomain;
}