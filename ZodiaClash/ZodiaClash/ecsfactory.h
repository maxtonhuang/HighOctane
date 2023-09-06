#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <string>


namespace Architecture {

    class ECS {

    public:

        void CreateEntity(/*attributes*/);
        void DeleteEntity(size_t entityID);
        void DeleteAllEntities();

    private:
        
        /////////// COMPONENTS ///////////

        // Define a component structure for user input
        struct Input {
            bool left, right, up, down;

            Input() : left(false), right(false), up(false), down(false) {}
        };

        struct Position {
            double _x, _y;
        };

        struct Collider {
            // bounding box
        };

        struct PhysicalAttributes {
            // velocity
            // movement
            // direction
            // acceleration
        };

        struct AI {
            // enums for AI Behaviour
        };

        struct Animation {

        };

        struct Audio {
            // Audio File
        };

        struct Timers {
            // timers for animation and cooldowns
        };

        struct StatusEffects {
            // buffs/debuffs
        };

        struct Health {

        };

        struct UI {

        };

        struct Transform {
            // for transforming meshes
        };

        ////////// ENTITIES ///////////

        size_t entityID{ 0 };

        // Define an entity structure
        struct Entity {
            Input* input;
            Position* position;
            Collider* collider;
            PhysicalAttributes* physicalAttributes;
            AI* ai;
            Animation* animation;
            Audio* audio;
            Timers* timers;
            StatusEffects* statusEffects;
            Health* health;
            UI* ui;
            Transform* transform;            
        };

        std::vector<Entity> entities;

    };
}