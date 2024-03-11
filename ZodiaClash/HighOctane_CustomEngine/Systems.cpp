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
#include "Graphics.h"
#include "Camera.h"
#include "model.h"
#include "message.h"
#include "physics.h"
#include "MultiThreading.h"
#include "Global.h"
#include "CollisionResolution.h"
#include "Serialization.h"
#include "Animator.h"
#include "Scripting.h"
#include "Editing.h"
#include "ScriptEngine.h"
#include "Battle.h"
#include "EntityFactory.h"
#include "EngineCore.h"
#include "CharacterStats.h"
#include "UIComponents.h"
#include "AssetManager.h"
#include "Layering.h"
#include "Selection.h"
#include "Utilities.h"
#include "Animation.h"
#include "UndoRedo.h"
#include "Particles.h"
#include <random>
#define FIXED_DT 1.0f/60.f
#define MAX_ACCUMULATED_TIME 5.f // to avoid the "spiral of death" if the system cannot keep up

constexpr float CORNER_SIZE{ 10.f };
constexpr float CROSS_SIZE{ 10.f };

// Extern for the vector to contain the full name for ImGui for scripting system
extern std::vector<std::string> fullNameVecImGUI;

/******************************************************************************
*
*	@brief Handles the physics-related updates of entities
*
*	This function handles:
*   1. Reading and processing physics-related mailbox messages.
*   2. Updating physics attributes based on entity size.
*   3. Debug drawing and physics calculations based on stepping mode.
*   4. Clearing the mailbox after processing.
*
******************************************************************************/
void PhysicsSystem::Update() {


	//process mesaage here
	bool reqStep{ false };
#if _DEBUG
	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::PHYSICS]) {
		switch (msg.type) {
		case TYPE::KEY_TRIGGERED:
			if (msg.info == INFO::KEY_8) {
				reqStep = true;
			}
			if (msg.info == INFO::KEY_9) {
				physics::PHYSICS->ToggleStepMode();
			}
			if (msg.info == INFO::KEY_0) {
				physics::PHYSICS->ToggleDebugMode();
			}
			break;
		}
	}

	Mail::mail().mailbox[ADDRESS::PHYSICS].clear(); // Clear the mailbox after processing.
#endif
	// Access component arrays through the ComponentManager
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& colliderArray = componentManager.GetComponentArrayRef<Collider>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();

	//update entity half-dimensions
	for (Entity const& entity : m_Entities) {
		Size sizeData{ sizeArray.GetData(entity) };
		Transform transformData{ transformArray.GetData(entity) };
		transformArray.GetData(entity).halfDimensions = { sizeData.width / 2.f * transformData.scale, sizeData.height / 2.f * transformData.scale };
		Collider colliderData{ colliderArray.GetData(entity) };
		if (colliderData.dimension.x == 0.f && colliderData.dimension.y == 0.f) {
			colliderData.dimension.x = sizeData.width;
			colliderData.dimension.y = sizeData.height;
			colliderData.scale = 1.f;
		}
		colliderArray.GetData(entity).position = transformData.position;
		colliderArray.GetData(entity).halfDimensions = { colliderData.dimension.x / 2.f * colliderData.scale, colliderData.dimension.y / 2.f * colliderData.scale };
	}

	// Check step mode and integrate physics
	if (physics::PHYSICS->GetStepModeActive()) {
		// Debug draw all entities
		// If step is required, integrate physics for all entities
		if (reqStep) {
			//for (Entity const& entity : m_Entities) {
			for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
				if (layersToSkip[layer_it] && layersToLock[layer_it]) {
					for (Entity& entity : layering[layer_it]) {
						if (entitiesToSkip[static_cast<uint32_t>(entity)] && entitiesToLock[static_cast<uint32_t>(entity)] && ECS::ecs().EntityExists(entity)) {
							if (ECS::ecs().HasComponent<Clone>(entity) && ECS::ecs().HasComponent<Transform>(entity) && ECS::ecs().HasComponent<Collider>(entity)) {
								Transform& transData = transformArray.GetData(entity);
								Collider& collData = colliderArray.GetData(entity);
								physics::PHYSICS->Integrate(transData, collData);
							}
						}
					}
				}
			}
		}
	}

	else {
		// Regular physics integration and debug drawing
		//for (Entity const& entity : m_Entities) {
		for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
			if (layersToSkip[layer_it] && layersToLock[layer_it]) {
				for (Entity& entity : layering[layer_it]) {
					if (entitiesToSkip[static_cast<uint32_t>(entity)] && entitiesToLock[static_cast<uint32_t>(entity)] && ECS::ecs().EntityExists(entity)) {
						if (ECS::ecs().HasComponent<Clone>(entity) && ECS::ecs().HasComponent<Transform>(entity) && ECS::ecs().HasComponent<Collider>(entity)) {
							Transform& transData = transformArray.GetData(entity);
							Collider& collData = colliderArray.GetData(entity);
							physics::PHYSICS->Integrate(transData, collData);
						}
					}
				}
			}
		}
	}
}

void PhysicsSystem::Draw() {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& colliderArray = componentManager.GetComponentArrayRef<Collider>();

	//for (Entity const& entity : m_Entities) {
	for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		if (layersToSkip[layer_it] && layersToLock[layer_it]) {
			for (Entity& entity : layering[layer_it]) {
				if (entitiesToSkip[static_cast<uint32_t>(entity)] && entitiesToLock[static_cast<uint32_t>(entity)] && ECS::ecs().EntityExists(entity)) {
					if (ECS::ecs().HasComponent<Clone>(entity) && ECS::ecs().HasComponent<Transform>(entity) && ECS::ecs().HasComponent<Collider>(entity)) {
						Transform& transData = transformArray.GetData(entity);
						Collider& collData = colliderArray.GetData(entity);

						physics::PHYSICS->DebugDraw(transData, collData);
					}
				}
			}
		}
	}
}

void EmitterSystem::Update()
{
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& emitterArray = componentManager.GetComponentArrayRef<Emitter>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();

	for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		// Ensure that skip and lock logic is applied as intended
		//if (layersToSkip[layer_it] || !layersToLock[layer_it]) continue;

		//for (Entity& entity : layering[layer_it]) {
		for (Entity entity : m_Entities) {

			// Check if entity should be skipped or is not locked as intended
			//if (entitiesToSkip[static_cast<uint32_t>(entity)] || !entitiesToLock[static_cast<uint32_t>(entity)] || !ECS::ecs().EntityExists(entity)) continue;

			if (ECS::ecs().HasComponent<Emitter>(entity)) {
				Emitter* emitter = &ECS::ecs().GetComponent<Emitter>(entity);

				if (emitter->textures.size() == 0) {
					continue;
				}

				emitter->emitterLifetime += FIXED_DT;

				//Initialise emitter
				//if (emitter->size.x == 0 && emitter->size.y == 0) {
				//	emitter->size.x = sizeArray.GetData(entity).width * transformArray.GetData(entity).scale;
				//	emitter->size.y = sizeArray.GetData(entity).height * transformArray.GetData(entity).scale;
				//}

				emitter->position = transformArray.GetData(entity).position;
				float emitterWidth = sizeArray.GetData(entity).width * transformArray.GetData(entity).scale / 2;
				float emitterHeight = sizeArray.GetData(entity).height * transformArray.GetData(entity).scale / 2;
				std::random_device rd;
				std::mt19937 gen(rd());
				// Create a uniform distribution
				std::uniform_real_distribution<float> dis(-1.f, 1.f);
				std::uniform_int_distribution<int> disTex(0, (int)(emitter->textures.size()) - 1);

				if (emitter->emitterLifetime >= emitter->frequency) {
					for (int i = 0; i < emitter->particlesRate; ++i) {
						// Here, you might introduce randomness or variations based on the emitter's properties
						Vec2 position = emitter->position + Vec2{dis(gen) * emitterWidth,dis(gen) * emitterHeight}; // Plus any offset or randomness
						Vec2 size = emitter->size;

						float velocityRandomness = emitter->singleSided ? fabs(dis(gen)) : dis(gen);
						Vec2 velocity = { emitter->velocity.x * velocityRandomness, emitter->velocity.y * fabs(dis(gen))}; // Plus any randomness or directional adjustments
						Color color = emitter->particleColor;
						float rotation = emitter->rotation * dis(gen);
						float rotationSpeed = emitter->rotationSpeed * dis(gen);
						float timer = emitter->particleLifetime;

						// Assuming nullptr for now, but you can pass custom update functions based on emitter or particle type
						void (*particleUpdate)(Particle&) = nullptr;

						// Adding the particle to the system
						auto & p = particles.AddParticle(true, position, size, velocity, color, particleUpdate, rotation, rotationSpeed);

						p.timer = timer;

						int textureIndex{ disTex(gen) };
						p.texture = assetmanager.texture.Get(emitter->textures[textureIndex].c_str());
						if (!p.texture) continue;
						p.textureID = (float)(p.texture->GetID() - 1.f);
					}
					emitter->emitterLifetime = 0.f; // Reset after spawning cycle
				}
			}
		}
	}
}

