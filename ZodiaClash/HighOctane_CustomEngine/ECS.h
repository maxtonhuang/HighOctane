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
*   @co-author	Foong Pun Yuen Nigel (classes ComponentFunctions and IComponentFunctions)
*
*	@email		p.foong\@digipen.edu
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
#include <functional>
#include <algorithm>
#include "debugdiagnostic.h"
#include "Components.h"
#include "MemoryManager.h"


using Entity = std::uint32_t;

// Maximum number of entities
const Entity MAX_ENTITIES = 100'000;

using ComponentType = std::uint8_t;

// Maximum number of components
const ComponentType MAX_COMPONENTS = 64;

using Signature = std::bitset<MAX_COMPONENTS>;


////////// ENTITY /////////////////////////////////////////////////////////////
class EntityManager {
public:
    // Constructor
    EntityManager();

    // Creates a new entity
    Entity CreateEntity();

    // Destroys an entity
    void DestroyEntity(Entity entity);

    // Adds a component to an entity
    void SetSignature(Entity entity, Signature signature);

    // Gets the signature of an entity
    Signature GetSignature(Entity entity);

    // Get the number of living entities
    uint32_t GetEntityCount() {
        return m_LivingEntityCount;
    }

    // Returns true if entity exists
    bool EntityExists(Entity entity);

private:
    // Queue of unused entity IDs
    std::queue<Entity> m_AvailableEntities{};

    std::set<Entity> m_ExistingEntities{};

    // Array of signatures where the index corresponds to the entity ID
    std::array<Signature, MAX_ENTITIES> m_Signatures{};

    // Total living entities - used to keep limits on how many exist
    uint32_t m_LivingEntityCount{};
};


////////// COMPONENT //////////////////////////////////////////////////////////
class ComponentFunctions {
public:
    //Adds component to the current entity
    virtual void AddComponent(Entity) = 0;

    //Removes component from the current entity
    virtual void RemoveComponent(Entity) = 0;

    //Returns true if entity has component
    virtual bool HasComponent(Entity) = 0;

    //Copies component from dst entity to src entity
    virtual void CopyComponent(Entity dst, Entity src) = 0;
    
    std::string name{};
};

// This class is used to store the functions of a component
template <typename T>
class IComponentFunctions : public ComponentFunctions {
public:
    //Adds component to the current entity
    void AddComponent(Entity e);

    //Removes component from the current entity
    void RemoveComponent(Entity e);

    //Returns true if entity has component
    bool HasComponent(Entity e);

    //Copies component from dst entity to src entity
    void CopyComponent(Entity dst, Entity src);
};

// This virtual class is used to store the functions of a component
class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
};

// This class is used to store the functions of a component
template<typename T>
class ComponentArray : public IComponentArray {
public:

    // Insert data into the array
    void InsertData(Entity entity, T component) {
        ASSERT(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end(), "Component added to same entity more than once.");

        // Put new entry at end and update the maps
        size_t newIndex = m_Size;
        m_EntityToIndexMap[entity] = newIndex;
        m_IndexToEntityMap[newIndex] = entity;

        // This portion is new ========================================
        m_ComponentArray[newIndex] = static_cast<T*>(m_MemoryManager->Allocate());
        new (m_ComponentArray[newIndex]) T(); // Placement new, creates object at place of pointer, DOES NOT ALLOCATE MEMORY
        *(m_ComponentArray[newIndex]) = component;
        // End of new portion ========================================

        ++m_Size;

    }

    // Removes data from the array
    void RemoveData(Entity entity) {
        ASSERT(m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end(), "Removing non-existent component.");

        // Copy element at end into deleted element's place to maintain density
        size_t indexOfRemovedEntity = m_EntityToIndexMap[entity];

        size_t indexOfLastElement = m_Size - 1;
        if (m_ComponentArray[indexOfRemovedEntity] != nullptr) {
            (void)(m_ComponentArray[indexOfRemovedEntity]->~T()); // Manually call the destructor
            m_MemoryManager->Free(m_ComponentArray[indexOfRemovedEntity]); // Free the memory
        }
        m_ComponentArray[indexOfRemovedEntity] = m_ComponentArray[indexOfLastElement];

        // Update map to point to moved spot
        Entity entityOfLastElement = m_IndexToEntityMap[indexOfLastElement];
        m_EntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
        m_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

        m_EntityToIndexMap.erase(entity);
        m_IndexToEntityMap.erase(indexOfLastElement);

        --m_Size;
    }
    
