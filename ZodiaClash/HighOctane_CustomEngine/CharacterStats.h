#pragma once
#include "ECS.h"
#include "GameStateManager.h"
//#include "Battle.h"
#include "Animator.h"
#include "model.h"
#include <vector>
#include "Components.h"
#include "ECS.h"

struct GameObject {
    Entity entity;
    Tag tag;
    std::string name;

public:
    static std::vector<GameObject> FindGameObjectsWithTag(std::string tag) 
    {

        auto& texArray = ECS::ecs().GetComponentManager().GetComponentArrayRef<Tag>();

        return std::vector<GameObject>();
    }

};

class CharacterStats
{
public:
    bool isnull;
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

private:
    void Start();
    void Death();
    /*GameStateManager gameManager;
   BattleManager battleManager;
   StatusEffectHud statusEffectHud;*/

};