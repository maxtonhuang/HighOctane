#pragma once
/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		InternalCalls.h
*
*	@author		Koh Wen Yuan
*
*	@email		k.wenyuan\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		20 October 2023
*
* *****************************************************************************
*
*	@brief		Internal calls to be linked with c# side
*
*	This file contains all the definition of the functions to be linked to
*   C# side. This is to be used with the scripting system.
******************************************************************************/

#include "Scripting.h"
#include "ScriptEngine.h"
#include "vmath.h"


namespace internalcalls {

    static void DebugTrace(MonoString* message) {
        char* msg = mono_string_to_utf8(message);
        LOG_TRACE(msg);
        mono_free(msg);
    }

    static void DebugInfo(MonoString* message) {
		char* msg = mono_string_to_utf8(message);
		LOG_INFO(msg);
        mono_free(msg);
	}

    static void DebugWarning(MonoString* message) {
        char* msg = mono_string_to_utf8(message);
        LOG_WARNING(msg);
        mono_free(msg);
    }

    static void DebugError(MonoString* message) {
		char* msg = mono_string_to_utf8(message);
		LOG_ERROR(msg);
        mono_free(msg);
	}

    static void DebugFatal(MonoString* message) {
		char* msg = mono_string_to_utf8(message);
		LOG_FATAL(msg);
        mono_free(msg);
	}

    /*!
    * \brief Logging function
    *
    * This function is responsible for logging the message from C# side
    *
    * \param managedMessage The message to be logged
    */
    static void Log(MonoString* message)
    {
        // Convert a C# string in to a C++ string
        char* msg = mono_string_to_utf8(message);
        // Print message

        LOG_DEBUG(msg);
        mono_free(msg);
    }


    /*!
    * \brief Logging function for vector3 type
    *
    * This function is responsible for logging the message from C# side
    *
    * \param param The message to be logged
    * \param result The message to be logged
    */
    static void LogVector3(vmath::Vector3* param, vmath::Vector3* result)
    {
        std::cout << *param << std::endl;

        *result = param->negate();
    }


    /*!
    * \brief GetKeyDown function
    *
    * This function is responsible for checking if the key is down
    *
    * \param key The key to be checked
    *
    * \return True if the key is down, false otherwise
    */
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

    /*!
    * \brief GetAxisHorizontal function
    *
    * This function is responsible for checking if the keys to
    * go left and right are pressed
    *
    * \param key The key to be checked
    *
    * \return int -1 if left is pressed, 1 if right is pressed, 0 otherwise
    */
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


    /*!
    * \brief GetAxisVertical function
    *
    * This function is responsible for checking if the keys to
    * go up and down are pressed
    *
    * \param key The key to be checked
    *
    * \return int -1 if down is pressed, 1 if up is pressed, 0 otherwise
    */
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

    /*!
    * \brief Gets the position of an entity.
    *
    * This function retrieves the position of the specified entity and stores it in the provided vector.
    *
    * \param entity The entity to query for position.
    * \param outPosition Pointer to a vmath::Vector2 where the position will be stored.
    */
    static void EntityGetPosition(Entity entity, vmath::Vector2* outPosition) {

        // If the entity is 0, then it is not valid
        if (entity == 0) {
            return;
        }

        //vmath::Vector2 tmp = ECS::ecs().GetComponent<Transform>(entity).position;
        //*outTranslation = { tmp.x, tmp.y, 0.0f };
        *outPosition = ECS::ecs().GetComponent<Transform>(entity).position;
    }

    /*!
    * \brief Sets the position of an entity.
    *
    * This function updates the position of the specified entity with the provided position.
    *
    * \param entity The entity to update.
    * \param position Pointer to a vmath::Vector2 containing the new position.
    */
    static void EntitySetPosition(Entity entity, vmath::Vector2* position) {

        // If the entity is 0, then it is not valid
        if (entity == 0) {
            return;
        }


        ECS::ecs().GetComponent<Transform>(entity).position = *position;
    }

    /*!
    * \brief Gets the force of an entity.
    *
    * This function retrieves the force acting on the specified entity and stores it in the provided vector.
    *
    * \param entity The entity to query for force.
    * \param outForce Pointer to a vmath::Vector2 where the force will be stored.
    */
    // This is to get the force
    static void EntityGetForce(Entity entity, vmath::Vector2* outForce) {
        // If the entity is 0, then it is not valid
        if (entity == 0) {
            return;
        }

        *outForce = ECS::ecs().GetComponent<Transform>(entity).force;
    }

    /*!
    * \brief Sets the force of an entity.
    *
    * This function updates the force acting on the specified entity with the provided force.
    *
    * \param entity The entity to update.
    * \param forceAdd Pointer to a vmath::Vector2 containing the force to be set.
    */
    static void EntitySetForce(Entity entity, vmath::Vector2* forceAdd) {

        // If the entity is 0, then it is not valid
        if (entity == 0) {
            return;
        }
        ECS::ecs().GetComponent<Transform>(entity).force = *forceAdd;
    }