/**************************************************************************/
/*!
	@brief Advances the state of the particle system by a fixed time step and
		   periodically generates new particles with predetermined properties.

	@details This function performs two main tasks during each update cycle:
			 1. It updates all existing particles in the system using the
				predefined fixed time step (`FIXED_DT`), ensuring consistent
				simulation regardless of frame rate.
			 2. It periodically generates a new particle based on a frequency
				(`freq`), using a random position and velocity. The frequency
				of particle generation is controlled by a timer that resets
				after reaching a specified threshold.
*
******************************************************************************/
void ParticleSystem::Update()
{
	particles.Update(FIXED_DT);
}

/******************************************************************************
*
*	@brief Calls the Draw function in particle manager
*
******************************************************************************/
void ParticleSystem::Draw()
{
	particles.Draw(FIXED_DT);
}

/******************************************************************************
*
*	@brief Checks and handles collisions between entities.
*
*	In this function:
*	1. It fetches required components (like Transform and Body).
*	2. Checks for collisions involving the `MainCharacter`.
*	3. Updates entities' positions and velocities after collisions.
*	4. Clears the collision mailbox.
*
******************************************************************************/

void CollisionSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& colliderArray = componentManager.GetComponentArrayRef<Collider>();

	// Create a list of SweepAndPruneEntry objects to store colliders along the x-axis
	std::vector<physics::SweepAndPruneEntry> xSortedColliders;

	// Populate the xSortedColliders list and sort it along the x-axis
	//for (Entity const& entity : m_Entities) {
	for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		if (layersToSkip[layer_it] && layersToLock[layer_it]) {
			for (Entity& entity : layering[layer_it]) {
				if (entitiesToSkip[static_cast<uint32_t>(entity)] && entitiesToLock[static_cast<uint32_t>(entity)] && ECS::ecs().EntityExists(entity)) {
					if (ECS::ecs().HasComponent<Clone>(entity) && ECS::ecs().HasComponent<Transform>(entity) && ECS::ecs().HasComponent<Collider>(entity)) {
						Collider* collideData = &colliderArray.GetData(entity);

						xSortedColliders.push_back(physics::SweepAndPruneEntry{ entity, collideData->position.x - collideData->halfDimensions.x, collideData->position.x + collideData->halfDimensions.x });
					}
				}
			}
		}
	}

	// Sort the xSortedColliders list based on the lower x-coordinate
	std::sort(xSortedColliders.begin(), xSortedColliders.end(), [](const physics::SweepAndPruneEntry& a, const physics::SweepAndPruneEntry& b) {
		return a.lowerX < b.lowerX;
	});

	// Iterate through the sorted xSortedColliders list and perform collision checks
	for (size_t i = 0; i < xSortedColliders.size(); ++i) {
		for (size_t j = i + 1; j < xSortedColliders.size(); ++j) {
			float aUpperX{ xSortedColliders[i].upperX };
			float bLowerX{ xSortedColliders[j].lowerX };
			if (bLowerX > aUpperX) break;

			Entity const& entity1 = xSortedColliders[i].entity;
			Entity const& entity2 = xSortedColliders[j].entity;

			// Perform collision checks between entity1 and entity2 here
			Transform* transData1 = &transformArray.GetData(entity1);
			Transform* transData2 = &transformArray.GetData(entity2);
			Collider* collideData1 = &colliderArray.GetData(entity1);
			Collider* collideData2 = &colliderArray.GetData(entity2);

			if ((collideData1->bodyShape == Collider::SHAPE_ID::SHAPE_BOX) && (collideData2->bodyShape == Collider::SHAPE_ID::SHAPE_BOX)) {
				bool hasCollided = physics::CheckCollisionBoxBox(*collideData1, *collideData2, transData1->velocity, transData2->velocity);
				if (hasCollided) {
					if (collideData1->type == Collider::EVENT || collideData2->type == Collider::EVENT) {
						if (collideData1->type == Collider::EVENT && collideData2->type == Collider::MAIN && !collideData1->collided) {
							events.Call(collideData1->eventName, collideData1->eventInput);
							collideData1->collided = true;
						}
						else if (collideData2->type == Collider::EVENT && collideData1->type == Collider::MAIN && !collideData2->collided) {
							events.Call(collideData2->eventName, collideData2->eventInput);
							collideData2->collided = true;
						}
					}
					else {
						physics::DynamicStaticResponse(*transData1, *transData2);
					}
				}
			}
			else if ((collideData1->bodyShape == Collider::SHAPE_ID::SHAPE_CIRCLE) && (collideData2->bodyShape == Collider::SHAPE_ID::SHAPE_CIRCLE)) {
				bool hasCollided = physics::CheckCollisionCircleCircle(*collideData1, *collideData2);
				if (hasCollided) {
					physics::DynamicStaticResponse(*transData1, *transData2);
				}
			}
			else if ((collideData1->bodyShape == Collider::SHAPE_ID::SHAPE_CIRCLE) && (collideData2->bodyShape == Collider::SHAPE_ID::SHAPE_BOX)) {
				bool hasCollided = physics::CheckCollisionCircleCircle(*collideData1, *collideData2);
				if (hasCollided) {
					physics::DynamicStaticResponse(*transData1, *transData2);
				}
			}
			else if ((collideData1->bodyShape == Collider::SHAPE_ID::SHAPE_BOX) && (collideData2->bodyShape == Collider::SHAPE_ID::SHAPE_CIRCLE)) {
				bool hasCollided = physics::CheckCollisionCircleCircle(*collideData1, *collideData2);
				if (hasCollided) {
					physics::DynamicStaticResponse(*transData1, *transData2);
				}
			}
		}
	}
	Mail::mail().mailbox[ADDRESS::COLLISION].clear();
}

