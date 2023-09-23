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
*	@file		Systems.cpp
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
*	@brief		Systems running on underlying ECS Architecture
*
*	This file contains the system functions running on the underlying ECS
*	Architecture. It will call other functions in their respective source
*	files, and pass the required Entity information to them.
* 
******************************************************************************/


#include "ECS.h"
#include "Components.h"
#include "Movement.h"
#include "graphics.h"
#include "model.h"
#include "message.h"


extern ECS ecs;
extern Mail mail;
	
void PhysicsSystem::Update() {

}
	
// Movement System
// For MainCharacter
void MovementSystem::Update() {
	//std::cout << "MovementSystem's m_Entities Size(): " << m_Entities.size() <<std::endl;

	//// Access the ComponentManager through the ECS class
	//ComponentManager& componentManager = ecs.GetComponentManager();

	//// Access component arrays through the ComponentManager
	//auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	//auto& animationArray = componentManager.GetComponentArrayRef<Animation>();
	//auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	//auto& sizeArray = componentManager.GetComponentArrayRef<Size>();

	for (Entity const& entity : m_Entities) {
		UpdateMovement(ecs.GetComponent<Transform>(entity));

		/*Model modelData = modelArray.GetData(entity);
		Animation* aniData = &animationArray.GetData(entity);
		Tex* texData = &texArray.GetData(entity);
		Size* sizeData = &sizeArray.GetData(entity);*/

		Model* modelData = &ecs.GetComponent<Model>(entity);
		Animation* aniData = &ecs.GetComponent<Animation>(entity);
		Tex* texData = &ecs.GetComponent<Tex>(entity);
		Size* sizeData = &ecs.GetComponent<Size>(entity);

		modelData->UpdateAnimationMC(*aniData, *texData, *sizeData);
	}
	mail.mailbox[ADDRESS::MOVEMENT].clear();
}

void ModelSystem::Update() {

	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ecs.GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& animationArray = componentManager.GetComponentArrayRef<Animation>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();

	for (Entity const& entity : m_Entities) {
		Model* modelData = &modelArray.GetData(entity);
		Animation* aniData = &animationArray.GetData(entity);
		Tex* texData = &texArray.GetData(entity);
		Size* sizeData = &sizeArray.GetData(entity);

		modelData->UpdateAnimationNPC(*aniData, *texData, *sizeData);
	}
	mail.mailbox[ADDRESS::MODEL].clear();
}

void GraphicsSystem::Initialize() {
	for (Entity const& entity : m_Entities) {
		Model* m = &ecs.GetComponent<Model>(entity);
		m->Update(ecs.GetComponent<Transform>(entity), ecs.GetComponent<Size>(entity));
	}
}

void GraphicsSystem::Update() {
	//std::cout << "GraphicsSystem's m_Entities Size(): " << m_Entities.size() << std::endl;
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ecs.GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	auto& animationArray = componentManager.GetComponentArrayRef<Animation>();

	for (Entity const& entity : m_Entities) {
		Model m = modelArray.GetData(entity);
		m.Update(transformArray.GetData(entity), sizeArray.GetData(entity));
		m.Draw(texArray.GetData(entity), animationArray.GetData(entity));
	}
	graphics.Draw();
}
