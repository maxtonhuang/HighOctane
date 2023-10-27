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

    static void EntityGetPosition(Entity entity, vmath::Vector2* outPosition) {

        // If the entity is 0, then it is not valid
        if (entity == 0) {
            return;
        }

        //vmath::Vector2 tmp = ECS::ecs().GetComponent<Transform>(entity).position;
        //*outTranslation = { tmp.x, tmp.y, 0.0f };
        *outPosition = ECS::ecs().GetComponent<Transform>(entity).position;
    }

    static void EntitySetPosition(Entity entity, vmath::Vector2* position) {

        // If the entity is 0, then it is not valid
        if (entity == 0) {
            return;
        }


        ECS::ecs().GetComponent<Transform>(entity).position = *position;
    }

    // This is to get the force
    static void EntityGetForce(Entity entity, vmath::Vector2* outForce) {
		// If the entity is 0, then it is not valid
        if (entity == 0) {
			return;
		}

		*outForce = ECS::ecs().GetComponent<Transform>(entity).force;
	}

    static void EntitySetForce(Entity entity, vmath::Vector2* forceAdd) {

        // If the entity is 0, then it is not valid
        if (entity == 0) {
            return;
        }
        ECS::ecs().GetComponent<Transform>(entity).force = *forceAdd;
    }

    static void EntityGetRotation(Entity entity, float* outRotation) {
		// If the entity is 0, then it is not valid
        if (entity == 0) {
			*outRotation = 0.0f;
		}

		*outRotation = ECS::ecs().GetComponent<Transform>(entity).rotation;
	}

    static void EntitySetRotation(Entity entity, float* rotationAdd) {

        // If the entity is 0, then it is not valid
        if (entity == 0) {
            return;
        }
        ECS::ecs().GetComponent<Transform>(entity).rotation = *rotationAdd;
    }

    static void EntityGetScale(Entity entity, float* outScale) {
        // If the entity is 0, then it is not valid
        if (entity == 0) {
            return;
        }

        *outScale = ECS::ecs().GetComponent<Transform>(entity).scale;

    }

    static void EntitySetScale(Entity entity, float* scaleAdd) {
        // If the entity is 0, then it is not valid
        if (entity == 0) {
            return;
        }

        ECS::ecs().GetComponent<Transform>(entity).scale = *scaleAdd;

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
    static bool EntityHasComponent(Entity entity, MonoReflectionType* componentName) {
        return ECS::ecs().HasComponent<MonoReflectionType*>(entity);
	}

    static void EntityAddComponent(Entity entity, MonoReflectionType* componentName) {
		ECS::ecs().AddComponent<MonoReflectionType*>(entity, componentName);
	}

    static void EntityRemoveComponent(Entity entity) {
        ECS::ecs().RemoveComponent<MonoReflectionType*>(entity);
    }

    static MonoReflectionType* EntityGetComponent(Entity entity) {
		return ECS::ecs().GetComponent<MonoReflectionType*>(entity);
	}

    // Entities

#define ADD_INTERNAL_CALL(name) mono_add_internal_call("InternalCalls::" #name, name);

    // This should be a class
    void AddInternalCall() {
        ADD_INTERNAL_CALL(Log);
        ADD_INTERNAL_CALL(GetKeyDown);
        ADD_INTERNAL_CALL(LogVector3);
        ADD_INTERNAL_CALL(GetAxisHorizontal);
        ADD_INTERNAL_CALL(GetAxisVertical);
        ADD_INTERNAL_CALL(EntityGetPosition);
        ADD_INTERNAL_CALL(EntitySetPosition);

        // Get the force
        ADD_INTERNAL_CALL(EntityGetForce);
        ADD_INTERNAL_CALL(EntitySetForce);

        // Get the rotation
        ADD_INTERNAL_CALL(EntityGetRotation);
        ADD_INTERNAL_CALL(EntitySetRotation);

        // Get the scale
        ADD_INTERNAL_CALL(EntityGetScale);
        ADD_INTERNAL_CALL(EntitySetScale);

        // Entities
        ADD_INTERNAL_CALL(EntityHasComponent);
        ADD_INTERNAL_CALL(EntityAddComponent);
        ADD_INTERNAL_CALL(EntityRemoveComponent);
        ADD_INTERNAL_CALL(EntityGetComponent);
        // Entities



    }

}



