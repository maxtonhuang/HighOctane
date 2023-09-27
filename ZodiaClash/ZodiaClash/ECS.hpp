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
*	@file		ECS.hpp
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
*	This file contains the templte definitions of all the different functions
*   of the ECS. For more information, please refer to "ECS.h".
*
******************************************************************************/


///////////////////////////////////////////////////////////////////////////
////////// COMPONENT //////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

// ---------- Component Array ---------- //

template<typename T>
void ComponentArray<T>::InsertData(Entity entity, T component) {
    Assert(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end(), "Component added to same entity more than once.");

    // Put new entry at end and update the maps
    size_t newIndex = m_Size;
    m_EntityToIndexMap[entity] = newIndex;
    m_IndexToEntityMap[newIndex] = entity;
    m_ComponentArray[newIndex] = component;
    ++m_Size;
}

template<typename T>
void ComponentArray<T>::RemoveData(Entity entity) { // check
    Assert(m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end(), "Removing non-existent component.");

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
    Assert(m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end(), "Retrieving non-existent component.");

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

    Assert(m_ComponentTypes.find(typeName) != m_ComponentTypes.end(), "Registering component type more than once.");

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

    Assert(m_ComponentTypes.find(typeName) == m_ComponentTypes.end(), "Component not registered before use.");

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

template<typename T>
std::shared_ptr<ComponentArray<T>> ComponentManager::GetComponentArray() {
    const char* typeName = typeid(T).name();

    Assert(m_ComponentTypes.find(typeName) == m_ComponentTypes.end(), "Component not registered before use.");

    return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeName]);
}

template<typename T>
bool ComponentManager::isComponentTypeRegistered() {
    const char* typeName = typeid(T).name();
    return m_ComponentTypes.find(typeName) != m_ComponentTypes.end();
}

template<typename T>
ComponentArray<T>& ComponentManager::GetComponentArrayRef() {
    const char* typeName = typeid(T).name();
    Assert(m_ComponentTypes.find(typeName) == m_ComponentTypes.end(), "Component not registered before use.");
    return *static_cast<ComponentArray<T>*>(m_ComponentArrays[typeName].get());
}



///////////////////////////////////////////////////////////////////////////
////////// SYSTEM /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

template<typename T>
std::shared_ptr<T> SystemManager::RegisterSystem() {
    const char* typeName = typeid(T).name();

    Assert(m_Systems.find(typeName) != m_Systems.end(), "Registering system more than once.");

    // Create a pointer to the system and return it so it can be used externally
    std::shared_ptr<T> system = std::make_shared<T>();
    m_Systems.insert({ typeName, system });
    return system;
}

/*template<typename T>
std::shared_ptr<T> SystemManager::RegisterSystem(T& input) {
    const char* typeName = typeid(T).name();

    Assert(m_Systems.find(typeName) != m_Systems.end(), "Registering system more than once.");

    // Create a pointer to the system and return it so it can be used externally
    std::shared_ptr<T> system = std::shared_ptr<T>(&input);
    m_Systems.insert({ typeName, system });
    return system;
}*/

template<typename T>
void SystemManager::SetSignature(Signature signature) {
    const char* typeName = typeid(T).name();

    Assert(m_Systems.find(typeName) == m_Systems.end(), "System used before registered.");

    // Set the signature for this system
    m_Signatures.insert({ typeName, signature });
}

///////////////////////////////////////////////////////////////////////////
////////// ECS COORDINATOR ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

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

template<typename T>
bool ECS::isComponentTypeRegistered() {
    return m_ComponentManager->isComponentTypeRegistered<T>();
}

template<typename T>
bool ECS::HasComponent(Entity entity) {
    return m_ComponentManager->isComponentTypeRegistered<T>() &&
        m_EntityManager->GetSignature(entity).test(m_ComponentManager->GetComponentType<T>());
}
// System methods
template<typename T>
std::shared_ptr<T> ECS::RegisterSystem() {
    return m_SystemManager->RegisterSystem<T>();
}

template<typename T>
std::shared_ptr<T> ECS::RegisterSystem(T& input) {
    return m_SystemManager->RegisterSystem<T>(input);
}

template<typename T>
void ECS::SetSystemSignature(Signature signature) {
    m_SystemManager->SetSignature<T>(signature);
}
