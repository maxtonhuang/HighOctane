#pragma once
#include <stack>
#include "EntityFactory.h"
#include "ECS.h"
#include "Components.h"

class UndoRedo {
public:
    void RecordCurrent(Entity entity);
    void Undo();

private:
    struct EntityState {
        Entity entity;
        Transform transform;
    };

    std::stack<EntityState> undoStack;
};

extern UndoRedo undoRedo;