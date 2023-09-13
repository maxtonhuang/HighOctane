#include "ECS.h"




namespace Architecture {

    ///////////////////////////////////////////////////////////////////////////
    ////////// ENTITY /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    EntityManager::EntityManager() {
        // Initialize the queue with all possible entity IDs
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
            m_AvailableEntities.push(entity);
        }
    }

    Entity EntityManager::CreateEntity() {
        Assert(m_LivingEntityCount < MAX_ENTITIES, "Too many entities in existence.");

        // Take an ID from the front of the queue
        Entity id = m_AvailableEntities.front();
        m_AvailableEntities.pop();
        ++m_LivingEntityCount;

        return id;
    }

    void EntityManager::DestroyEntity(Entity entity) {
        Assert(entity < MAX_ENTITIES, "Entity out of range.");

        // Invalidate the destroyed entity's signature
        m_Signatures[entity].reset();

        // Put the destroyed ID at the back of the queue
        m_AvailableEntities.push(entity);
        --m_LivingEntityCount;
    }

    void EntityManager::SetSignature(Entity entity, Signature signature) {
        Assert(entity < MAX_ENTITIES, "Entity out of range.");

        // Put this entity's signature into the array
        m_Signatures[entity] = signature;
    }

    Signature EntityManager::GetSignature(Entity entity) {
        Assert(entity < MAX_ENTITIES, "Entity out of range.");

        // Get this entity's signature from the array
        return m_Signatures[entity];
    }



    ///////////////////////////////////////////////////////////////////////////
    ////////// COMPONENT //////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////

    // ---------- Component Array ---------- //

    template<typename T>
    void ComponentArray<T>::InsertData(Entity entity, T component) {
        Assert(m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end(), "Component added to same entity more than once.");

        // Put new entry at end and update the maps
        size_t newIndex = m_Size;
        m_EntityToIndexMap[entity] = newIndex;
        m_IndexToEntityMap[newIndex] = entity;
        m_ComponentArray[newIndex] = component;
        ++m_Size;
    }

    template<typename T>
    void ComponentArray<T>::RemoveData(Entity entity) {
        Assert(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end(), "Removing non-existent component.");

        // Copy element at end into deleted element's place to maintain density
        size_t indexOfRemovedEntity = m_EntityToIndexMap[entity];
        size_t indexOfLastElement = m_Size - 1;
        m_ComponentArray[indexOfRemovedEntity] = m_ComponentArray[indexOfLastElement];

        // Update map to point to moved spot
        Entity entityOfLastElement = m_IndexToEntityMap[indexOfLastElement];
        m_EntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
        m_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

        m_EntityToIndexMap.erase(entity);
        m_IndexToEntityMap.erase(indexOfLastElement);

        --m_Size;
    }

    template<typename T>
    T& ComponentArray<T>::GetData(Entity entity) {
        Assert(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end(), "Retrieving non-existent component.");

        // Return a reference to the entity's component
        return m_ComponentArray[m_EntityToIndexMap[entity]];
    }

    template<typename T>
    void ComponentArray<T>::EntityDestroyed(Entity entity) {
        if (m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end()) {
            // Remove the entity's component if it existed
            RemoveData(entity);
        }
    }


    // ---------- Component Manager ---------- //

    template<typename T>
    void ComponentManager::RegisterComponent() {
        const char* typeName = typeid(T).name();

        Assert(m_ComponentTypes.find(typeName) == m_ComponentTypes.end(), "Registering component type more than once.");

        // Add this component type to the component type map
        m_ComponentTypes.insert({ typeName, m_NextComponentType });

        // Create a ComponentArray pointer and add it to the component arrays map
        m_ComponentArrays.insert({ typeName, std::make_shared<ComponentArray<T>>() });

        // Increment the value so that the next component registered will be different
        ++m_NextComponentType;
    }

    template<typename T>
    ComponentType ComponentManager::GetComponentType() {
        const char* typeName = typeid(T).name();

        Assert(m_ComponentTypes.find(typeName) != m_ComponentTypes.end(), "Component not registered before use.");

        // Return this component's type - used for creating signatures
        return m_ComponentTypes[typeName];
    }

    template<typename T>
    void ComponentManager::AddComponent(Entity entity, T component) {
        // Add a component to the array for an entity
        GetComponentArray<T>()->InsertData(entity, component);
    }

    template<typename T>
    void ComponentManager::RemoveComponent(Entity entity) {
        // Remove a component from the array for an entity
        GetComponentArray<T>()->RemoveData(entity);
    }

    template<typename T>
    T& ComponentManager::GetComponent(Entity entity) {
        // Get a reference to a component from the array for an entity
        return GetComponentArray<T>()->GetData(entity);
    }

    void ComponentManager::EntityDestroyed(Entity entity) {
        // Notify each component array that an entity has been destroyed
        // If it has a component for that entity, it will remove it
        for (std::pair<const char*, std::shared_ptr<IComponentArray>> const& pair : m_ComponentArrays) {
            std::shared_ptr<IComponentArray> const& component = pair.second;

            component->EntityDestroyed(entity);
        }
    }

    template<typename T>
    std::shared_ptr<ComponentArray<T>> ComponentManager::GetComponentArray() {
        const char* typeName = typeid(T).name();

        Assert(m_ComponentTypes.find(typeName) != m_ComponentTypes.end(), "Component not registered before use.");

        return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeName]);
    }



    ///////////////////////////////////////////////////////////////////////////
    ////////// SYSTEM /////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////


    template<typename T>
    std::shared_ptr<T> SystemManager::RegisterSystem() {
        const char* typeName = typeid(T).name();

        Assert(m_Systems.find(typeName) == m_Systems.end(), "Registering system more than once.");

        // Create a pointer to the system and return it so it can be used externally
        std::shared_ptr<T> system = std::make_shared<T>();
        m_Systems.insert({ typeName, system });
        return system;
    }

    template<typename T>
    void SystemManager::SetSignature(Signature signature) {
        const char* typeName = typeid(T).name();

        Assert(m_Systems.find(typeName) != m_Systems.end(), "System used before registered.");

        // Set the signature for this system
        m_Signatures.insert({ typeName, signature });
    }

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
        m_EntityManager = std::make_unique<EntityManager>();
        m_ComponentManager = std::make_unique<ComponentManager>();
        m_SystemManager = std::make_unique<SystemManager>();
    }


    // Entity methods
    Entity ECS::CreateEntity() {
        return m_EntityManager->CreateEntity();
    }

    void ECS::DestroyEntity(Entity entity) {
        m_EntityManager->DestroyEntity(entity);

        m_ComponentManager->EntityDestroyed(entity);

        m_SystemManager->EntityDestroyed(entity);
    }


    // Component methods
    template<typename T>
    void ECS::RegisterComponent() {
        m_ComponentManager->RegisterComponent<T>();
    }

    template<typename T>
    void ECS::AddComponent(Entity entity, T component) {
        m_ComponentManager->AddComponent<T>(entity, component);

        Signature signature = m_EntityManager->GetSignature(entity);
        signature.set(m_ComponentManager->GetComponentType<T>(), true);
        m_EntityManager->SetSignature(entity, signature);

        m_SystemManager->EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    void ECS::RemoveComponent(Entity entity) {
        m_ComponentManager->RemoveComponent<T>(entity);

        Signature signature = m_EntityManager->GetSignature(entity);
        signature.set(m_ComponentManager->GetComponentType<T>(), false);
        m_EntityManager->SetSignature(entity, signature);

        m_SystemManager->EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    T& ECS::GetComponent(Entity entity) {
        return m_ComponentManager->GetComponent<T>(entity);
    }

    template<typename T>
    ComponentType ECS::GetComponentType() {
        return m_ComponentManager->GetComponentType<T>();
    }


    // System methods
    template<typename T>
    std::shared_ptr<T> ECS::RegisterSystem() {
        return m_SystemManager->RegisterSystem<T>();
    }

    template<typename T>
    void ECS::SetSystemSignature(Signature signature) {
        m_SystemManager->SetSignature<T>(signature);
    }

extern ECS ecs;

}

