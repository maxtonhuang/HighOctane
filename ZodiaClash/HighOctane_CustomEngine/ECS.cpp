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
*	@file		ECS.cpp
*
*	@author		Maxton Huang Xinghua
*
*	@email		m.huang\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		22 September 2023
*
* *****************************************************************************
*
*	@brief		The underlying ECS Architecture of the game
*
*	This file contains the non-templte definitions of all the different
*   functions of the ECS. For more information, please refer to "ECS.h".
*
******************************************************************************/


#include "ECS.h"
#include "Components.h"

///////////////////////////////////////////////////////////////////////////
////////// ENTITY /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

EntityManager::EntityManager() {
    // Initialize the queue with all possible entity IDs
    for (Entity entity = 1; entity < MAX_ENTITIES; ++entity) {
        m_AvailableEntities.push(entity);
    }
}

Entity EntityManager::CreateEntity() {
    ASSERT(m_LivingEntityCount >= MAX_ENTITIES, "Too many entities in existence.");

    // Take an ID from the front of the queue
    Entity id = m_AvailableEntities.front();
    m_AvailableEntities.pop();
    m_ExistingEntities.insert(id);
    ++m_LivingEntityCount;

    return id;
}

void EntityManager::DestroyEntity(Entity entity) {
    ASSERT(entity >= MAX_ENTITIES, "Entity out of range.");

    // Invalidate the destroyed entity's signature
    m_Signatures[entity].reset();

    // Put the destroyed ID at the back of the queue
    m_AvailableEntities.push(entity);
    m_ExistingEntities.erase(entity);
    --m_LivingEntityCount;
}

void EntityManager::SetSignature(Entity entity, Signature signature) {
    ASSERT(entity >= MAX_ENTITIES, "Entity out of range.");

    // Put this entity's signature into the array
    m_Signatures[entity] = signature;
}

Signature EntityManager::GetSignature(Entity entity) {
    ASSERT(entity >= MAX_ENTITIES, "Entity out of range.");

    // Get this entity's signature from the array
    return m_Signatures[entity];
}

bool EntityManager::EntityExists(Entity entity) {
    return (bool)(m_ExistingEntities.count(entity));
}


///////////////////////////////////////////////////////////////////////////
////////// COMPONENT //////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

// ---------- Component Manager ---------- //

void ComponentManager::EntityDestroyed(Entity entity) {
    // Notify each component array that an entity has been destroyed
    // If it has a component for that entity, it will remove it
    for (std::pair<const char*, std::shared_ptr<IComponentArray>> const& pair : m_ComponentArrays) {
        std::shared_ptr<IComponentArray> const& component = pair.second;

        component->EntityDestroyed(entity);
    }
}




///////////////////////////////////////////////////////////////////////////
////////// SYSTEM /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void SystemManager::EntityDestroyed(Entity entity) {
    // Erase a destroyed entity from all system lists
    // m_Entities is a set so no check needed
    for (std::pair<const char*, std::shared_ptr<System>> const& pair : m_Systems) {
        std::shared_ptr<System> const& system = pair.second;

        system->m_Entities.erase(entity);
    }
}

void SystemManager::EntitySignatureChanged(Entity entity, Signature entitySignature) {
    // Notify each system that an entity's signature changed
    for (std::pair<const char*, std::shared_ptr<System>> const& pair : m_Systems) {
        const char* const& type = pair.first;
        std::shared_ptr<System> const& system = pair.second;
        Signature const& systemSignature = m_Signatures[type];

        // Entity signature matches system signature - insert into set
        if ((entitySignature & systemSignature) == systemSignature) {
            system->m_Entities.insert(entity);
        }
        // Entity signature does not match system signature - erase from set
        else {
            system->m_Entities.erase(entity);
        }
    }
}



///////////////////////////////////////////////////////////////////////////
////////// ECS COORDINATOR ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void ECS::Init() {
    // Create pointers to each manager
    m_ComponentManager = std::make_unique<ComponentManager>();
    m_EntityManager = std::make_unique<EntityManager>();
    m_SystemManager = std::make_unique<SystemManager>();
}

// Entity methods
Entity ECS::CreateEntity() {
    return m_EntityManager->CreateEntity();
}

// Returns the total number of Entities existing
uint32_t ECS::GetEntityCount() {
    return m_EntityManager->GetEntityCount();
}

// Return pointer to Component Manager
// to access component functions and arrays
// for faster access to large amounts of data
ComponentManager& ECS::GetComponentManager() {
    return *m_ComponentManager;
}

std::unordered_map<std::string, std::shared_ptr<ComponentFunctions>>& ECS::GetTypeManager() {
    return m_TypeManager;
}

// Destroys the Entity and updates the corresponding arrays
void ECS::DestroyEntity(Entity entity) {
    m_EntityManager->DestroyEntity(entity);

    m_ComponentManager->EntityDestroyed(entity);

    m_SystemManager->EntityDestroyed(entity);
}
