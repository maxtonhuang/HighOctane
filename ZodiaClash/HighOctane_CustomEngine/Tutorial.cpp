#include "Tutorial.h"
#include "Events.h"
#include "Layering.h"
#include "message.h"
#include "CharacterStats.h"
#include "Global.h"
#include "Utilities.h"

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

		for (Entity const& entity : m_Entities) {
			bool animating = false;
			for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::BATTLE]) {
				switch (msg.type) {
				case(TYPE::ANIMATING):
					animating = true;

					break;
				}
			}

			if (!animating) {
				//UpdateState();
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
		// view enemy skills
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_04.prefab");
		// TODO: add button for tooltip
		RevertLayers();
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
		battleSys->tutorialCalled = 0;
		entityList.push_back(battleSys->skillButtons[0]);
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);
		break;
	case 8:
		// select enemy target (single)
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_09.prefab");

		entityList = battleSys->targetCircleList;
		for (CharacterStats* c : battleSys->GetEnemies()) {
			entityList.push_back(c->entity);
		}
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);
		break;
	case 9:
		// skill 2 (AOE)
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_10.prefab");

		entityList.push_back(battleSys->skillButtons[1]);
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);
		break;
	case 10:
		// select all enemy targets (AOE)
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_11.prefab");

		entityList = battleSys->targetCircleList;
		for (CharacterStats* c : battleSys->GetEnemies()) {
			entityList.push_back(c->entity);
		}
		GetChildren(entityList);
		SurfaceTargetLayers(entityList);
		break;
	case 11:
		// end of tutorial/outro
		currentTutorialEntity = EntityFactory::entityFactory().ClonePrefab("tutorial_12.prefab");
		RevertLayers();
		break;
	default:
		if (overlay && overlayOn) {
			EntityFactory::entityFactory().DeleteCloneModel(overlay);
			overlay = 0;
			overlayOn = false;
		}
		break;
	}
}

void UITutorialSystem::CheckConditionFulfilled(bool& result) {
	static vmath::Vector2 tutMousePos{ RESET_VEC2 };
	auto& modelArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Model>() };
	BattleSystem* bs = events.GetBattleSystem();

	if (!((stepIndex >= 7) && (stepIndex <= 10)))
		return;

	for (Postcard const& msg : Mail::mail().mailbox[ADDRESS::UICOMPONENT]) {
		switch (msg.type) {
		case(TYPE::MOUSE_MOVE):
			tutMousePos = { msg.posX, msg.posY };
			break;
		case(TYPE::MOUSE_CLICK):
			switch (stepIndex) {
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

				for (Entity& e : bs->targetCircleList) {
					Model& targetModel{ modelArray.GetData(e) };
					if (IsWithinObject(targetModel, tutMousePos)) {
						return;
					}
				}
				result = false;
				break;
			}
			default:
				break;
			}
			break;
		}
	}
}

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

void UITutorialSystem::SurfaceTargetLayers(std::vector<Entity> entities) {
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
	}

	// ensure sufficient layers above overlay layer
	int layerDiff = static_cast<int>(topLayer - overlayLayer - (highestLayer - lowestLayer));
	while (layerDiff < 0) {
		CreateNewLayer();
		layerDiff++;
		topLayer++;
	}

	for (const Entity& e : entities) {
		size_t layer = FindInLayer(e).first;
		size_t newLayer = (highestLayer >= 0) ? topLayer - (highestLayer - layer) : layer;

		originalLayers.push_back(std::make_pair(e, layer));
		TransferToLayer(e, newLayer);
		modifiedLayers.push_back(std::make_pair(e, newLayer));
	}
}

void UITutorialSystem::RevertLayers() {
	for (std::pair<Entity, size_t>& pair : originalLayers) {
		TransferToLayer(pair.first, pair.second);
	}
	originalLayers.clear();
	modifiedLayers.clear();
}

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