/******************************************************************************
*
*	@brief Handles movement of the main character
*
*	Calculates the velocity according to key presses and stores it for the
*	Physics System to handle the update of the position.
*
******************************************************************************/
// For MainCharacter only
void MovementSystem::Update() {

	static vmath::Vector2 boundaryMax{ 0.f,0.f };
	static vmath::Vector2 boundaryMin{ 0.f,0.f };

	if (!inEditing || viewportWindowHovered) {

		// Access the ComponentManager through the ECS class
		ComponentManager& componentManager = ECS::ecs().GetComponentManager();

		// Access component arrays through the ComponentManager
		auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
		auto& modelArray = componentManager.GetComponentArrayRef<Model>();
		auto& colliderArray = componentManager.GetComponentArrayRef<Collider>();
		auto& mcArray = componentManager.GetComponentArrayRef<MainCharacter>();
		auto& animationArray = componentManager.GetComponentArrayRef<AnimationSet>();

		//for (Entity const& entity : m_Entities) {
		for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
			if (layersToSkip[layer_it] && layersToLock[layer_it]) {
				for (Entity& entity : layering[layer_it]) {
					if (entitiesToSkip[static_cast<uint32_t>(entity)] && entitiesToLock[static_cast<uint32_t>(entity)] && ECS::ecs().EntityExists(entity)) {
						if (colliderArray.HasComponent(entity)) {
							Collider colliderData = colliderArray.GetData(entity);
							boundaryMax.x = (colliderData.position.x > boundaryMax.x) ? (colliderData.position.x) : boundaryMax.x;
							boundaryMax.y = (colliderData.position.y > boundaryMax.y) ? (colliderData.position.y) : boundaryMax.y;
							boundaryMin.x = (colliderData.position.x < boundaryMin.x) ? (colliderData.position.x) : boundaryMin.x;
							boundaryMin.y = (colliderData.position.y < boundaryMin.y) ? (colliderData.position.y) : boundaryMin.y;
							if (boundaryMax.x - boundaryMin.x < GRAPHICS::defaultWidthF) {
								boundaryMin.x = -GRAPHICS::w;
								boundaryMax.x = GRAPHICS::w;
							}
							if (boundaryMax.y - boundaryMin.y < GRAPHICS::defaultHeightF) {
								boundaryMin.y = -GRAPHICS::h;
								boundaryMax.y = GRAPHICS::h;
							}
						}
						
						if (ECS::ecs().HasComponent<MainCharacter>(entity) && ECS::ecs().HasComponent<Clone>(entity) && ECS::ecs().HasComponent<Model>(entity) && ECS::ecs().HasComponent<Size>(entity) && ECS::ecs().HasComponent<Tex>(entity)) {
							Transform* transformData = &transformArray.GetData(entity);
							Model* modelData = &modelArray.GetData(entity);
							MainCharacter* mcData = &mcArray.GetData(entity);
							AnimationSet* animationData = &animationArray.GetData(entity);

							for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::MOVEMENT]) {
								switch (msg.type) {
								case(TYPE::DIALOGUE_ACTIVE):
									animationData->Stop();
									return;
								}
							}

							UpdateMovement(*transformData, *modelData);
							//Idle
							if (transformData->force.x == 0.f && transformData->force.y == 0.f) {
								if (mcData->moved) {
									//animationData->Stop();
								}
								mcData->moved = false;
							}
							//Moving
							else {
								if (!mcData->moved) {
									animationData->Start("Walk", entity);
								}
								mcData->moved = true;
							}

							if (transformData->force.x > 0.f) {
								modelData->SetMirror(false);
							}
							else if (transformData->force.x < 0.f) {
								modelData->SetMirror(true);
							}
							colliderArray.GetData(entity).type = Collider::MAIN;

							//camera.SetTarget(entity);

							camera.SetPos(std::clamp(transformData->position.x, boundaryMin.x + GRAPHICS::w, boundaryMax.x - GRAPHICS::w), std::clamp(transformData->position.y, boundaryMin.y + GRAPHICS::h, boundaryMax.y - GRAPHICS::h));
						}
					}
				}
			}
		}
	}
}

/******************************************************************************
*
*	@brief Handles animation for each entity, if any
*
*	Handles the animation for each entity, such as sprite cycling, if an entity
*	has any animation.
*
******************************************************************************/
void AnimationSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& animationArray = componentManager.GetComponentArrayRef<AnimationSet>();
	bool lockBattleSystem{ false };

	for (Entity const& entity : m_Entities) {
	/*for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		if (layersToSkip[layer_it] && layersToLock[layer_it]) {
			for (Entity& entity : layering[layer_it]) {
				if (entitiesToSkip[static_cast<uint32_t>(entity)] && entitiesToLock[static_cast<uint32_t>(entity)] && ECS::ecs().EntityExists(entity)) {
					if (ECS::ecs().HasComponent<AnimationSet>(entity) && ECS::ecs().HasComponent<Clone>(entity)) {*/
						AnimationSet* animationData = &animationArray.GetData(entity);
						animationData->Update(entity);

						//Lock the battle system if animation is playing
						if (animationData->activeAnimation != nullptr && animationData->activeAnimation->loop == false && animationData->activeAnimation->active == true) {
							lockBattleSystem = true;
						}
		//			}
		//		}
		//	}
		//}
	}
	if (lockBattleSystem) {
		Mail::mail().CreatePostcard(TYPE::ANIMATING, ADDRESS::ANIMATION, INFO::NONE, 0.f, 0.f);
		events.GetBattleSystem()->locked = true;
	}
	else {
		events.GetBattleSystem()->locked = false;
	}
}

/******************************************************************************
*
*	@brief Unused
*
*	Handles ...
*
******************************************************************************/
void ModelSystem::Update() {
	// Empty
}

/******************************************************************************
*
*	@brief Handles audio for the engine
*
*	Handles ...
*
******************************************************************************/
void AudioSystem::Update() {
	assetmanager.audio.Update();
}

/******************************************************************************
*
*	@brief Initializes Graphics System
*
*	-
*
******************************************************************************/
void GraphicsSystem::Initialize() {
	for (Entity const& entity : m_Entities) {
		Model* m = &ECS::ecs().GetComponent<Model>(entity);
		if (ECS::ecs().HasComponent<TextLabel>(entity)) {
			m->type = UI;
		}
		m->Update(ECS::ecs().GetComponent<Transform>(entity), ECS::ecs().GetComponent<Size>(entity));
	}
	camera.SetPos(0.f, 0.f);
	camera.SetZoom(1.f);
}

/******************************************************************************
*
*	@brief Sends Entity data to Graphics Manager
*
*	Sends the corresponding Entity data to the Graphics Manager (Graphics.cpp)
*	for rendering and display on screen.
*
******************************************************************************/
void GraphicsSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	auto& textArray = componentManager.GetComponentArrayRef<TextLabel>();

	//for (Entity const& entity : m_Entities) {
	for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		if (layersToSkip[layer_it] && layersToLock[layer_it]) {
			for (Entity& entity : layering[layer_it]) {
				if (entitiesToSkip[static_cast<uint32_t>(entity)] && entitiesToLock[static_cast<uint32_t>(entity)] && ECS::ecs().EntityExists(entity)) {
					if (ECS::ecs().HasComponent<Clone>(entity) && ECS::ecs().HasComponent<Transform>(entity) && ECS::ecs().HasComponent<Model>(entity) && ECS::ecs().HasComponent<Size>(entity)) {
						Model* m = &modelArray.GetData(entity);
						Size* size = &sizeArray.GetData(entity);
						Transform* transform = &transformArray.GetData(entity);
						/*if (m->CheckTransformUpdated(*transform, *size)) {

						}*/
						m->Update(*transform, *size);
					}
				}
			}
		}
	}

	//UPDATE FREE CAMERA MOVEMENT
	if (viewportWindowHovered) {
		for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::MOVEMENT]) {
			if (msg.type == TYPE::KEY_DOWN) {
				switch (msg.info) {
				case INFO::KEY_Y:   camera.AddZoom(0.1f * FIXED_DT);        break;
				case INFO::KEY_U:   camera.AddZoom(-0.1f * FIXED_DT);       break;
				case INFO::KEY_I:   camera.AddPos(0.f, 200.f * FIXED_DT);   break;
				case INFO::KEY_J:   camera.AddPos(-200.f * FIXED_DT, 0.f);  break;
				case INFO::KEY_K:   camera.AddPos(0, -200.f * FIXED_DT);    break;
				case INFO::KEY_L:   camera.AddPos(200.f * FIXED_DT, 0.f);   break;
				default: break;
				}
			}
		}
	}
	camera.Update();

	//FPS counter text
	static Entity fpsCounter{};
	static bool fpsCounterToggle{ false };
	std::stringstream fps{ };
	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::MOVEMENT]) {
		if (msg.type == TYPE::KEY_TRIGGERED) {
			switch (msg.info) {
			case INFO::KEY_9:   fpsCounterToggle = !fpsCounterToggle;        break;
			default: break;
			}
		}
	}
	bool fpsCounterExists{ ECS::ecs().EntityExists(fpsCounter) };
	if (!fpsCounterExists && fpsCounterToggle) {
		fpsCounter = EntityFactory::entityFactory().ClonePrefab("fps_counter.prefab");
	}
	else if (fpsCounterExists && !fpsCounterToggle) {
		EntityFactory::entityFactory().DeleteCloneModel(fpsCounter);
	}

	if (fpsCounterExists) {
		fps << 1 / g_dt;
		std::string fpsLabel{ "FPS: " + fps.str() };
		textArray.GetData(fpsCounter).textString = fpsLabel;
	}
}

