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
    if (undoStack.size() >= UNDOREDO_STACK_SIZE) {
        undoStack.pop_back();
        /*if (currentState.action == ACTION::DELENTITY) {
            ECS::ecs().DestroyEntity(currentState.entity);
        }*/
    }
    undoStack.push_front(currentState);
}

void UndoRedo::Undo() {
    if (!undoStack.empty()) {
        printf("UNDO STACK SIZE: %d\n", undoStack.size());
        EntityChanges currentState = undoStack.front();
        undoStack.pop_front();
        printf("UNDO ACTION: %d\n", static_cast<int>(currentState.action));
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
        default:
            printf("Unknown action type: %d\n", static_cast<int>(currentState.action));
        }

    }
}
