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
    struct stats {
        float           maxHealth;
        float           health;
        float           attack;
        float           defense;
        int             speed;
    }; stats stats{};

    //negative effects 
    struct debuff {
        int             bleedStack{ 3 }; //decrease the health
    }; debuff debuffs{};

    void TakeDamage(float damage);
    void HealBuff(float buffAmount);
    GameObject gameObject{};
    void Start();
    void InitializeCharacterSkills(CharacterStats& character);
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