/******************************************************************************
*
*	@brief Draws models onto screen
*
*	Displays entity data on screen.
*
******************************************************************************/
void GraphicsSystem::Draw() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	auto& textlabelArray = componentManager.GetComponentArrayRef<TextLabel>();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();

	graphics.viewport.Unuse();
	for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		if (layersToSkip[layer_it] || GetCurrentSystemMode() != SystemMode::EDIT) {
			for (size_t entity_it = 0; entity_it < layering[layer_it].size(); ++entity_it) {
				Entity entity = layering[layer_it][entity_it];
				if (entitiesToSkip[entity] || GetCurrentSystemMode() != SystemMode::EDIT) {
					Tex* tex{};
					Model* m{};
					if (modelArray.HasComponent(entity)) {
						m = &modelArray.GetData(entity);
						if (texArray.HasComponent(entity)) {
							tex = &texArray.GetData(entity);
						}
						m->Draw(tex);
						if (textlabelArray.HasComponent(entity)) {
							TextLabel* textLabelData = &textlabelArray.GetData(entity);
							graphics.DrawLabel(*textLabelData, textLabelData->textColor);
						}
					}
					else if (GetCurrentSystemMode() == SystemMode::EDIT && transformArray.HasComponent(entity)) {
						Transform* transform{ &transformArray.GetData(entity) };
						Name* name{ &nameArray.GetData(entity) };
						if (name->selected) {
							graphics.DrawCircle(transform->position.x, transform->position.y, GRAPHICS::DEBUG_CIRCLE_RADIUS, 0.f, 1.f, 0.f, 0.2f);
						}
						else {
							graphics.DrawCircle(transform->position.x, transform->position.y, GRAPHICS::DEBUG_CIRCLE_RADIUS, 1.f, 1.f, 1.f, 0.2f);
						}
					}
				}
			}
		}
	}

	if (GetCurrentSystemMode() == SystemMode::EDIT && snappingOn) {
		Renderer* render = &graphics.renderer["staticline"];
		for (auto& it : snappingLines) {
			for (auto& [axis, points] : it.second) {
				auto& [linePoint1, linePoint2, crossPoint1, crossPoint2] = points;
				graphics.DrawLine(linePoint1.x, linePoint1.y, linePoint2.x, linePoint2.y, 0.6f, 0.6f, 1.f, 1.f, render);
				graphics.DrawLine(crossPoint1.x - CROSS_SIZE, crossPoint1.y + CROSS_SIZE, crossPoint1.x + CROSS_SIZE, crossPoint1.y - CROSS_SIZE, 0.6f, 0.6f, 1.f, 1.f, render);
				graphics.DrawLine(crossPoint1.x - CROSS_SIZE, crossPoint1.y - CROSS_SIZE, crossPoint1.x + CROSS_SIZE, crossPoint1.y + CROSS_SIZE, 0.6f, 0.6f, 1.f, 1.f, render);
				graphics.DrawLine(crossPoint2.x - CROSS_SIZE, crossPoint2.y + CROSS_SIZE, crossPoint2.x + CROSS_SIZE, crossPoint2.y - CROSS_SIZE, 0.6f, 0.6f, 1.f, 1.f, render);
				graphics.DrawLine(crossPoint2.x - CROSS_SIZE, crossPoint2.y - CROSS_SIZE, crossPoint2.x + CROSS_SIZE, crossPoint2.y + CROSS_SIZE, 0.6f, 0.6f, 1.f, 1.f, render);
			}
		}
	}

	if (GetCurrentSystemMode() == SystemMode::EDIT && tilemapOn) {
		int timesX = static_cast<int>(((static_cast<float>(std::abs(gridOffsetX)) / 100.f * static_cast<float>(gridSpacingX)) + (static_cast<float>(GRAPHICS::viewportWidth) / 2.f / camera.GetZoom())) / static_cast<float>(gridSpacingX));
		for (int x = -timesX + static_cast<int>(camera.GetPos().x / static_cast<float>(gridSpacingX)) - 1; x < timesX + static_cast<int>(camera.GetPos().x / static_cast<float>(gridSpacingX)) + 2; ++x) {
			graphics.DrawLine(
				static_cast<float>(x * gridSpacingX) + (static_cast<float>(gridOffsetX) / 100.f * static_cast<float>(gridSpacingX)),
				static_cast<float>(GRAPHICS::viewportHeight) / 2.f / camera.GetZoom() + camera.GetPos().y,
				static_cast<float>(x * gridSpacingX) + (static_cast<float>(gridOffsetX) / 100.f * static_cast<float>(gridSpacingX)),
				static_cast<float>(-GRAPHICS::viewportHeight) / 2.f / camera.GetZoom() + camera.GetPos().y, 0.7f, 0.7f, 0.7f, 0.2f, nullptr
			);
		}

		int timesY = static_cast<int>(((static_cast<float>(std::abs(gridOffsetY)) / 100.f * static_cast<float>(gridSpacingY)) + (static_cast<float>(GRAPHICS::viewportHeight) / 2.f / camera.GetZoom())) / static_cast<float>(gridSpacingY));
		for (int y = -timesY + static_cast<int>(camera.GetPos().y / static_cast<float>(gridSpacingY)) - 1; y < timesY + static_cast<int>(camera.GetPos().y / static_cast<float>(gridSpacingY)) + 2; ++y) {
			graphics.DrawLine(
				static_cast<float>(GRAPHICS::viewportWidth) / 2.f / camera.GetZoom() + camera.GetPos().x,
				static_cast<float>(y * gridSpacingY) + (static_cast<float>(gridOffsetY) / 100.f * static_cast<float>(gridSpacingY)),
				static_cast<float>(-GRAPHICS::viewportWidth) / 2.f / camera.GetZoom() + camera.GetPos().x,
				static_cast<float>(y * gridSpacingY) + (static_cast<float>(gridOffsetY) / 100.f * static_cast<float>(gridSpacingY)), 0.7f, 0.7f, 0.7f, 0.2f, nullptr
			);
		}
	}

	graphics.Draw();

}

/******************************************************************************
*
*	@brief Sends Entity array data to Serializer
*
*	Sends the corresponding Entity array data to the Serializer
*	(Serialization.cpp) for saving into	file.
*
******************************************************************************/
void SerializationSystem::Update() {
	if (saveFile) {
		PrepareLayeringForSerialization();
		//EmbedSkipLockForSerialization();
		std::string scenePath{ SaveFileDialog("*.scn","Scene File") };
		if (scenePath != "") {
			assetmanager.SaveScene(scenePath);
		}
		saveFile = false;
	}

	if (destroyAll) {
		std::vector<Entity> entitylist{};
		EntityFactory::entityFactory().masterEntitiesList.clear();
		for (Entity e : m_Entities) {
			entitylist.push_back(e);
		}
		for (Entity e : entitylist) {
			//ECS::ecs().DestroyEntity(e);
			EntityFactory::entityFactory().DeleteCloneModel(e);
		}
		destroyAll = false;
	}

	if (newScene) {
		assetmanager.UnloadAll();
		if (newSceneName != "") {
			assetmanager.LoadAssets(newSceneName);
		}
		initLevel = true;
		newScene = false;
		if (GetCurrentSystemMode() == SystemMode::PAUSE) {
			SetCurrentSystemMode(SystemMode::RUN);
		}
		assetmanager.audio.ResumeGroup("BGM");
		assetmanager.audio.ResumeGroup("SFX");
		EngineCore::engineCore().set_m_previousTime(GetTime());
	}

	if (playButton) {

		std::string scenePath{ assetmanager.GetDefaultPath() + "Scenes/tmp.scn" };
		if (scenePath != "") {
			std::ofstream sceneFile{ scenePath.c_str() };

			std::string jsonPath{ assetmanager.GetDefaultPath() + "Scenes/tmp.json" };
			std::vector<Entity> entityList{};
			for (auto& e : m_Entities) {
				entityList.push_back(e);
			}
			Serializer::SaveEntityToJson(jsonPath, entityList);

			auto files = assetmanager.GetFiles();
			for (auto& f : files) {
				sceneFile << f << "\n";
			}
			sceneFile << "tmp.json";
			sceneFile.close();
		}

		//initLevel = true;
		playButton = false;
	}


}

