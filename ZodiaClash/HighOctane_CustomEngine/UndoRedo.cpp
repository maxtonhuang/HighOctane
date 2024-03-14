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
*	@file		UndoRedo.cpp
*
*	@author		Kai Alexander Van Adrichem Boogaert
*
*	@email		kaialexander.v\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		10 October 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file is the definitions for the Undo Feature in our editor.
*   !-----M3-----!
*   - Allowed Undo of transform(position,scale,rotation) Currently works for when editing transform within the scene viewport
*   - Allowed Undo for Add and removing objects
*   - UndoStack utilises a deque for ease of access between the front and the back elements
*   - Utilises an action enum to properly handle specific types of actions, which can be further expanded upon in the future
*
******************************************************************************/
#include "UndoRedo.h"
#include "Global.h"
#include "Layering.h"
#include "AssetManager.h"

#define UNDOREDO_STACK_SIZE 20 //Stack size
UndoRedo undoRedo;

void UndoRedo::RecordCurrent(Entity entity, ACTION action) {
    EntityChanges currentState;
    currentState.entity = entity;
    currentState.action = action;
    undoFlag = false;
    if (!undoStack.empty()) {
        if (ECS::ecs().GetComponent<Transform>(currentState.entity) == undoRedo.CheckFrontTransform()) {
            undoRedo.StackPopFront(); //Prevents case when you double click the same entity
        }
    }
    switch (action) {
    case ACTION::TRANSFORM:
        currentState.transform = ECS::ecs().GetComponent<Transform>(entity);
        break;
    case ACTION::SIZE:
        currentState.size = ECS::ecs().GetComponent<Size>(entity);
        break;
    case ACTION::TEXTURE:
        currentState.tex.tex = ECS::ecs().GetComponent<Tex>(entity).tex;
        currentState.size = ECS::ecs().GetComponent<Size>(entity);
        break;
    case ACTION::DELENTITY:
        currentState.layerIndex = FindInLayer(entity).first;
        break;
    }
    // To properly delete an entity if it exceeds the stack of entities
    if (undoStack.size() >= UNDOREDO_STACK_SIZE) {
        EntityChanges checkLast = undoStack.back();
        undoStack.pop_back();
        if (checkLast.action == ACTION::DELENTITY) {
            EntityFactory::entityFactory().DeleteCloneModel(checkLast.entity);
        }
    }
    undoStack.push_front(currentState);
}

void UndoRedo::RecordComponent(Entity entity, ACTION action, std::string component) {
    EntityChanges currentState;
    currentState.entity = entity;
    currentState.action = action;
    currentState.component = component;

    undoStack.push_front(currentState);
}

void UndoRedo::Undo() {
    if (!undoStack.empty()) {
        undoFlag = true;
        EntityChanges currentState = undoStack.front();
        undoStack.pop_front();
        //Move the undone action to the redoStack
        redoStack.push_front(currentState);
        switch(currentState.action) {
        case ACTION::TRANSFORM:
            ECS::ecs().GetComponent<Transform>(currentState.entity) = currentState.transform;
            break;
        case ACTION::ADDENTITY:
            ECS::ecs().RemoveComponent<Clone>(currentState.entity);
            entitiesToSkip[currentState.entity] = false;
            entitiesToLock[currentState.entity] = false;
            break;
        case ACTION::DELENTITY:
            ECS::ecs().AddComponent(currentState.entity, Clone{});
            entitiesToSkip[currentState.entity] = true;
            entitiesToLock[currentState.entity] = true;
            break;
        case ACTION::SIZE:
            ECS::ecs().GetComponent<Size>(currentState.entity) = currentState.size;
            break;
        case ACTION::TEXTURE:
            ECS::ecs().GetComponent<Tex>(currentState.entity).tex = assetmanager.texture.Get(currentState.tex.tex->GetName().c_str());
            ECS::ecs().GetComponent<Size>(currentState.entity) = currentState.size;
            break;
        case ACTION::ADDCOMP:
            for (auto& compType : typeMap) {
                if (currentState.component == compType.first) {
                    compType.second->RemoveComponent(currentState.entity);
                }
            }
            break;
        case ACTION::DELCOMP:
            for (auto& compType : typeMap) {
                if (currentState.component == compType.first) {
                    compType.second->AddComponent(currentState.entity);
                }
            }
            break;
        }

    }
}

