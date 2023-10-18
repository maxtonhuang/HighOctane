#include "ScriptLinker.h"
#include "Scripting.h"

// This function is to log things into my console on C# side
void Log(MonoString* managedMessage)
{
    // Convert a C# string in to a C++ string
    char* message = mono_string_to_utf8(managedMessage);
    // Print message
    std::cout << message << std::endl;
}

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

#define ADD_INTERNAL_CALL(name) mono_add_internal_call("InternalCalls::" #name, name);

// This should be a class
void addInternalCalls() {
	ADD_INTERNAL_CALL(Log);
    ADD_INTERNAL_CALL(GetKeyDown);
}