/******************************************************************************
*
*	@brief Initialies the Script System
*
*   This function initializes the Script System by calling the ScriptEngine's
*   ScriptInit function for each entity with a script component.
*
******************************************************************************/
bool FirstInitScriptSystem = true;
void ScriptSystem::Initialize() {
	std::unordered_map<Entity, std::vector<std::string>> scriptMap;

	// Iterate through all entities with a script component
	for (const Entity& entity : m_Entities) {

		// Get the script component
		Script* s = &ECS::ecs().GetComponent<Script>(entity);
		if (!s) {
			// If there's no script associated with the entity, just continue to the next iteration.
			continue;
		}

		// Only allows the script system to init once
		if (FirstInitScriptSystem) {
			ScriptEngine::ScriptInit(entity);
			FirstInitScriptSystem = false;
		}
	}
}

/******************************************************************************
*
*   @brief Updates the Script System
*
*   This function updates the Script System by calling the ScriptEngine's
*   ScriptUpdate function for each entity with a script component.
*
******************************************************************************/
void ScriptSystem::Update() {

	ComponentManager& componentManager = ECS::ecs().GetComponentManager();
	auto& scriptArray = componentManager.GetComponentArrayRef<Script>();

	for (Entity const& entity : m_Entities) {

		Script* scriptData = &scriptArray.GetData(entity);

		for (size_t i = 0; i < scriptData->scriptNameVec.size(); ++i) {
			ScriptEngine::ScriptUpdate(entity);

		}

	}

}


/******************************************************************************
*
*	@brief Update Function for the Editing System
*
*	Sends the corresponding Entity data to the Editing System (Editing.cpp)
*
******************************************************************************/
void EditingSystem::Update() {

	thereWasAClickThisCycle = false;
	somethingWasSelectedThisCycle = false;

	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	//auto& colorArray = componentManager.GetComponentArrayRef<Color>();

	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::EDITING]) {
		switch (msg.type) {
		case TYPE::KEY_TRIGGERED: {
			switch (msg.info) {

			case INFO::KEY_RSHIFT:
			case INFO::KEY_LSHIFT:
				shiftKeyPressed = true;
				break;
			case INFO::KEY_RCTRL:
			case INFO::KEY_LCTRL:
				controlKeyPressed = true;
				break;
			case INFO::KEY_G:
				if (controlKeyPressed && !shiftKeyPressed && (selectedEntities.size() > 1)) {
					GroupSelection();
				}
				else if (controlKeyPressed && shiftKeyPressed && (selectedEntities.size() > 1)) {
					UngroupSelection();
				}
				break;
			case INFO::KEY_Z:
				if (controlKeyPressed) {
					undoRedo.Undo();
				}
				break;
			}
			break;

		case TYPE::KEY_UP: {
			switch (msg.info) {
			case INFO::KEY_RSHIFT:
			case INFO::KEY_LSHIFT:
				shiftKeyPressed = false;
				break;
			case INFO::KEY_RCTRL:
			case INFO::KEY_LCTRL:
				controlKeyPressed = false;
				break;
			}
		}
						 break;
		}

		case TYPE::MOUSE_MOVE:
			prevMousePosition = currentMousePosition;
			currentMousePosition = { msg.posX, msg.posY };

			for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
				if (layersToSkip[layer_it] && layersToLock[layer_it]) {
					for (Entity& entity : layering[layer_it]) {
						if (entitiesToSkip[static_cast<uint32_t>(entity)] && entitiesToLock[static_cast<uint32_t>(entity)] && ECS::ecs().EntityExists(entity)) {
							Name& n = nameArray.GetData(entity);
							if (n.selected) {
								if (modelArray.HasComponent(entity)) {
									Model& m = modelArray.GetData(entity);
									if (IsNearby(m.GetMax(), currentMousePosition, CORNER_SIZE) || IsNearby(m.GetMin(), currentMousePosition, CORNER_SIZE) || IsNearby({ m.GetMax().x, m.GetMin().y }, currentMousePosition, CORNER_SIZE) || IsNearby({ m.GetMin().x, m.GetMax().y }, currentMousePosition, CORNER_SIZE) || IsWithinObject(m, currentMousePosition)) {
										withinSomething = true;
									}
								}
								else {
									Transform& t = transformArray.GetData(entity);
									if (t.position.distance(currentMousePosition) < GRAPHICS::DEBUG_CIRCLE_RADIUS) {
										withinSomething = true;
									}
								}
							}
						}
					}
				}
			}

			mouseMoved = true;
			break;
		}
	}

	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::EDITING]) {
		switch (msg.type) {
		case TYPE::MOUSE_CLICK:


			for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
				for (Entity& entity : layering[layer_it]) {
					if (!ECS::ecs().EntityExists(entity)) {
						continue;
					}
					Name& n = nameArray.GetData(entity);
					Transform& t = transformArray.GetData(entity);
					n.draggingOffset = GetOffset(t.position, currentMousePosition);
				}
			}
			break;
		}
	}

	// Selection starts here

	for (int layer_it = static_cast<int>(layering.size() - 1); layer_it >= 0; --layer_it) {
		if (layersToSkip[layer_it] && layersToLock[layer_it]) {
			for (int entity_it = static_cast<int>(layering[layer_it].size() - 1); entity_it >= 0; --entity_it) {
				Entity entity = layering[layer_it][entity_it];
				if (entitiesToSkip[static_cast<uint32_t>(entity)] && entitiesToLock[static_cast<uint32_t>(entity)] && ECS::ecs().EntityExists(entity)) {
					Name& n = nameArray.GetData(entity);
					Transform& t = transformArray.GetData(entity);
					Model* m{};
					if (modelArray.HasComponent(entity)) {
						m = &modelArray.GetData(entity);
					}
					Selection(entity, n, t, *m, static_cast<size_t>(layer_it));
					if (somethingWasSelectedThisCycle) {
						break;
					}
				}
			}
		}
		if (somethingWasSelectedThisCycle) {
			break;
		}
	}

	if (thereWasAClickThisCycle && !somethingWasSelectedThisCycle) {
		UnselectAll();
	}

	if (toCopy) {
		for (Entity entity : selectedEntities) {
			EntityFactory::entityFactory().CloneMaster(entity);
		}
		toCopy = false;
		selectedEntities.clear();
		UnselectAll();
	}


	if (toDestroy) {
		for (Entity entity : selectedEntities) {
			undoRedo.RecordCurrent(entity, ACTION::DELENTITY);
			ECS::ecs().RemoveComponent<Clone>(entity);
			entitiesToSkip[entity] = false;
			entitiesToLock[entity] = false;
		}
		toDestroy = false;
		selectedEntities.clear();
		UnselectAll();
		selectedLayer = std::numeric_limits<size_t>::max();
	}


	selectedEntities.clear();
	for (Entity entity : m_Entities) {
		if (nameArray.GetData(entity).selected) {
			selectedEntities.emplace_back(entity);
		}
	}

	if (keyObjectID != std::numeric_limits<Entity>().max()) {
		bool found = false;
		for (Entity entity : selectedEntities) {
			if (keyObjectID == entity && modelArray.HasComponent(keyObjectID)) {
				found = true;
				break;
			}
		}
		if (found) {
			//change color
			modelArray.GetData(keyObjectID).GetColorRef() = { 0.5f, 1.f, 0.5f, 1.f }; // GREEN
		}
		else if (keyObjectID != std::numeric_limits<Entity>().max()) {
			// change back color
			modelArray.GetData(keyObjectID).GetColorRef() = keyObjectColor;
			keyObjectID = std::numeric_limits<Entity>().max();
			keyObjectColor = { RESET_VEC4 };
		}
	}

	// Editing starts here

	for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		if (layersToSkip[layer_it] && layersToLock[layer_it]) {
			for (Entity& entity : layering[layer_it]) {
				if (entitiesToSkip[static_cast<uint32_t>(entity)] && entitiesToLock[static_cast<uint32_t>(entity)] && ECS::ecs().EntityExists(entity)) {
					Name& n = nameArray.GetData(entity);
					Transform& t = transformArray.GetData(entity);
					Model* m{};
					Size& s = sizeArray.GetData(entity);

					if (modelArray.HasComponent(entity)) {
						m = &modelArray.GetData(entity);
					}
					// edit entity's properties
					UpdateProperties(entity, n, t, s, m, layer_it);
				}
			}
		}
	}


	for (auto& [e, color] : snappingHighlight) {
		if (modelArray.HasComponent(e)) {
			modelArray.GetData(e).GetColorRef() = glm::vec4(1.f, 0.5f, 0.5f, 1.f); // Red;
		}
	}

	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::EDITING]) {
		switch (msg.type) {
		case TYPE::MOUSE_UP:
			draggingThisCycle = false;
			break;
		}
	}

	mouseMoved = false;
	if (!draggingThisCycle) {
		withinSomething = false;
	}
}


