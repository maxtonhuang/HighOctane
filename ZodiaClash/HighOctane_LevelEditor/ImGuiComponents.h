#pragma once
#include "ImGuiLib.h"
#include "ECS.h"

void UpdateComponentViewer();
void ComponentBrowser(Entity);
void SaveAsPrefab(std::string prefabPath, Entity);
