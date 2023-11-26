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
*   - Allowed Undo of transform(position,scale,rotation)
*   - Allowed Undo for Add and removing objects
*
******************************************************************************/
#include "UndoRedo.h"
#include "Global.h"
#include "Layering.h"

#define UNDOREDO_STACK_SIZE 20
UndoRedo undoRedo;

void UndoRedo::RecordCurrent(Entity entity, ACTION action) {
    EntityChanges currentState;
    currentState.entity = entity;
    currentState.transform = ECS::ecs().GetComponent<Transform>(entity);
    currentState.action = action;
    if (!undoStack.empty()) {
        if (ECS::ecs().GetComponent<Transform>(currentState.entity).position == undoRedo.CheckFrontTransform().position) {
            undoRedo.StackPopFront();
        }
    }
    if (undoStack.size() >= UNDOREDO_STACK_SIZE) {
        EntityChanges checkLast = undoStack.back();
        undoStack.pop_back();
        if (checkLast.action == ACTION::DELENTITY) {
            EntityFactory::entityFactory().DeleteCloneModel(checkLast.entity);
        }
    }
    undoStack.push_front(currentState);
}

void UndoRedo::Undo() {
    if (!undoStack.empty()) {
        EntityChanges currentState = undoStack.front();
        undoStack.pop_front();
        switch(currentState.action) {
        case ACTION::TRANSFORM:
            ECS::ecs().GetComponent<Transform>(currentState.entity) = currentState.transform;
            break;
        case ACTION::ADDENTITY:
            RemoveEntityFromLayering(currentState.entity);
            ECS::ecs().RemoveComponent<Clone>(currentState.entity);
            break;
        case ACTION::DELENTITY:
            ECS::ecs().AddComponent(currentState.entity, Clone{});
            entitiesToSkip[currentState.entity] = true;
            entitiesToLock[currentState.entity] = true;
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
