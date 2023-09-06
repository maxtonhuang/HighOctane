#include "ecsfactory.h"

#include <iostream>
#include <vector>
#include <array>
#include <string>


/*


*/

namespace Architecture {


    


    /*
    // Define a simple ECS system for physics
    class PhysicsSystem {
    public:
        void Update(std::vector<Entity>& entities) {
            for (Entity& entity : entities) {
                if (entity.position && entity.input) {
                    // Simulate physics based on user input
                    if (entity.input->left) {
                        entity.position->x -= 0.1f;
                    }
                    if (entity.input->right) {
                        entity.position->x += 0.1f;
                    }
                    if (entity.input->up) {
                        entity.position->y += 0.1f;
                    }
                    if (entity.input->down) {
                        entity.position->y -= 0.1f;
                    }
                }
            }
        }
    };

    // Define a simple ECS system for rendering
    class RenderingSystem {
    public:
        void Render(const std::vector<Entity>& entities) {
            for (const Entity& entity : entities) {
                if (entity.renderable) {
                    std::cout << "Rendering entity " << entity.id << " with sprite: " << entity.renderable->sprite << std::endl;
                }
            }
        }
    };

    // Define a simple ECS system for handling user input
    class InputSystem {
    public:
        void ProcessInput(std::vector<Entity>& entities) {
            // In a real game, you would collect and process input events here
            // For simplicity, we'll assume the input is set directly on entities in this example
        }
    };

    // Define a simple ECS system for AI
    class AISystem {
    public:
        void UpdateAI(std::vector<Entity>& entities) {
            for (Entity& entity : entities) {
                if (entity.ai && entity.ai->active) {
                    // Implement AI logic here (e.g., decision-making, pathfinding, etc.)
                    std::cout << "AI for entity " << entity.id << " is active." << std::endl;
                }
            }
        }
    };

    int main() {
        // Create some entities and components
        Entity entity1 = { 1, new Position(0.0f, 0.0f), new Renderable("sprite1.png"), new Input(), new AI() };
        Entity entity2 = { 2, new Position(1.0f, 1.0f), nullptr, new Input(), nullptr };

        // Add entities to the ECS
        std::vector<Entity> entities; <----------------------------------
        entities.push_back(entity1);
        entities.push_back(entity2);

        // Create ECS systems
        PhysicsSystem physicsSystem;
        RenderingSystem renderingSystem;
        InputSystem inputSystem;
        AISystem aiSystem;

        // Main game loop
        for (int i = 0; i < 5; ++i) {
            // Process user input
            inputSystem.ProcessInput(entities);

            // Update physics
            physicsSystem.Update(entities);

            // Update AI
            aiSystem.UpdateAI(entities);

            // Render entities
            renderingSystem.Render(entities);
        }

        // Clean up allocated memory for components (in a real ECS, you'd manage this differently)
        for (Entity& entity : entities) {
            delete entity.position;
            delete entity.renderable;
            delete entity.input;
            delete entity.ai;
        }

        return 0;
    }
    */



    // Create new entity and add to the entity list.

void ECS::CreateEntity(/*attributes*/) {
    Entity tempEntity = {/*attributes*/ };
    entities.emplace_back(tempEntity);
    ++entityID;
}

void ECS::DeleteAllEntities() {
    for (Entity val : entities) {
        /* Delete Each Attribute */
    }
}

}