/******************************************************************************
*
*	\copyright
*		All content(C) 2023/2024 DigiPen Institute of Technology Singapore.
*		All rights reserved. Reproduction or disclosure of this file or its
*		contents without the prior written consent of DigiPen Institute of
*		Technology is prohibited.
*
* *****************************************************************************
*
*	@file		CharacterStats.h
*
*	@author		Liu WanTing
*
*	@email		wanting.liu\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		22 September 2023
*
* *****************************************************************************
*
*	@brief		Battle Character Statistics
*
*	This file contains implementations of functions related to a character's
*	statistics and actions during a battle scene. These functions are part of the game
*	logic and are called by the Game State Manager (GSM) during battle events.
*
******************************************************************************/

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
class BattleSystem;

class CharacterStats
{
public:
    CharacterStats();
    CharacterStats(CharacterStats const&);
    bool checkedStatus;
    CharacterType tag{};
    Entity entity{}; //for reference back to ECS
    CharacterAction action{};
    BattleSystem* parent{};
    std::string icon{}; //Texture for UI icon
    struct stats {
        float           maxHealth;
        float           health;
        float           attack;
        float           defense;
        int             speed;
    }; stats stats{};

    //positive effects
    struct buff {
        int   attackStack{ 0 };
        float attackBuff{ 0.f };
        int defenseStack{ 0 };
        float defenseBuff{ 0.f };
        int   reflectStack{ 0 };
        int   shieldStack{ 0 }; //Protected by another entity
        Entity shieldEntity{ 0 };
    }; buff buffs{};

    //negative effects 
    struct debuff {
       int    bloodStack{ 0 }; //decrease the health
       int    tauntStack{ 0 }; //must target debuff
       Entity tauntTarget{ 0 }; //who to target for taunt
       int    stunStack{ 0 }; //skip turn and reduce
       int    huntedStack{ 0 }; //will be targeted by enemies
       int    igniteStack{ 0 }; //takes damage when using chi
       int   attackStack{ 0 };
       float attackDebuff{ 0.f };
       int defenseStack{ 0 };
       float defenseDebuff{ 0.f };
       bool operator!= (const debuff& rhs);
    }; debuff debuffs{};

    bool boss{};
    bool untargetable{};
    int cycle{ 0 }; // for boss cycles
    bool charge{};
    bool crit{}; //Check if enemy was hit by a critical
    float damage{}; //damage taken

    void TakeDamage(float damage);
    void HealBuff(float buffAmount);
    void SpeedBuff(CharacterStats* ally);
    void ApplyBloodStack();
    GameObject gameObject{};
    void Start();
    bool operator==(const CharacterStats&) const;
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


//part of debuff struct
 //int             shatterCounter; //lower the defense
//int             stunCounter; //freeze there 
//int             tauntCounter; //forced to attack the person that taunt me
        
//struct buff {
//    bool            speedCheck;
//    int             enrageCounter;
//    int             armorCounter;
//}