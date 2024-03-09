#pragma once
#include "ECS.h"
#include "Battle.h"

class UITutorialSystem : public System {
public:
	void Initialize();
	void Update() override;
	void UpdateState();
	void CheckConditionFulfilled(bool& result);
	Entity overlay{ 0 };
	Entity currentTutorialEntity{ 0 };
	int stepIndex{ 0 };
	
	std::vector<std::pair<Entity, size_t>> originalLayers;
	std::vector<std::pair<Entity, size_t>> modifiedLayers;
private:
	bool overlayOn{ false };
	bool skillTooltipCalled{ false };
	void GetChildren(std::vector<Entity>& entityList);
	void SurfaceTargetLayers(std::vector<Entity> entities);
	void RevertLayers();
	void MaintainLayers();
};