/******************************************************************************
*
*	@brief Draws the green outline for selected entities
* 
*	Includes drawing of red outline for entities that are being snapped to
*
******************************************************************************/
void EditingSystem::Draw() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();

	for (size_t layer_it = 0; layer_it < layering.size(); ++layer_it) {
		if (layersToSkip[layer_it] && layersToLock[layer_it]) {
			for (Entity& entity : layering[layer_it]) {
				if (entitiesToSkip[static_cast<uint32_t>(entity)] && entitiesToLock[static_cast<uint32_t>(entity)] && ECS::ecs().EntityExists(entity)) {
					Name* n = &nameArray.GetData(entity);
					if (n->selected && modelArray.HasComponent(entity)) {
						Model* m = &modelArray.GetData(entity);
						m->DrawOutline();
					}
				}
			}
		}
	}

	if (snappingOn) {

		for (auto& [e, color] : snappingHighlight) {
			if (modelArray.HasComponent(e)) {
				modelArray.GetData(e).DrawOutlineRed();
			}
		}

	}
	
}

/******************************************************************************
*
*	@brief Battle System is located in Battle.cpp
* 
*	-
*
******************************************************************************/


/******************************************************************************
*
*	@brief Refer to UITextLabelSystem::Draw() function
* 
*	-
*
******************************************************************************/
void UITextLabelSystem::Update() {
	//ARCHIVED: MOVED OVER TO DRAW FUNCTION AS OFFSETS ONLY CALCULATED AFTER MODELS UPDATES ARE DONE
}

/******************************************************************************
*
*	@brief Determines offset required from parent entity for drawing by graphics
* 
*	-
*
******************************************************************************/
void UITextLabelSystem::Draw() {
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	//// Access component arrays through the ComponentManager
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	auto& textLabelArray = componentManager.GetComponentArrayRef<TextLabel>();
	auto& buttonArray = componentManager.GetComponentArrayRef<Button>();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();

	for (Entity const& entity : m_Entities) {
		Model* modelData = &modelArray.GetData(entity);
		Size* sizeData = &sizeArray.GetData(entity);
		Tex* texData = nullptr;
		TextLabel* textLabelData = &textLabelArray.GetData(entity);
		Button* buttonData = nullptr;
		Name* nameData = &nameArray.GetData(entity);
		Transform* transformData = &transformArray.GetData(entity);

		//if entity has button component, drawing managed by button
		if (buttonArray.HasComponent(entity)) {
			buttonData = &buttonArray.GetData(entity);
			textLabelData->UpdateOffset(*transformData, *sizeData, buttonData->padding);
		}
		else {
			/*sizeData->width = std::max(textLabelData->textWidth, sizeData->width);
			sizeData->height = std::max(textLabelData->textHeight, sizeData->height);*/
			textLabelData->Update(*modelData, *nameData);
			textLabelData->UpdateOffset(*transformData, *sizeData);
		}

		if (texArray.HasComponent(entity)) {
			texData = &texArray.GetData(entity);
		}

		if (!buttonData && !texData) {
			if (GetCurrentSystemMode() == SystemMode::EDIT) {
				(textLabelData->hasBackground) ? modelData->SetAlpha(modelData->GetAlpha())
					: (textLabelData->currentState == STATE::NONE) ? modelData->SetAlpha(0.0f)
					: modelData->SetAlpha(0.2f);
			}
			else {
				(textLabelData->hasBackground) ? modelData->SetAlpha(modelData->GetAlpha())
					: modelData->SetAlpha(0.0f);
			}
		}
	}
}

/******************************************************************************
*
*	@brief Updates Button components' state, color and size
* 
*	-
*
******************************************************************************/
void UIButtonSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	//auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& nameArray = componentManager.GetComponentArrayRef<Name>();
	auto& textLabelArray = componentManager.GetComponentArrayRef<TextLabel>();
	auto& buttonArray = componentManager.GetComponentArrayRef<Button>();

	BattleSystem* battleSys = events.GetBattleSystem();
	bool updateBattleInfoButton{ battleSys && !ECS::ecs().EntityExists(battleSys->battleInfoButton) };

	for (Entity const& entity : m_Entities) {
		//Size* sizeData = &sizeArray.GetData(entity);
		Name* nameData = &nameArray.GetData(entity);
		Model* modelData = &modelArray.GetData(entity);
		TextLabel* textLabelData = &textLabelArray.GetData(entity);
		Button* buttonData = &buttonArray.GetData(entity);

		buttonData->Update(*modelData, *nameData, *textLabelData, entity);

		glm::vec4 btnColor = (GetCurrentSystemMode() == SystemMode::EDIT) ? buttonData->GetDefaultButtonColor() : buttonData->GetButtonColor();
		modelData->SetColor(btnColor.r, btnColor.g, btnColor.b);
		modelData->SetAlpha(btnColor.a);

		if (updateBattleInfoButton && buttonData->eventName == "Toggle Battle Info") {
			battleSys->battleInfoButton = entity;
			battleSys->allBattleUI.push_back(entity);
		}
	}
}

/******************************************************************************
*
*	@brief Updates the HP value and its child components
* 
*	-
*
******************************************************************************/
void UIHealthBarSystem::Update() {

	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	auto& textLabelArray = componentManager.GetComponentArrayRef<TextLabel>();
	auto& healthBarArray = componentManager.GetComponentArrayRef<HealthBar>();
	auto& healthRemainingArray = componentManager.GetComponentArrayRef<HealthRemaining>();
	auto& parentArray = componentManager.GetComponentArrayRef<Parent>();
	auto& childArray = componentManager.GetComponentArrayRef<Child>();

	for (Entity const& entity : m_Entities) {
		Size* pSizeData = &sizeArray.GetData(entity);
		HealthBar* healthBarData = &healthBarArray.GetData(entity);
		Parent* parentData = &parentArray.GetData(entity);

		healthBarData->UpdateHealth();

		if (parentData->children.empty())
			continue;

		for (int count = 0; count < parentData->children.size(); count++) {
			Entity childEntity = parentData->children[count];

			if (healthRemainingArray.HasComponent(childEntity)) {
				HealthRemaining* healthRemainingData = &healthRemainingArray.GetData(childEntity);
				Model* childModel = &modelArray.GetData(childEntity);
				Child* childData = &childArray.GetData(childEntity);
				Size* cSizeData = &sizeArray.GetData(childEntity);
				healthRemainingData->currentHealth = healthBarData->currentHealth;
				healthRemainingData->UpdateSize(*healthBarData, *pSizeData, *cSizeData);
				healthRemainingData->UpdateColors(*childModel, *healthBarData->charaStatsRef);
				healthRemainingData->UpdateOffset(*pSizeData, *healthBarData, *childData);
			}
			if (textLabelArray.HasComponent(childEntity)) {
				TextLabel* textLabelData = &textLabelArray.GetData(childEntity);
				healthBarData->UpdateTextDisplay(*textLabelData);
			}
		}
	}
}

