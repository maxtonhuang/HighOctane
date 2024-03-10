#pragma once
#include "ECS.h"
#include "Global.h"
#include "Battle.h"

class UITutorialSystem : public System {
public:
	Entity overlay{ 0 };
	Entity currentTutorialEntity{ 0 };
	int stepIndex{ 0 };
	bool overlayOn{ false };
	bool systemOverlayOn{ false };
	std::vector<std::pair<Entity, size_t>> originalLayers;
	std::vector<std::pair<Entity, size_t>> modifiedLayers;

	void Initialize();
	void Update() override;
	void UpdateState();
	void CheckConditionFulfilled(bool& result);
	void CheckPrefabOverlap();
	void SurfaceSystemOverlay(Entity& entity);
	void MaintainLayers();
private:
	vmath::Vector2 tutMousePos{ RESET_VEC2 };
	bool prefabOffset { false };
	bool nextStepWait { false };
	bool tutorialComplete{ false };

	void GetChildren(std::vector<Entity>& entityList);
	void SurfaceTargetLayers(const std::vector<Entity> entities);
	void RevertLayers();
	
};