    // Returns a reference to the component of an entity
    T& GetData(Entity entity) {
        ASSERT(m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end(), "Retrieving non-existent component.");

        // Return a reference to the entity's component
        return *(m_ComponentArray[m_EntityToIndexMap[entity]]);
    }
    
    // Updates the maps when an entity is destroyed
    void EntityDestroyed(Entity entity) {
        if (m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end()) {
            // Remove the entity's component if it existed
            RemoveData(entity);
        }
    }
    
    // Checks if an entity has a component
    bool HasComponent(Entity entity) {
        return m_EntityToIndexMap.count(entity) ? true : false;
    }

    // Returns the array of entities
    std::vector<Entity> GetEntityArray() {
        std::vector<Entity> array{};
        for (auto& e : m_EntityToIndexMap) {
            array.push_back(e.first);
        }
        return array;
    }

    // Returns the array of components
    std::vector<T*> GetDataArray() {
        std::vector<T*> array{};
        for (auto& e : m_EntityToIndexMap) {
            array.push_back(m_ComponentArray[e.second]);
        }
        return array;
    }

    // Returns the array of pairs of entities and components
    std::vector<std::pair<Entity, T*>> GetPairArray() {
        std::vector<std::pair<Entity, T*>> array{};
        for (auto& e : m_EntityToIndexMap) {
            array.push_back(std::pair<Entity,T*>{e.first, m_ComponentArray[e.second]});
        }
        return array;
    }

    // Constructor
    ComponentArray() : m_MemoryManager{ std::make_unique<ObjectAllocator>((sizeof(T) < sizeof(void*)) ? (sizeof(void*)) : (sizeof(T)), config) } {};

    // Destructor
    ~ComponentArray() {
        for (auto& e : m_EntityToIndexMap) {
            if (m_ComponentArray[e.second] != nullptr) {
                (void)(m_ComponentArray[e.second]->~T());
				m_MemoryManager->Free(m_ComponentArray[e.second]);
			}
		}
	}

private:

    // This portion is new ========================================
    bool useCPPMemMgr{ false };
    unsigned objectsPerPage{ 32 };
    unsigned maxPages{};
    bool debug{ false };
    unsigned padbytes{ 0 };
    OAConfig::HeaderBlockInfo header{ OAConfig::hbNone };
    unsigned alignment{ 16 };
    OAConfig config{ useCPPMemMgr,objectsPerPage, maxPages, debug, padbytes, header, alignment };

    std::unique_ptr<ObjectAllocator> m_MemoryManager;
    // End of new portion ========================================

    // The packed array of components (of generic type T),
    // set to a specified maximum amount, matching the maximum number
    // of entities allowed to exist simultaneously, so that each entity
    // has a unique spot.
    std::array<T*, MAX_ENTITIES> m_ComponentArray{};

    // Map from an entity ID to an array index.
    std::unordered_map<Entity, size_t> m_EntityToIndexMap{};

    // Map from an array index to an entity ID.
    std::unordered_map<size_t, Entity> m_IndexToEntityMap{};

    // Total size of valid entries in the array.
    size_t m_Size{};
};


class ComponentManager {
public:

    // Registers a component type
    template<typename T>
    void RegisterComponent() {
        const char* typeName = typeid(T).name();

        ASSERT(m_ComponentTypes.find(typeName) != m_ComponentTypes.end(), "Registering component type more than once.");

        // Add this component type to the component type map
        m_ComponentTypes.insert({ typeName, m_NextComponentType });

        // Create a ComponentArray pointer and add it to the component arrays map
        m_ComponentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

        // Increment the value so that the next component registered will be different
        ++m_NextComponentType;
    }

    // Returns the component type of a component
    template<typename T>
    ComponentType GetComponentType() {
        const char* typeName = typeid(T).name();

        ASSERT(m_ComponentTypes.find(typeName) == m_ComponentTypes.end(), "Component not registered before use.");

        // Return this component's type - used for creating signatures
        return m_ComponentTypes[typeName];
    }

    // Adds a component to an entity
    template<typename T>
    void AddComponent(Entity entity, T component) {
        // Add a component to the array for an entity
        GetComponentArray<T>()->InsertData(entity, component);
    }

