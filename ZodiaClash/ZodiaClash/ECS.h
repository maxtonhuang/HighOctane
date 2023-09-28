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
*	@file		ECS.h
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
*	This file contains all the function declarations of the ECS Architecture.
*   The parts of the ECS Architecture include:
* 
*   [1] ENTITY -
*    -  Entity Manager - Manages the Entity IDs and keeps track of how
*                        many entities ther are currently.
*
*   [2] COMPONENT -
*    -  Component Array - Arrays that store data of each individual component
*                         (struct). Arrays must be packed without empty slots,
*                         and the Component Array achieves this by moving the
*                         last component in the array to an empty slot created
*                         by destroying a component.
*    -  Component Manager - Updates all the Component Arrays when a component
*                           is added or removed. Returns arrays and their
*                           corresponding data when needed.
* 
*   [3] SYSTEM -
*    -  System Manager - Maintains all registered systems and their
*                        corresponding system signatures. Adds the appropriate
*                        entities to each system's array of entities. If an
*                        entity's signature changes (components added or
*                        removed) or the entity is destroyed, the system's
*                        array of entities will be updated accordingly.
* 
*   [4] COORDINATOR
*    -  Coordinator - Acts as an interface to access and modify all the other
*                     managers in the ECS. It may slow down access so should
*                     only be used during non-time-critical updates such as
*                     initializatoin, or used sparingly in the game loops.
*
******************************************************************************/

#pragma once

#include <array>
#include <bitset>
#include <queue>
#include <unordered_map>
#include <set>
#include <memory>
#include "debugdiagnostic.h"
#include "Components.h"

using Entity = std::uint32_t;

// Maximum number of entities
const Entity MAX_ENTITIES = 1'000'000;

using ComponentType = std::uint8_t;

// Maximum number of components
const ComponentType MAX_COMPONENTS = 64;

using Signature = std::bitset<MAX_COMPONENTS>;


////////// ENTITY /////////////////////////////////////////////////////////////

class EntityManager {
public:
    EntityManager();

    Entity CreateEntity();

    void DestroyEntity(Entity entity);

    void SetSignature(Entity entity, Signature signature);

    Signature GetSignature(Entity entity);

    uint32_t GetEntityCount() {
        return m_LivingEntityCount;
    }

private:
    // Queue of unused entity IDs
    std::queue<Entity> m_AvailableEntities{};

    // Array of signatures where the index corresponds to the entity ID
    std::array<Signature, MAX_ENTITIES> m_Signatures{};

    // Total living entities - used to keep limits on how many exist
    uint32_t m_LivingEntityCount{};
};


////////// COMPONENT //////////////////////////////////////////////////////////

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
};


template<typename T>
class ComponentArray : public IComponentArray {
public:
    void InsertData(Entity entity, T component);

    void RemoveData(Entity entity);
        
    T& GetData(Entity entity);

    void EntityDestroyed(Entity entity) override;

private:
    // The packed array of components (of generic type T),
    // set to a specified maximum amount, matching the maximum number
    // of entities allowed to exist simultaneously, so that each entity
    // has a unique spot.
    std::array<T, MAX_ENTITIES> m_ComponentArray;

    // Map from an entity ID to an array index.
    std::unordered_map<Entity, size_t> m_EntityToIndexMap;

    // Map from an array index to an entity ID.
    std::unordered_map<size_t, Entity> m_IndexToEntityMap;

    // Total size of valid entries in the array.
    size_t m_Size;
};


class ComponentManager {
public:
    template<typename T>
    void RegisterComponent();

    template<typename T>
    ComponentType GetComponentType();

    template<typename T>
    void AddComponent(Entity entity, T component);

    template<typename T>
    void RemoveComponent(Entity entity);

    template<typename T>
    T& GetComponent(Entity entity);

    void EntityDestroyed(Entity entity);

    template<typename T>
    bool isComponentTypeRegistered(); // new

    template<typename T>
    ComponentArray<T>& GetComponentArrayRef();


private:
    // Map from type string pointer to a component type
    std::unordered_map<const char*, ComponentType> m_ComponentTypes{};

    // Map from type string pointer to a component array
    std::unordered_map<const char*, std::shared_ptr<IComponentArray>> m_ComponentArrays{};

    // The component type to be assigned to the next registered component - starting at 0
    ComponentType m_NextComponentType{};

    // Convenience function to get the statically casted pointer to the ComponentArray of type T.
    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray();
};


////////// SYSTEM /////////////////////////////////////////////////////////////

class System {
public:
    virtual void Update() = 0;
    std::set<Entity> m_Entities;
};


class SystemManager {
public:
    template<typename T>
    std::shared_ptr<T> RegisterSystem();

    template<typename T>
    void SetSignature(Signature signature);

    void EntityDestroyed(Entity entity);

    void EntitySignatureChanged(Entity entity, Signature entitySignature);

private:
    // Map from system type string pointer to a signature
    std::unordered_map<const char*, Signature> m_Signatures{};

    // Map from system type string pointer to a system pointer
    std::unordered_map<const char*, std::shared_ptr<System>> m_Systems{};
};


////////// ECS COORDINATOR ////////////////////////////////////////////////////

class ECS {
public:
    void Init();

    // Disallow copying to prevent creation of more than one instance
    ECS(const ECS &) = delete;
    ECS & operator=(const ECS &) = delete;

    // Public accessor for the Singleton instance
    static ECS & ecs() {
        static ECS entityComponentSystem;
        return entityComponentSystem;
    }

    // Entity methods ---------------------------------------------------------
    Entity CreateEntity();

    void DestroyEntity(Entity entity);

    template<typename T>
    bool HasComponent(Entity entity);
    
    uint32_t GetEntityCount();

    // Component methods ------------------------------------------------------
    template<typename T>
    void RegisterComponent();

    template<typename T>
    void AddComponent(Entity entity, T component);

    template<typename T>
    void RemoveComponent(Entity entity);

    template<typename T>
    T& GetComponent(Entity entity);

    template<typename T>
    ComponentType GetComponentType();

    template<typename T>
    bool isComponentTypeRegistered();

    ComponentManager& GetComponentManager();

    // System methods ---------------------------------------------------------
    template<typename T>
    std::shared_ptr<T> RegisterSystem();

    template<typename T>
    std::shared_ptr<T> RegisterSystem(T& input);

    template<typename T>
    void SetSystemSignature(Signature signature);

private:

    ECS() {}

    std::unique_ptr<ComponentManager> m_ComponentManager;
    std::unique_ptr<EntityManager> m_EntityManager;
    std::unique_ptr<SystemManager> m_SystemManager;
};

#include "ECS.hpp" // for template functions of ECS



////////// System Declarations ////////////////////////////////////////////////

class PhysicsSystem : public System {
public:
    void Update() override;
};

class CollisionSystem : public System {
public:
    void Update() override;
};

class MovementSystem : public System {
public:
    void Update() override;
};

class ModelSystem : public System {
public:
    void Update() override;
};

class GraphicsSystem : public System {
public:
    void Initialize();
    void Update() override;
};

class FontSystem : public System {
public:
    void Initialize();
    void Update() override;
};

class SerializationSystem : public System {
public:
    void Update() override;
};