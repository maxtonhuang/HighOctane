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
*	@file		Tutorial.cpp
*
*	@co-author	Chua Zhen Rong
*
*	@email		c.zhenrong\@digipen.edu
* 
*   @author		Foong Pun Yuen Nigel (core system structure outline)
*
*	@email		p.foong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		[M5] 15 March 2024
*
* *****************************************************************************
*
*	@brief		Event handling and automation of tutiorial system
*
*	Contains logic and event handling for the tutorial system. Layer management
*	and storing of tutorials state is handled by the system, and makes use of
*	on click event triggers to advance the tutorial.
*
******************************************************************************/

#include "Tutorial.h"
#include "Events.h"
#include "Layering.h"
#include "message.h"
#include "CharacterStats.h"
#include "Utilities.h"
#include "AssetManager.h"

void UITutorialSystem::Initialize() {
	events.ConnectTutorialSystem(this);
	stepIndex = -1;
	UpdateState();
}

/******************************************************************************
*
*	@brief Updates tutorial state
*
*	-
*
******************************************************************************/
void UITutorialSystem::Update() {
	// Access the ComponentManager through the ECS class
	//ComponentManager& componentManager = ECS::ecs().GetComponentManager();

	BattleSystem* battleSys = events.GetBattleSystem();
	if (battleSys) {
		if (stepIndex && battleSys->skillTooltipCalled) {
			MaintainLayers();
		}

		if (battleSys->activeCharacter && battleSys->activeCharacter->tag == CharacterType::PLAYER && battleSys->activeCharacter->action.entityState == WAITING) {
			if (!overlayOn && stepIndex && !tutorialComplete) {
				UpdateState();
			}
		}

		for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::UICOMPONENT]) {
			switch (msg.type) {
			case(TYPE::MOUSE_MOVE):
				tutMousePos = { msg.posX, msg.posY };
				break;
			}
		}
	}
}

/*!
* \brief Tutorial UpdateState
*
* Updates tutorial state based on current step index
*
*/
void UITutorialSystem::UpdateState() {
	static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
	BattleSystem* battleSys = events.GetBattleSystem();
	std::vector<Entity> entityList{};

	switch (stepIndex) {
	case 0:
		// intro line
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_01.prefab");
		overlayOn = true;
		tutorialComplete = false;
		assetmanager.audio.SetGroupFilter("BGM", LOW_FILTER_VALUE);
		break;
	case 1:
		// player hp ui
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_02.prefab");

		entityList = battleSys->allyHealthBars;
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);
		break;
	case 2:
		// enenmy hp ui
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_03.prefab");

		entityList = battleSys->enemyHealthBars;
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);
		break;
	case 3:
		// view enemy skills - battleinfo
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_04.prefab");
		
		entityList.push_back(battleSys->battleInfoButton);
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);
		break;
	case 4:
		// turn order
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_05.prefab");

		for (Entity e : battleSys->turnOrderQueueAnimator) {
			entityList.push_back(e);
		}
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);
		break;
	case 5:
		// skill buttons
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_06.prefab");

		entityList = battleSys->skillButtons;
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);
		break;
	case 6:
		// chi system
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_07.prefab");

		entityList.push_back(battleSys->chiLabel);
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);
		break;
	case 7:
		// skill 1 (ST)
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_08.prefab");
		battleSys->tutorialLock = 0;

		entityList.push_back(battleSys->chiLabel);
		entityList.push_back(battleSys->skillButtons[0]);
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);
		break;
	case 8:
		// select enemy target (single)
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_09.prefab");

		// force tutorial to wait for battle system next step
		nextStepWait = true;

		entityList = battleSys->targetCircleList;
		for (CharacterStats* c : battleSys->GetEnemies()) {
			entityList.push_back(c->entity);
		}
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);
		break;
	case 9:
		// skill 2 (AOE)
		// SP: at first call return, battle in progress. call again once its player turn again
		if (nextStepWait && overlay) {
			RevertLayers();
			EntityFactory::entityFactory().DeleteCloneModel(overlay);
			overlay = 0;
			overlayOn = false;
			assetmanager.audio.SetGroupFilter("BGM", 1.f);
			return;
		}
		if (!overlay) {
			overlay = EntityFactory::entityFactory().ClonePrefab("tutorial_overlay.prefab");
			overlayOn = true;
			assetmanager.audio.SetGroupFilter("BGM", LOW_FILTER_VALUE);
		}
		nextStepWait = false;
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_10.prefab");

		entityList.push_back(battleSys->chiLabel);
		entityList.push_back(battleSys->skillButtons[1]);
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);
		break;
	case 10:
		// select all enemy targets (AOE)
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_11.prefab");

		// force tutorial to wait for battle system next step
		nextStepWait = true;

		entityList = battleSys->targetCircleList;
		for (CharacterStats* c : battleSys->GetEnemies()) {
			entityList.push_back(c->entity);
		}
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);
		break;
	case 11:
		// end of tutorial/outro 1
		// SP: at first call return, battle in progress. call again once its player turn again
		if (nextStepWait && overlay) {
			RevertLayers();
			EntityFactory::entityFactory().DeleteCloneModel(overlay);
			overlay = 0;
			overlayOn = false;
			assetmanager.audio.SetGroupFilter("BGM", 1.f);
			return;
		}
		if (!overlay) {
			overlay = EntityFactory::entityFactory().ClonePrefab("tutorial_overlay.prefab");
			overlayOn = true;
			assetmanager.audio.SetGroupFilter("BGM", LOW_FILTER_VALUE);
		}
		nextStepWait = false;
		battleSys->tutorialLock = 1;
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_12.prefab");
		RevertLayers();
		break;
	case 12:
		// end of tutorial/outro 2
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_13.prefab");
		RevertLayers();
		break;
	default:		
		if (overlay && overlayOn) {
			RevertLayers();
			EntityFactory::entityFactory().DeleteCloneModel(overlay);
			overlay = 0;
			overlayOn = false;
			tutorialComplete = true;
			battleSys->tutorialLock = 0;
			assetmanager.audio.SetGroupFilter("BGM", 1.f);
		}
		break;
	}
}

