#pragma once
#include "CharacterCommon.h"
#include "CharacterAction.h"
#include "ECS.h"
#include "GameStateManager.h"
#include "Animator.h"
#include "model.h"
#include <vector>
#include "Components.h"

//forward declarations
class CharacterStats; 
class BattleSystem;

class CharacterStats
{
public:
    Animator animator;
    bool checkedStatus;
    CharacterType tag;
    Entity entity; //for reference back to ECS
    CharacterAction action;
    BattleSystem* parent;
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
    void Start();
private:
    
    //void Death(Entity& entity);
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