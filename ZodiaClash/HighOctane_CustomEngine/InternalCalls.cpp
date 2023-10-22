#include "ScriptLinker.h"
#include "Scripting.h"

#include "ScriptEngine.h"
#include "vmath.h"

// Change this to a class in the future, change it to scriptglue.cpp, put the functions in there and namespace InternalCalls::

namespace internalcalls {

    // This function is to log things into my console on C# side
    static void Log(MonoString* managedMessage)
    {
        // Convert a C# string in to a C++ string
        char* message = mono_string_to_utf8(managedMessage);
        // Print message

        mono_free(message);
        std::cout << message << std::endl;
    }

    // This function is an example of the vectors
    static void LogVector3(vmath::Vector3* param, vmath::Vector3* result)
    {
        std::cout << *param << std::endl;

        *result = param->negate();
    }


    // This function is to check whether the key is pressed or not on C# side
    static bool GetKeyDown(INFO key) {
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
        ADD_INTERNAL_CALL(LogVector3);
    }

}