/*!
* \brief Tutorial CheckConditionFulfilled
*
* When on click, confirm system mode is current and pre-requisites of current step are fulfilled
* (e.g . mouse click on skill button 1, mouse click on enemy target)
*
*/
void UITutorialSystem::CheckConditionFulfilled(bool& result) {
	auto& modelArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>() };
	BattleSystem* bs = events.GetBattleSystem();
	std::vector<Entity> entityList{};

	if ((GetCurrentSystemMode() == SystemMode::PAUSE || GetCurrentSystemMode() == SystemMode::GAMEHELP || GetCurrentSystemMode() == SystemMode::SETTINGS || GetCurrentSystemMode() == SystemMode::EXITCONFIRM)) {
		result = false;
		return;
	}

	switch (stepIndex) {
	case 3:
		 // return false if battleinfo tooltip is open
		if (ECS::ecs().EntityExists(bs->battleinfo))
		{
			entityList.push_back(bs->battleinfo);
			result = false;
		}

		// SP: ensure battleInfoButton & battleinfo is surfaced
		entityList.push_back(bs->battleInfoButton);		
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);

		break;
	case 7:
	{
		// return false if mouse click is not on skill button 1
		Model& skillModel{ modelArray.GetData(bs->skillButtons[0]) };
		if (!IsWithinObject(skillModel, tutMousePos))
			result = false;
		break;
	}
	case 8:
	{
		// return false if mouse click is not on enemy target
		for (Entity& e : bs->targetCircleList) {
			Model& targetModel{ modelArray.GetData(e) };
			if (IsWithinObject(targetModel, tutMousePos)) {
				return;
			}
		}
		result = false;

		// SP: ensure targetCircle is surfaced
		entityList = bs->targetCircleList;
		for (CharacterStats* c : bs->GetEnemies()) {
			entityList.push_back(c->entity);
		}
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);

		break;
	}
	case 9:
	{
		// return false if mouse click is not on skill button 2
		Model& skillModel{ modelArray.GetData(bs->skillButtons[1]) };
		if (!IsWithinObject(skillModel, tutMousePos))
			result = false;
		break;
	}
	case 10:
	{
		// return false if mouse click is not on enemy target
		bool aoe{ bs->activeCharacter->action.selectedSkill.attacktype == AttackType::AOE };
		if (!aoe) {
			result = false;
			break;
		}

		entityList = bs->targetCircleList;
		for (Entity& e : entityList) {
			Model& targetModel{ modelArray.GetData(e) };
			if (IsWithinObject(targetModel, tutMousePos)) {
				return;
			}
		}
		result = false;

		// SP: ensure targetCircle is surfaced
		for (CharacterStats* c : bs->GetEnemies()) {
			entityList.push_back(c->entity);
		}
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);

		break;
	}
	default:
		return;
	}
}