    // Removes a component from an entity
    template<typename T>
    void RemoveComponent(Entity entity) {
        // Remove a component from the array for an entity
        GetComponentArray<T>()->RemoveData(entity);
    }

    // Returns a reference to a component of an entity
    template<typename T>
    T& GetComponent(Entity entity) {
        // Get a reference to a component from the array for an entity
        return GetComponentArray<T>()->GetData(entity);
    }

    // Updates the component arrays when an entity is destroyed
    void EntityDestroyed(Entity entity);


    // Checks if a component type is registered
    template<typename T>
    bool isComponentTypeRegistered() {
        const char* typeName = typeid(T).name();
        return m_ComponentTypes.find(typeName) != m_ComponentTypes.end();
    }

    // Returns the component array of a component type
    template<typename T>
    ComponentArray<T>& GetComponentArrayRef() {
        const char* typeName = typeid(T).name();
        ASSERT(m_ComponentTypes.find(typeName) == m_ComponentTypes.end(), "Component not registered before use.");
        return *static_cast<ComponentArray<T>*>(m_ComponentArrays[typeName].get());
    }


private:
    // Map from type string pointer to a component type
    std::unordered_map<const char*, ComponentType> m_ComponentTypes{};

    // Map from type string pointer to a component array
    std::unordered_map<const char*, std::shared_ptr<IComponentArray>> m_ComponentArrays{};

    // The component type to be assigned to the next registered component - starting at 0
    ComponentType m_NextComponentType{};

    // Convenience function to get the statically casted pointer to the ComponentArray of type T.
    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        const char* typeName = typeid(T).name();

        ASSERT(m_ComponentTypes.find(typeName) == m_ComponentTypes.end(), "Component not registered before use.");

        return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeName]);
    }
};


////////// SYSTEM /////////////////////////////////////////////////////////////

class System {
public:
    virtual void Update() = 0;
    virtual void Draw() {};
    std::set<Entity> m_Entities;
};


class SystemManager {
public:

    // Registers new systems
    template<typename T>
    std::shared_ptr<T> RegisterSystem() {
        const char* typeName = typeid(T).name();

        ASSERT(m_Systems.find(typeName) != m_Systems.end(), "Registering system more than once.");

        // Create a pointer to the system and return it so it can be used externally
        std::shared_ptr<T> system = std::make_shared<T>();
        m_Systems.insert({ typeName, system });
        return system;
    }
    
    // Registers new systems
    template<typename T>
    void SetSignature(Signature signature) {
        const char* typeName = typeid(T).name();

        ASSERT(m_Systems.find(typeName) == m_Systems.end(), "System used before registered.");

        // Set the signature for this system
        m_Signatures.insert({ typeName, signature });
    }

    
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

    // Initializes the ECS
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

    uint32_t GetEntityCount();

    // Component methods ------------------------------------------------------
    template<typename T>
    void RegisterComponent() {
        //Add type to type manager
        std::shared_ptr<IComponentFunctions<T>> f{ std::make_shared<IComponentFunctions<T>>() };
        f->name = typeid(T).name();
        m_TypeManager[typeid(T).name()] = f;

        m_ComponentManager->RegisterComponent<T>();
    }

    // Adds a component to an entity
    template<typename T>
    void AddComponent(Entity entity, T component) {
        m_ComponentManager->AddComponent<T>(entity, component);

        Signature signature = m_EntityManager->GetSignature(entity);
        signature.set(m_ComponentManager->GetComponentType<T>(), true);
        m_EntityManager->SetSignature(entity, signature);

        m_SystemManager->EntitySignatureChanged(entity, signature);
    }

    // Removes a component from an entity
    template<typename T>
    void RemoveComponent(Entity entity) {
        m_ComponentManager->RemoveComponent<T>(entity);

        Signature signature = m_EntityManager->GetSignature(entity);
        signature.set(m_ComponentManager->GetComponentType<T>(), false);
        m_EntityManager->SetSignature(entity, signature);

        m_SystemManager->EntitySignatureChanged(entity, signature);
    }

    // Returns a reference to a component of an entity
    template<typename T>
    T& GetComponent(Entity entity) {
        return m_ComponentManager->GetComponent<T>(entity);
    }

    // Returns the type of a component
    template<typename T>
    ComponentType GetComponentType() {
        return m_ComponentManager->GetComponentType<T>();
    }