/******************************************************************************
*
*	@brief Updates skill point (chi) balance and toggles animation set accordingly
* 
*	-
*
******************************************************************************/
void UISkillPointSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& animationSetArray = componentManager.GetComponentArrayRef<AnimationSet>();
	auto& textLabelArray = componentManager.GetComponentArrayRef<TextLabel>();
	auto& skillPtHudArray = componentManager.GetComponentArrayRef<SkillPointHUD>();
	auto& skillPtArray = componentManager.GetComponentArrayRef<SkillPoint>();
	auto& parentArray = componentManager.GetComponentArrayRef<Parent>();

	BattleSystem* battleSys = events.GetBattleSystem();
	if (battleSys && battleSys->battleState == PLAYERTURN && battleSys->activeCharacter->tag == CharacterType::PLAYER) {
		for (Entity const& entity : m_Entities) {
			TextLabel* textLabelData = &textLabelArray.GetData(entity);
			SkillPointHUD* skillPtHudData = &skillPtHudArray.GetData(entity);
			Parent* parentData = &parentArray.GetData(entity);

			skillPtHudData->UpdateBalance();
			textLabelData->SetTextString(std::to_string(skillPtHudData->skillPointBalance));

		if (parentData->children.empty())
			continue;

		for (int count = 0; count < parentData->children.size(); count++) {
			Entity childEntity = parentData->children[count];

			if (!skillPtArray.HasComponent(childEntity))
				continue;

			SkillPoint* skillPtData = &skillPtArray.GetData(childEntity);
			AnimationSet* aniSetData = &animationSetArray.GetData(childEntity);
			if (skillPtData->isActive == (count < skillPtHudData->skillPointBalance))
				continue;

				skillPtData->isActive = !(skillPtData->isActive);

				//note: changes will be reflected outside of edit mode!
				(skillPtData->isActive) ?
					aniSetData->Start("Active", childEntity)
					: aniSetData->Start("Inactive", childEntity);
			}
		}
	}
}

/******************************************************************************
*
*	@brief Updates Attackskill buttons' state, event call and its child components
*
******************************************************************************/
void UIAttackSkillSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& texArray = componentManager.GetComponentArrayRef<Tex>();
	auto& textLabelArray = componentManager.GetComponentArrayRef<TextLabel>();
	auto& buttonArray = componentManager.GetComponentArrayRef<Button>();
	auto& atkSkillArray = componentManager.GetComponentArrayRef<AttackSkill>();
	auto& skillAtkTypeArray = componentManager.GetComponentArrayRef<SkillAttackType>();
	auto& skillCostArray = componentManager.GetComponentArrayRef<SkillCost>();
	auto& parentArray = componentManager.GetComponentArrayRef<Parent>();

	BattleSystem* battleSys = events.GetBattleSystem();
	if (battleSys && battleSys->battleState == PLAYERTURN && battleSys->activeCharacter->tag == CharacterType::PLAYER) {
		std::vector<Attack>* characterSkills = &battleSys->activeCharacter->action.skills;

		for (Entity const& entity : m_Entities) {
			AttackSkill* atkSkillData = &atkSkillArray.GetData(entity);
			Button* buttonData = &buttonArray.GetData(entity);
			Parent* parentData = &parentArray.GetData(entity);

			// update skill texture (SkillIcon component)
			int chiBalance = battleSys->chi;
			bool isSufficient = (chiBalance >= (*characterSkills)[atkSkillData->skillIndex].chiCost);
			
			// function to update button trigger
			atkSkillData->UpdateSkillEvent(*buttonData);

			// function to handle state if player has sufficient chi
			atkSkillData->UpdateButtonState(*buttonData, isSufficient);

			if (parentData->children.empty())
				continue;

			for (int count = 0; count < parentData->children.size(); count++) {
				Entity childEntity = parentData->children[count];

				// update skill attack type (SkillAttackType + TextLabel/Tex components)
				// function to update attack type text label
				if (skillAtkTypeArray.HasComponent(childEntity) && textLabelArray.HasComponent(childEntity)) {
					TextLabel* textLabelData = &textLabelArray.GetData(childEntity);
					atkSkillData->UpdateAtkTypeLbl(*textLabelData, (*characterSkills)[atkSkillData->skillIndex].attacktype);
					continue;
				}
				// function to update icon
				if (skillAtkTypeArray.HasComponent(childEntity) && texArray.HasComponent(childEntity)) {
					continue;
				}

				// update chi cost (SkillCost + TextLabel components)
				// function to update textlabel
				if (skillCostArray.HasComponent(childEntity) && textLabelArray.HasComponent(childEntity)) {
					TextLabel* textLabelData = &textLabelArray.GetData(childEntity);
					atkSkillData->UpdateSkillCostLbl(*textLabelData, (*characterSkills)[atkSkillData->skillIndex].chiCost);
					continue;
				}
			}
		}
	}
}

/******************************************************************************
*
*	@brief Updates AllyHUD
*
*	Updates parent AllyHUD referencing characterStats stored in HealthBar 
*	component where it will be used most often (theoretical rationale: if ally
*	falls then the index ref will update to the next alive ally, etc.)
*
******************************************************************************/
void UIAllyHudSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& allyHudArray = componentManager.GetComponentArrayRef<AllyHUD>();
	auto& healthBarArray = componentManager.GetComponentArrayRef<HealthBar>();
	//auto& characterStatsArray = componentManager.GetComponentArrayRef<CharacterStats>();

	BattleSystem* battleSys = events.GetBattleSystem();
	if (battleSys) {
		std::vector<CharacterStats*> allPlayers = battleSys->GetPlayers();
		for (Entity const& entity : m_Entities) {
			AllyHUD* allyHudData = &allyHudArray.GetData(entity);
			HealthBar* healthBarData = &healthBarArray.GetData(entity);
			bool checkResult = false;
			allyHudData->CheckValidIndex(static_cast<int>(allPlayers.size()), checkResult);
			if (checkResult) {
				if (!allyHudData->initialised) {
					allyHudData->initialised = true;
					//healthBarData->charaStatsRef = allPlayers[allyHudData->allyIndex];
				}
				if (healthBarData->charaStatsRef != nullptr) {
					allyHudData->ToggleStatusFx(entity, healthBarData->charaStatsRef);
				}
			}
			if (battleSys->battleState == WIN || battleSys->battleState == LOSE) {
				healthBarData->charaStatsRef = nullptr;
			}
		}
	}
}

/******************************************************************************
*
*	@brief Updates parent EnemyHUD
*
*	Updates parent EnemyHUD referencing characterStats stored in HealthBar
*	component where it will be used most often. Also updates status effect
*	display.
*
******************************************************************************/
void UIEnemyHudSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& enemyHudArray = componentManager.GetComponentArrayRef<EnemyHUD>();
	auto& healthBarArray = componentManager.GetComponentArrayRef<HealthBar>();
	//auto& characterStatsArray = componentManager.GetComponentArrayRef<CharacterStats>();

	BattleSystem* battleSys = events.GetBattleSystem();
	if (battleSys) {
		std::vector<CharacterStats*> allEnemies = battleSys->GetEnemies();
		for (Entity const& entity : m_Entities) {
			EnemyHUD* enemyHudData = &enemyHudArray.GetData(entity);
			HealthBar* healthBarData = &healthBarArray.GetData(entity);
			bool checkResult = false;
			enemyHudData->CheckValidIndex(static_cast<int>(allEnemies.size()), checkResult);
			if (checkResult) {
				if (!enemyHudData->initialised) {
					enemyHudData->initialised = true;
					//healthBarData->charaStatsRef = allEnemies[enemyHudData->enemyIndex];
				}
				if (healthBarData->charaStatsRef != nullptr) {
					enemyHudData->ToggleStatusFx(entity, healthBarData->charaStatsRef);
				}
			}
			else {
				enemyHudData->ToggleStatusFx(entity, 0);
			}
			if (battleSys->battleState == WIN || battleSys->battleState == LOSE) {
				//healthBarData->charaStatsRef = nullptr;
			}
		}
	}
}

/******************************************************************************
*
*	@brief Updates status effect stacks and position based off parent entity
* 
*	-
*
******************************************************************************/
void UIEffectSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	//auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	//auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	auto& textLabelArray = componentManager.GetComponentArrayRef<TextLabel>();
	auto& healthBarArray = componentManager.GetComponentArrayRef<HealthBar>();
	auto& statusFxArray = componentManager.GetComponentArrayRef<StatusEffect>();

	BattleSystem* battleSys = events.GetBattleSystem();
	if (battleSys) {
		for (Entity const& entity : m_Entities) {
			//Transform* transformData = &transformArray.GetData(entity);
			StatusEffect* statusFxData = &statusFxArray.GetData(entity);

			statusFxData->UpdateOffset(entity);

			if (textLabelArray.HasComponent(entity)) {
				HealthBar* parentHealth = &healthBarArray.GetData(statusFxData->character);
				TextLabel* textLabelData = &textLabelArray.GetData(entity);
				if (parentHealth->charaStatsRef != nullptr) {
					statusFxData->UpdateStacksLbl(*textLabelData, parentHealth->charaStatsRef);
				}
			}
		}
	}
}