/*!
* \brief Tutorial SurfaceSystemOverlay
*
* Self explanatory. System overlays take precedence over tutorial under any condition.
*
*/
void UITutorialSystem::SurfaceSystemOverlay(Entity& entity) {
	if (!overlayOn || tutorialComplete) {
		return;
	}

	std::vector<Entity> entityList{};
	for (std::pair<Entity, size_t> pair : originalLayers) {
		entityList.push_back(pair.first);
	}
	entityList.push_back(entity);
	GetChildren(entityList);
	SurfaceTargetLayers(entityList);
	systemOverlayOn = true;
}

/*!
* \brief Tutorial GetChildren
*
* Helper function to get children of entities to surface
*
*/
void UITutorialSystem::GetChildren(std::vector<Entity>& entityList) {
	static auto& parentArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Parent>() };
	std::vector<Entity> tmp{};
	for (Entity e : entityList) {
		if (!parentArray.HasComponent(e)) {
			continue;
		}
		std::vector<Entity> children = parentArray.GetData(e).children;
		for (Entity c : children) {
			tmp.push_back(c);
		}
	}
	for (Entity e : tmp) {
		entityList.push_back(e);
	}
}

/*!
* \brief Tutorial SurfaceTargetLayers
*
* Helper function to get surface target entities to the appropriate layer
*
*/
void UITutorialSystem::SurfaceTargetLayers(const std::vector<Entity> entities) {
	if (entities.empty()) {
		return;
	}

	// reset layers
	RevertLayers();

	// get topmost layer of the scene, and overlay layer number
	size_t topLayer = GetTopLayer();
	size_t overlayLayer = FindInLayer(overlay).first;

	// get highest layer of the entities
	size_t highestLayer = FindInLayer(entities.front()).first;
	size_t lowestLayer = highestLayer;
	for (const Entity& e : entities) {
		size_t layer = FindInLayer(e).first;
		highestLayer = std::max(highestLayer, layer);
		lowestLayer = std::min(lowestLayer, layer);
		originalLayers.push_back(std::make_pair(e, layer));
	}

	// ensure sufficient layers above overlay layer
	int layerDiff = static_cast<int>(topLayer - overlayLayer - (highestLayer - lowestLayer));
	while (layerDiff < 0) {
		CreateNewLayer();
		layerDiff++;
		topLayer++;
	}

	for (std::pair<Entity, size_t>& pair : originalLayers) {
		size_t layer = pair.second;
		size_t newLayer = /*(highestLayer >= 0) ? */topLayer - (highestLayer - layer)/* : layer*/;

		//originalLayers.push_back(std::make_pair(e, layer));
		TransferToLayer(pair.first, newLayer);
		modifiedLayers.push_back(std::make_pair(pair.first, newLayer));
	}
}

/*!
* \brief Tutorial RevertLayers
*
* Helper function to return entities to their original layers before surfacing
* new set of entities.
*
*/
void UITutorialSystem::RevertLayers() {
	RebuildLayeringAfterDeserialization();
	ExtractSkipLockAfterDeserialization();
	for (std::pair<Entity, size_t>& pair : originalLayers) {
		TransferToLayer(pair.first, pair.second);
	}
	originalLayers.clear();
	modifiedLayers.clear();
}

/*!
* \brief Tutorial MaintainLayers
*
* Enforce surfaced layers to remain in their new layer so that it remains unaffected
* by tooltip hovers, prefab cloning or any other layering changes.
*
*/
void UITutorialSystem::MaintainLayers() {
	size_t topLayer = GetTopLayer();
	for (std::pair<Entity, size_t>& pair : modifiedLayers) {
		size_t currentLayer = FindInLayer(pair.first).first;
		if (currentLayer != pair.second) {
			while (pair.second > topLayer) {
				CreateNewLayer();
				topLayer++;
			}
			TransferToLayer(pair.first, pair.second);
		}
	}
}