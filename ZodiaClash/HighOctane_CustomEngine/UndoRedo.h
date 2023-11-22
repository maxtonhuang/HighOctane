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