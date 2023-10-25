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
            case TYPE::KEY_DOWN:
                if (msg.info == key) {
                    return true;
                }
                break;
            }
        }
        return false;
    }

    // This function is to get the horizontal axis on C# side
    static int GetAxisHorizontal() {
        for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::SCRIPTING]) {
            switch (msg.type) {
			case TYPE::KEY_DOWN:
                if (msg.info == INFO::KEY_A || msg.info == INFO::KEY_LEFT) {
					return -1;
				}
                else if (msg.info == INFO::KEY_D || msg.info == INFO::KEY_RIGHT) {
					return 1;
				}
				break;
			}
		}
		return 0;
	}

    static void TransformGetTranslation(Entity entity, vmath::Vector2* outTranslation)
    {
        if (entity == 0) {
            return;
        }

        *outTranslation = ECS::ecs().GetComponent<Transform>(entity).position;
    }

    static void TransformSetTranslation(Entity entity, vmath::Vector2* translation)
    {
        if (entity == 0) {
            return;
        }
        ECS::ecs().GetComponent<Transform>(entity).position = *translation;
    }


    // This function is to get the vertical axis on C# side
    static int GetAxisVertical() {
        for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::SCRIPTING]) {
            switch (msg.type) {
            case TYPE::KEY_DOWN:
                if (msg.info == INFO::KEY_S || msg.info == INFO::KEY_DOWN) {
                    return -1;
                }
                else if (msg.info == INFO::KEY_W || msg.info == INFO::KEY_UP) {
                    return 1;
                }
                break;
            }
        }
        return 0;
    }

    // Entities
    template<typename T>
    static bool EntityHasComponent(/*Entity entity, */T componentName) {
        return ECS::ecs().HasComponent<T>(0/*entity*/);
	}


#define ADD_INTERNAL_CALL(name) mono_add_internal_call("InternalCalls::" #name, name);

    // This should be a class
    void AddInternalCall() {
        ADD_INTERNAL_CALL(Log);
        ADD_INTERNAL_CALL(GetKeyDown);
        ADD_INTERNAL_CALL(LogVector3);
        ADD_INTERNAL_CALL(GetAxisHorizontal);
        ADD_INTERNAL_CALL(GetAxisVertical);
        ADD_INTERNAL_CALL(TransformGetTranslation);
        ADD_INTERNAL_CALL(TransformSetTranslation);

    }

}