    /*!
    * \brief Gets the rotation of an entity.
    *
    * This function retrieves the rotation of the specified entity and stores it in the provided float.
    *
    * \param entity The entity to query for rotation.
    * \param outRotation Pointer to a float where the rotation will be stored.
    */
    static void EntityGetRotation(Entity entity, float* outRotation) {
        // If the entity is 0, then it is not valid
        if (entity == 0) {
            *outRotation = 0.0f;
        }

        *outRotation = ECS::ecs().GetComponent<Transform>(entity).rotation;
    }

    /*!
    * \brief Sets the rotation of an entity.
    *
    * This function updates the rotation of the specified entity with the provided rotation.
    *
    * \param entity The entity to update.
    * \param rotationAdd Pointer to a float containing the rotation to be set.
    */
    static void EntitySetRotation(Entity entity, float* rotationAdd) {

        // If the entity is 0, then it is not valid
        if (entity == 0) {
            return;
        }
        ECS::ecs().GetComponent<Transform>(entity).rotation = *rotationAdd;
    }

    /*!
    * \brief Gets the scale of an entity.
    *
    * This function retrieves the scale of the specified entity and stores it in the provided float.
    *
    * \param entity The entity to query for scale.
    * \param outScale Pointer to a float where the scale will be stored.
    */
    static void EntityGetScale(Entity entity, float* outScale) {
        // If the entity is 0, then it is not valid
        if (entity == 0) {
            return;
        }

        *outScale = ECS::ecs().GetComponent<Transform>(entity).scale;

    }

    /*!
    * \brief Sets the scale of an entity.
    *
    * This function updates the scale of the specified entity with the provided scale.
    *
    * \param entity The entity to update.
    * \param scaleAdd Pointer to a float containing the scale to be set.
    */
    static void EntitySetScale(Entity entity, float* scaleAdd) {
        // If the entity is 0, then it is not valid
        if (entity == 0) {
            return;
        }

        ECS::ecs().GetComponent<Transform>(entity).scale = *scaleAdd;

    }

    /*!
    * \brief Gets the delta time.
    *
    * This function retrieves the delta time and stores it in the provided float.
    *
    * \param outScale Pointer to a float where the delta time will be stored.
    */
    static void GetDeltaTime(float* outDeltaTime) {
        *outDeltaTime = g_dt;
    }


    static void GetFixedDeltaTime(float* outFixedDeltaTime) {
        // FIXED_DT is now 1.0f / 60.0f
        *outFixedDeltaTime = FIXED_DT;
    }

    /*!
    * \brief Checks if an entity has a specific component.
    *
    * This function checks if the specified entity has a particular component.
    *
    * \param entity The entity to check.
    * \param componentName The name or type of the component to be checked.
    * \return true if the entity has the component, false otherwise.
    */
    static bool EntityHasComponent(Entity entity) {
        return ECS::ecs().HasComponent<MonoReflectionType*>(entity);
    }

    /*!
    * \brief Adds a component to an entity.
    *
    * This function adds a component of a specified type to the provided entity.
    *
    * \param entity The entity to add the component to.
    * \param componentName The name or type of the component to be added.
    */
    static void EntityAddComponent(Entity entity, MonoReflectionType* componentName) {
        ECS::ecs().AddComponent<MonoReflectionType*>(entity, componentName);
    }

    /*!
    * \brief Removes a component from an entity.
    *
    * This function removes a component from the specified entity.
    *
    * \param entity The entity to remove the component from.
    */
    static void EntityRemoveComponent(Entity entity) {
        ECS::ecs().RemoveComponent<MonoReflectionType*>(entity);
    }

    /*!
    * \brief Gets a component from an entity.
    *
    * This function retrieves a component from the specified entity.
    *
    * \param entity The entity to get the component from.
    * \return The component associated with the entity.
    */
    static MonoReflectionType* EntityGetComponent(Entity entity) {
        return ECS::ecs().GetComponent<MonoReflectionType*>(entity);
    }


    /*!
    * \brief Adds internal calls for various methods.
    *
    * This method adds internal calls to corresponding functions for use with mono_add_internal_call.
    * It associates the provided methods with their respective names for internal calls.
    */
#define ADD_INTERNAL_CALL(name) mono_add_internal_call("InternalCalls::" #name, name);

    void AddInternalCall() {

        // Logging
        ADD_INTERNAL_CALL(Log);
        ADD_INTERNAL_CALL(DebugTrace);
        ADD_INTERNAL_CALL(DebugInfo);
        ADD_INTERNAL_CALL(DebugWarning);
        ADD_INTERNAL_CALL(DebugError);
        ADD_INTERNAL_CALL(DebugFatal);

        ADD_INTERNAL_CALL(GetKeyDown);
        ADD_INTERNAL_CALL(LogVector3);

        // Get the axis
        ADD_INTERNAL_CALL(GetAxisHorizontal);
        ADD_INTERNAL_CALL(GetAxisVertical);

        // Get the position
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

        // Get the delta time
        ADD_INTERNAL_CALL(GetDeltaTime);
        ADD_INTERNAL_CALL(GetFixedDeltaTime);

        // Entities
        ADD_INTERNAL_CALL(EntityHasComponent);
        ADD_INTERNAL_CALL(EntityAddComponent);
        ADD_INTERNAL_CALL(EntityRemoveComponent);
        ADD_INTERNAL_CALL(EntityGetComponent);
        // Entities



    }

}



