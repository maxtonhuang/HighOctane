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
    DELENTITY
};

class UndoRedo {
public:
    void RecordCurrent(Entity entity, ACTION action);
    void Undo();
    Transform CheckFrontTransform();
    void StackPopFront();
private:
    struct EntityChanges {
        Entity entity{};
        Transform transform{};
        Clone clone{};
        ACTION action{};
    };

    std::deque<EntityChanges> undoStack;
};

extern UndoRedo undoRedo;