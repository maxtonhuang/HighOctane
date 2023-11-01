#pragma once
#include <vector>
#include <string>
#include "Components.h"

class CharacterStats;

struct GameObject {
    Tag tag;
    std::string name;
    bool isnull;
    bool isActive;
public:
    static std::vector<GameObject*> FindGameObjectsWithTag(std::string tag)
    {
        //std::vector<GameObject*> output{};
        //auto tagArray{ ECS::ecs().GetComponentManager().GetComponentArrayRef<Tag>().GetArray() };
        //for (auto t : tagArray) {
            //if (t->tag == tag) {
                //output.push_back(t);
            //}
        //}
        return std::vector<GameObject*>{};
    }
    GameObject() : isActive(true), isnull() {}  // By default, the game object is active
    // set the active state of the game object
    void SetActive(bool active)
    {
        isActive = active;
    }

    // check if the game object is active
    bool IsActive() const
    {
        return isActive;
    }
};

enum EntityState
{
    START,
    WAITING,
    CHECKSTATUS,
    SELECTION,
    ATTACKING,
    ENDING,
    DYING,
    DEAD
};

enum class CharacterType {
    PLAYER,
    ENEMY
};

//struct SkillSelection
//{
//    GameObject skill1Prefab;
//    GameObject skill2Prefab;
//    GameObject skill3Prefab;
//    GameObject selectedSkillPrefab;
//};

struct TargetSelection
{
    std::vector<CharacterStats*> enemyTargets;
    std::vector<CharacterStats*> playerTargets;
    CharacterStats* selectedTarget;
};