void UndoRedo::Redo() {
    if (!undoFlag) {
        redoStack.clear();
    }
    if (!redoStack.empty()) {
        EntityChanges currentState = redoStack.front();
        redoStack.pop_front();

        undoStack.push_front(currentState);

        switch (currentState.action) {
        case ACTION::TRANSFORM:
            ECS::ecs().GetComponent<Transform>(currentState.entity) = currentState.transform;
            break;
        case ACTION::ADDENTITY:
            ECS::ecs().AddComponent(currentState.entity, Clone{});
            entitiesToSkip[currentState.entity] = true;
            entitiesToLock[currentState.entity] = true;
            break;
        case ACTION::DELENTITY:
            ECS::ecs().RemoveComponent<Clone>(currentState.entity);
            entitiesToSkip[currentState.entity] = false;
            entitiesToLock[currentState.entity] = false;
            break;
        case ACTION::SIZE:
            ECS::ecs().GetComponent<Size>(currentState.entity) = currentState.size;
            break;
        case ACTION::TEXTURE:
            ECS::ecs().GetComponent<Tex>(currentState.entity).tex = assetmanager.texture.Get(currentState.tex.tex->GetName().c_str());
            ECS::ecs().GetComponent<Size>(currentState.entity) = currentState.size;
            break;
        case ACTION::ADDCOMP:
            for (auto& compType : typeMap) {
                if (currentState.component == compType.first) {
                    compType.second->AddComponent(currentState.entity);
                }
            }
            break;
        case ACTION::DELCOMP:
            for (auto& compType : typeMap) {
                if (currentState.component == compType.first) {
                    compType.second->RemoveComponent(currentState.entity);
                }
            }
            break;
        }
    }
}

Transform UndoRedo::CheckFrontTransform() {
    return undoStack.front().transform;
}

void UndoRedo::StackPopFront() {
    undoStack.pop_front();
}

bool UndoRedo::notInUndoStack(size_t checkEntity){
    for (auto& entityChanges : undoStack) {
        if (static_cast<Entity>(checkEntity) == entityChanges.entity && (entityChanges.action == ACTION::DELENTITY) ) {
            return false;
        }
    }
    return true;
}

bool UndoRedo::notInRedoStack(size_t checkEntity){
    
    for (auto& entityChanges : redoStack) {
        if (static_cast<Entity>(checkEntity) == entityChanges.entity && ( (entityChanges.action == ACTION::ADDENTITY)) ) {
            return false;
        }
    }
    return true;
}

size_t UndoRedo::UndoSize() {
   return undoStack.size();
}
size_t UndoRedo::RedoSize() {
   return redoStack.size();
}


bool UndoRedo::Find(Entity entity) {
    for (const auto& entityChanges : undoStack) {
        if (entity == entityChanges.entity) {
            return true;
        }
    }

    for (const auto& entityChanges : redoStack) {
        if (entity == entityChanges.entity) {
            return true;
        }
    }

    return false;
}

void UndoRedo::Layer(size_t layer) {
    for (const auto& entityChanges : undoStack) {
        if (layer == entityChanges.layerIndex) {
            RemoveEntityFromLayering(entityChanges.entity);
            EntityFactory::entityFactory().DeleteCloneModel(entityChanges.entity);
        }
    }

    for (const auto& entityChanges : redoStack) {
        if (layer == entityChanges.layerIndex) {
            RemoveEntityFromLayering(entityChanges.entity);
            EntityFactory::entityFactory().DeleteCloneModel(entityChanges.entity);
        }
    }
}
