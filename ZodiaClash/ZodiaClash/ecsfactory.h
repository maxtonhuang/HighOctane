#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <string>


namespace Architecture {

    class ECS {

    public:

        void CreateEntity(/*attributes*/);

        void DeleteAllEntities();

    private:
        
        /////////// COMPONENTS ///////////

        // Define a component structure for position
        struct Position {
            float x, y;

            Position(float _x, float _y) : x(_x), y(_y) {}
        };

        // Define a component structure for rendering
        struct Renderable {
            std::string sprite;

            Renderable(const std::string& _sprite) : sprite(_sprite) {}
        };

        // Define a component structure for user input
        struct Input {
            bool left, right, up, down;

            Input() : left(false), right(false), up(false), down(false) {}
        };

        // Define a component structure for AI
        struct AI {
            bool active;

            AI() : active(true) {}
        };
        





        ////////// ENTITIES ///////////

        // Define an entity structure
        struct Entity {
            static int id;
            Position* position;
            Renderable* renderable;
            Input* input;
            AI* ai;
        };

        // array to contain IDs that correspond to the respective systems
        std::vector<unsigned int> physicsEntities;
        std::vector<unsigned int> aEntities;
        std::vector<unsigned int> bEntities;
        std::vector<unsigned int> cEntities;
        std::vector<unsigned int> dEntities;
        std::vector<unsigned int> eEntities;
        std::vector<unsigned int> fEntities;

        std::vector<Entity> entities;

    };
}