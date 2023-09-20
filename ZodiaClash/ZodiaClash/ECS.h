#pragma once

//#include <iostream>
//#include <vector>
//#include <array>
//#include <string>
#include <array>
#include <bitset>
#include <queue>
#include <unordered_map>
#include <set>
#include <memory>
#include "debugdiagnostic.h"
#include "Components.h"


namespace Architecture {

    // A simple type alias
    using Entity = std::uint32_t;

    // Used to define the size of arrays later on
    const Entity MAX_ENTITIES = 1'000'000;

    // A simple type alias
    using ComponentType = std::uint8_t;

    // Used to define the size of arrays later on
    const ComponentType MAX_COMPONENTS = 16;

    // A simple type alias
    using Signature = std::bitset<MAX_COMPONENTS>;


    ////////// ENTITY /////////////////////////////////////////////////////////

    class EntityManager {
    public:
        EntityManager();

        Entity CreateEntity();

        void DestroyEntity(Entity entity);

        void SetSignature(Entity entity, Signature signature);

        Signature GetSignature(Entity entity);

    private:
        // Queue of unused entity IDs
        std::queue<Entity> m_AvailableEntities{};

        // Array of signatures where the index corresponds to the entity ID
        std::array<Signature, MAX_ENTITIES> m_Signatures{};

        // Total living entities - used to keep limits on how many exist
        uint32_t m_LivingEntityCount{};
    };


    ////////// COMPONENT //////////////////////////////////////////////////////

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


    ////////// SYSTEM /////////////////////////////////////////////////////////

    class System {
    public:
        virtual void Update() = 0;
        std::set<Entity> m_Entities;
    };


    class SystemManager {
    public:
        template<typename T>
        std::shared_ptr<T> RegisterSystem();
        //template<typename T>
        //std::shared_ptr<T> RegisterSystem(T& input);

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


    ////////// ECS COORDINATOR ////////////////////////////////////////////////

    class ECS {
    public:
        void Init();

        // Entity methods ------------------------------
        Entity CreateEntity();

        void DestroyEntity(Entity entity);

        // Component methods ---------------------------
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

        // System methods ------------------------------
        template<typename T>
        std::shared_ptr<T> RegisterSystem();
        template<typename T>
        std::shared_ptr<T> RegisterSystem(T& input);

        template<typename T>
        void SetSystemSignature(Signature signature);

    private:
        std::unique_ptr<ComponentManager> m_ComponentManager;
        std::unique_ptr<EntityManager> m_EntityManager;
        std::unique_ptr<SystemManager> m_SystemManager;
    };

    #include "ECS.t" // for template functions of ECS

    class PhysicsSystem : public System {
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
        void Update() override;
    };

    
}