    // Checks if a component type is registered
    template<typename T>
    bool isComponentTypeRegistered() {
        return m_ComponentManager->isComponentTypeRegistered<T>();
    }

    // Checks if an entity has a component
    template<typename T>
    bool HasComponent(Entity entity) {
        return m_ComponentManager->isComponentTypeRegistered<T>() &&
            m_EntityManager->GetSignature(entity).test(m_ComponentManager->GetComponentType<T>());
    }

    ComponentManager& GetComponentManager();

    std::unordered_map<std::string, std::shared_ptr<ComponentFunctions>>& GetTypeManager();

    // System methods ---------------------------------------------------------
    // Registers a system
    template<typename T>
    std::shared_ptr<T> RegisterSystem() {
        return m_SystemManager->RegisterSystem<T>();
    }

    // Sets the signature of a system
    template<typename T>
    std::shared_ptr<T> RegisterSystem(T& input) {
        return m_SystemManager->RegisterSystem<T>(input);
    }

    // Sets the signature of a system
    template<typename T>
    void SetSystemSignature(Signature signature) {
        m_SystemManager->SetSignature<T>(signature);
    }

    // Checks whether an entity exists
    bool EntityExists(Entity entity) {
        return m_EntityManager->EntityExists(entity);
    }

private:
    // Constructor
    ECS() {}
    std::unordered_map<std::string, std::shared_ptr<ComponentFunctions>> m_TypeManager;
    std::unique_ptr<ComponentManager> m_ComponentManager;
    std::unique_ptr<EntityManager> m_EntityManager;
    std::unique_ptr<SystemManager> m_SystemManager;
};

////////// Definitions of IComponentFunctions //////////////////////////////////

// Adds component to the current entity
template <typename T>
void IComponentFunctions<T>::AddComponent(Entity e) {
    ECS::ecs().AddComponent<T>(e, T{});
}

// Removes component from the current entity
template <typename T>
void IComponentFunctions<T>::RemoveComponent(Entity e) {
    ECS::ecs().RemoveComponent<T>(e);
}

// Returns true if entity has component
template <typename T>
bool IComponentFunctions<T>::HasComponent(Entity e) {
    return ECS::ecs().HasComponent<T>(e);
}

// Copies component from dst entity to src entity
template <typename T>
void IComponentFunctions<T>::CopyComponent(Entity dst, Entity src) {
    if (ECS::ecs().HasComponent<T>(dst)) {
        ECS::ecs().GetComponent<T>(dst) = ECS::ecs().GetComponent<T>(src);
    }
    else {
        ECS::ecs().AddComponent<T>(dst, ECS::ecs().GetComponent<T>(src));
    }
}

////////// System Declarations ////////////////////////////////////////////////

class PhysicsSystem : public System {
public:
    void Update() override;
    void Draw() override;
};

class ParticleSystem : public System {
public:
    void Update() override;
    void Draw() override;
};

class EmitterSystem : public System {
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

class AnimationSystem : public System {
public:
    void Update() override;
};

class AudioSystem : public System {
public:
    void Update() override;
};

class GraphicsSystem : public System {
public:
    void Initialize();
    void Update() override;
    void Draw() override;
};

class SerializationSystem : public System {
public:
    void Update() override;
};


class EditingSystem : public System {
public:
    void Update() override;
    void Draw() override;
};

class UITextLabelSystem : public System {
public:
    void Update() override;
    void Draw() override;
};

class UIButtonSystem : public System {
public:
    void Update() override;
};

class UIHealthBarSystem : public System {
public:
    void Update() override;
};

class UISkillPointSystem : public System {
public:
    void Update() override;
};

class UIAttackSkillSystem : public System {
public:
    void Update() override;
};

class UIAllyHudSystem : public System {
public:
    void Update() override;
};

class UIEnemyHudSystem : public System {
public:
    void Update() override;
};

class UIEffectSystem : public System {
public:
    void Update() override;
};

class UIDialogueSystem : public System {
public:
    void Initialize();
    void Update() override;
};

class UISliderSystem : public System {
public:
    void Update() override;
};

class ModelSystem : public System {
public:
    void Update() override;
};

class ChildSystem : public System {
public:
    void Update() override;
};

class ParentSystem : public System {
public:
    void Update() override;
};
