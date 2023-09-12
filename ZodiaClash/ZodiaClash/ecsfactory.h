#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <bitset>
#include <queue>
#include "debugdiagnostic.h"


namespace Architecture {


    ////////// ENTITY //////////


    // A simple type alias
    using Entity = std::uint32_t;

    // Used to define the size of arrays later on
    const Entity MAX_ENTITIES = 5000;

    class EntityManager
    {
    public:
        EntityManager()
        {
            // Initialize the queue with all possible entity IDs
            for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
            {
                mAvailableEntities.push(entity);
            }
        }

        Entity CreateEntity()
        {
            Assert(mLivingEntityCount < MAX_ENTITIES, "Too many entities in existence.");

            // Take an ID from the front of the queue
            Entity id = mAvailableEntities.front();
            mAvailableEntities.pop();
            ++mLivingEntityCount;

            return id;
        }

        void DestroyEntity(Entity entity)
        {
            //assert(entity < MAX_ENTITIES && "Entity out of range.");

            // Invalidate the destroyed entity's signature
            mSignatures[entity].reset();

            // Put the destroyed ID at the back of the queue
            mAvailableEntities.push(entity);
            --mLivingEntityCount;
        }

        void SetSignature(Entity entity, Signature signature)
        {
            //assert(entity < MAX_ENTITIES && "Entity out of range.");

            // Put this entity's signature into the array
            mSignatures[entity] = signature;
        }

        Signature GetSignature(Entity entity)
        {
            //assert(entity < MAX_ENTITIES && "Entity out of range.");

            // Get this entity's signature from the array
            return mSignatures[entity];
        }

    private:
        // Queue of unused entity IDs
        std::queue<Entity> mAvailableEntities{};

        // Array of signatures where the index corresponds to the entity ID
        std::array<Signature, MAX_ENTITIES> mSignatures{};

        // Total living entities - used to keep limits on how many exist
        uint32_t mLivingEntityCount{};
    };


    ////////// COMPONENT //////////



    // A simple type alias
    using ComponentType = std::uint8_t;

    // Used to define the size of arrays later on
    const ComponentType MAX_COMPONENTS = 32;



    // A simple type alias
    using Signature = std::bitset<MAX_COMPONENTS>;









    ////////// SYSTEM //////////


















    /*


    class ECS {

    public:

        void CreateEntity();
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




    */

}