/******************************************************************************
*
*	@brief Initializes dialogue system
*
*	-
*
******************************************************************************/
void UIDialogueSystem::Initialize() {
	events.ConnectDialogueSystem(this);
}

/******************************************************************************
*
*	@brief Updates parent DialogueHUD
*
*	-
*
******************************************************************************/
void UIDialogueSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& modelArray = componentManager.GetComponentArrayRef<Model>();
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& sizeArray = componentManager.GetComponentArrayRef<Size>();
	auto& textLabelArray = componentManager.GetComponentArrayRef<TextLabel>();
	auto& dialogueSpeakerArray = componentManager.GetComponentArrayRef<DialogueSpeaker>();
	auto& dialogueHudArray = componentManager.GetComponentArrayRef<DialogueHUD>();
	auto& parentArray = componentManager.GetComponentArrayRef<Parent>();
	auto& childArray = componentManager.GetComponentArrayRef<Child>();
	auto& animationArray = componentManager.GetComponentArrayRef<AnimationSet>();
	auto& cloneArray = componentManager.GetComponentArrayRef<Clone>();

	for (Entity const& entity : m_Entities) {
		Parent* parentData = &parentArray.GetData(entity);
		DialogueHUD* dialogueHudData = &dialogueHudArray.GetData(entity);
		Model* modelData = &modelArray.GetData(entity);
		Transform* transformData = &transformArray.GetData(entity);
		Size* sizeData = &sizeArray.GetData(entity);

		if (dialogueHudData->dialogues.empty())
		{
			continue;
		}

		if (dialogueHudData->dialogueQueue.empty())
		{
			// push dialogue lines into queue (dialogues that are not triggered)
			for (DialogueHUD::Dialogue& dialogue : dialogueHudData->dialogues)
			{
				if (!dialogue.isTriggered && (&dialogue != dialogueHudData->currentDialogue))
					dialogueHudData->dialogueQueue.push(&dialogue);
			}
		}

		// assign dialogue pointer if empty
		if (!dialogueHudData->currentDialogue && !dialogueHudData->dialogueQueue.empty())
		{
			dialogueHudData->currentDialogue = dialogueHudData->dialogueQueue.top();
			dialogueHudData->dialogueQueue.pop();
		}

		if (dialogueHudData->currentDialogue->isActive) {
			Mail::mail().CreatePostcard(TYPE::DIALOGUE_ACTIVE, ADDRESS::DIALOGUE, INFO::NONE, 0.f, 0.f);
		}

		if (GetCurrentSystemMode() == SystemMode::RUN && !dialogueHudData->currentDialogue->isActive && dialogueHudData->currentDialogue->dialogueLines.size()) {
			// if dialogue is NOT triggered and triggerType is autoLaunch, start dialogue
			if (!dialogueHudData->currentDialogue->isTriggered && (dialogueHudData->currentDialogue->triggerType == DIALOGUE_TRIGGER::AUTO_LAUNCH))
			{
				dialogueHudData->currentDialogue->viewingIndex = 0;
				dialogueHudData->StartDialogue(entity, DIALOGUE_TRIGGER::AUTO_LAUNCH);
			}
			// else if dialogue is triggered and has animation set, transition to next scene after animation
			else if (dialogueHudData->currentDialogue->isTriggered && animationArray.HasComponent(entity)) {
				if (dialogueHudData->currentDialogue->postDialogueScene							// if postDialogueScene is set
					&& !dialogueHudData->currentDialogue->targetScene.empty()					// if targetScene is set
					&& !animationArray.GetData(entity).activeAnimation->active) // if animation has finished playing
				{
					events.Call("Transition Scene", dialogueHudData->currentDialogue->targetScene);
				}
			}
		}

		if (GetCurrentSystemMode() == SystemMode::EDIT && dialogueHudData->isEditing)
		{
			transformData->position.x = 0.0f;
			transformData->position.y = -350.0f;
		}

		// event handling if need to advance to next line
		dialogueHudData->Update(*modelData, entity);

		if (!dialogueHudData->currentDialogue || dialogueHudData->currentDialogue->dialogueLines.empty())
		{
			continue;
		}

		// load text data into text labels
		if (!(parentData->children.empty())) {
			Entity childEntity = parentData->children.front();

			if (dialogueHudData->currentDialogue->isActive &&
				dialogueSpeakerArray.HasComponent(childEntity) && textLabelArray.HasComponent(childEntity)) {
				Child* childData = &childArray.GetData(childEntity);
				TextLabel* speakerTextData = &textLabelArray.GetData(childEntity);
				Size* speakerSizeData = &sizeArray.GetData(childEntity);
				Model* speakerModelData = &modelArray.GetData(childEntity);
				speakerTextData->textString = (!dialogueHudData->currentDialogue->dialogueLines.empty()) ? dialogueHudData->currentDialogue->dialogueLines[dialogueHudData->currentDialogue->viewingIndex].speaker : "";

				if (speakerTextData->textString == "" && cloneArray.HasComponent(childEntity))
				{
					speakerSizeData->height = 0.001f;
					speakerSizeData->width = 0.001f;
				}
				speakerTextData->hasBackground = dialogueHudData->currentDialogue->speakerRequired ? true : false;
				float parentAlpha = modelData->GetAlpha();
				speakerModelData->SetAlpha(parentAlpha);

				if (!dialogueHudData->currentDialogue->dialogueLines[dialogueHudData->currentDialogue->viewingIndex].updated) {
					events.Call("Stop Group", "VOC");
					events.Call("Play Voice", dialogueHudData->currentDialogue->dialogueLines[dialogueHudData->currentDialogue->viewingIndex].voice);

					dialogueHudData->currentDialogue->dialogueLines[dialogueHudData->currentDialogue->viewingIndex].updated = true;
				}

				dialogueHudData->EnforceAlignment(*sizeData, *speakerSizeData, *speakerTextData, *childData);
			}
		}
		TextLabel* dialogueTextData = &textLabelArray.GetData(entity);
		dialogueTextData->textString = (!dialogueHudData->currentDialogue->dialogueLines.empty()) ? dialogueHudData->currentDialogue->dialogueLines[dialogueHudData->currentDialogue->viewingIndex].line : "";
		dialogueTextData->hasBackground = dialogueHudData->currentDialogue->speakerRequired ? true : false;
	}
}

/******************************************************************************
*
*	@brief Updates transform of child entities based on its parent entity
* 
*	-
*
******************************************************************************/
void ChildSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& transformArray = componentManager.GetComponentArrayRef<Transform>();
	auto& childArray = componentManager.GetComponentArrayRef<Child>();
	//auto& cloneArray = componentManager.GetComponentArrayRef<Clone>();

	for (Entity const& entity : m_Entities) {
		Child* childData = &childArray.GetData(entity);
		Entity parent = childData->parent;

		if (!ECS::ecs().EntityExists(parent)) {
			EntityFactory::entityFactory().DeleteCloneModel(entity);
			continue;
		}

		Transform* childTransform = &transformArray.GetData(entity);
		Transform* parentTransform = &transformArray.GetData(parent);

		*childTransform = childData->offset + *parentTransform;
	}
}

/******************************************************************************
*
*	@brief Updates vector of holding its child entities
* 
*	-
*
******************************************************************************/
void ParentSystem::Update() {
	// Access the ComponentManager through the ECS class
	ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	// Access component arrays through the ComponentManager
	auto& parentArray = componentManager.GetComponentArrayRef<Parent>();
	auto& childArray = componentManager.GetComponentArrayRef<Child>();

	for (Entity const& entity : m_Entities) {
		Parent* parentData = &parentArray.GetData(entity);

		std::unordered_set <Entity> childrenToRemove{};
		for (auto& child : parentData->children) {
			if (!childArray.HasComponent(child)) {
				childrenToRemove.insert(child);
			}
		}
		if (!childrenToRemove.empty()) {
			std::vector<Entity> newChildArray{};
			for (auto& child : parentData->children) {
				if (!childrenToRemove.count(child)) {
					newChildArray.push_back(child);
				}
			}
			parentData->children = newChildArray;
		}
	}
}
