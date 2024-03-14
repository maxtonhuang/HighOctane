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
*	@file		UndoRedo.h
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
#pragma once
#include <deque>
#include "EntityFactory.h"
#include "ECS.h"
#include "Components.h"
enum class ACTION
{
    NONE,
    TRANSFORM,
    ADDENTITY,
    DELENTITY,
    SIZE,
    TEXTURE,
    ADDCOMP,
    DELCOMP
};

class UndoRedo {
public:
    void RecordCurrent(Entity entity, ACTION action);
    void RecordComponent(Entity entity, ACTION action, std::string component);
    void Undo();
    void Redo();
    size_t UndoSize();
    size_t RedoSize();
    void Layer(size_t layer);
    Transform CheckFrontTransform();
    bool notInUndoStack(size_t checkEntity);
    bool notInRedoStack(size_t checkEntity);
    void StackPopFront();
    bool Find(Entity entity);
    bool undoFlag = false;
private:
    struct EntityChanges {
        Entity entity{};
        ACTION action{};
        Transform transform{};
        Clone clone{};
        Size size{};
        Tex tex{};
        size_t layerIndex{};
        std::string component;
    };

    std::deque<EntityChanges> undoStack;
    std::deque<EntityChanges> redoStack;
    std::deque<Entity>entityBuffer;
};

extern UndoRedo undoRedo;