#pragma once
#include "ECS.h"
#include "GameStateManager.h"
#include "Animator.h"
#include "model.h"
#include <vector>
#include "Components.h"
#include "ECS.h"

struct GameObject {
    Tag tag;
    std::string name;
    bool isnull;
    bool isActive;
public:
    static std::vector<GameObject> FindGameObjectsWithTag(std::string tag) 
    {

        auto& texArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Tag>();
        return std::vector<GameObject>();
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
    DYING
};

struct SkillSelection
{
    GameObject skill1Prefab;
    GameObject skill2Prefab;
    GameObject skill3Prefab;
    GameObject selectedSkillPrefab;
};

struct TargetSelection
{
    std::vector<GameObject> enemyTargets;
    std::vector<GameObject> playerTargets;
    std::vector<GameObject> selectedTarget;
};

class CharacterStats
{
public:
    Animator animator;
    bool checkedStatus;
    Tag tag;

    struct stats {
        float           maxHealth;
        float           health;
        float           attack;
        float           defense;
        int             speed;
    }; stats stats;

    void TakeDamage(float damage);
    void HealBuff(float buffAmount);
    GameObject gameObject;

private:
    void Start();
    void Death();
    /*GameStateManager gameManager;
   BattleManager battleManager;
   StatusEffectHud statusEffectHud;*/
};



//struct HUD {
//    Transform uniqueTurnHud;
//    Tex uniqueCharacterAvatar;
//    Entity characterHpHud;
//    Transform statusEffectPanel;
//    /*[SerializeField] private Image healthBarFill;
//    [SerializeField] private GameObject characterName;
//    [SerializeField] private GameObject floatingText;
//    [SerializeField] private GameObject statusEffectText;
//    [HideInInspector] public bool hoverHudCheck;*/
//}; HUD hud;

//struct otherPlayerHUD {
//    Model playerAvatar;
//    /*[SerializeField] private TextMeshProUGUI hpValueUi;
//    [HideInInspector] public Color32 playerAvatarOriginalColor;
//    [HideInInspector] public Color32 playerAvatarTargetColor;*/
//};

//struct otherEnemyHUD {
//    Model healthPanel;
//    /*[HideInInspector] public Color32 healthPanelOriginalColor;
//    [HideInInspector] public Color32 healthPanelTargetColor;
//    [HideInInspector] public bool checkedStatus;*/
//};

/*struct debuff {
    int             bleedStack;
    int             shatterCounter;
    int             stunCounter;
    int             tauntCounter;
};

struct buff {
    bool            speedCheck;
    int             enrageCounter;
    int             armorCounter;
};*/