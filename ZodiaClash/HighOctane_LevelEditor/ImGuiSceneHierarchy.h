#pragma once
#include "ImGuiLib.h"
#include "ECS.h"

extern Entity currentSelectedEntity;

void UpdateSceneHierachy();
void UpdatePrefabHierachy();
void SceneEntityNode(Entity entity);
void SceneEntityComponents(Entity entity);
