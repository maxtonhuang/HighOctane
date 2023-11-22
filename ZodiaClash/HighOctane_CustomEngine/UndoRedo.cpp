#include "UndoRedo.h"
#define UNDOREDO_STACK_SIZE 20
UndoRedo undoRedo;

void UndoRedo::RecordCurrent(Entity entity) {
    EntityState currentState;
    currentState.entity = entity;
    currentState.transform = ECS::ecs().GetComponent<Transform>(entity);
    if (undoStack.size() >= UNDOREDO_STACK_SIZE) {
        undoStack.pop();
    }
    undoStack.push(currentState);
}

void UndoRedo::Undo() {
    if (!undoStack.empty()) {
        EntityState currentState = undoStack.top();
        undoStack.pop();

        EntityState newState;
        newState.entity = currentState.entity;
        newState.transform = ECS::ecs().GetComponent<Transform>(currentState.entity);

        // Apply the previous state
        ECS::ecs().GetComponent<Transform>(currentState.entity) = currentState.transform;
    }
}
