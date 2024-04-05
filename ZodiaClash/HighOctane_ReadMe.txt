ZODIACLASH README

================================================================

Game name: ZodiaClash
Version: v0.6 M6 build

Programmers:
Huang Maxton Xinghua (m.huang@digipen.edu)
Liu Wanting (wanting.liu@digipen.edu)
Foong Pun Yuen Nigel (p.foong@digipen.edu)
Chua Zhen Rong (c.zhenrong@digipen.edu)
Van Adrichem Boogaert Kai Alexander (kaialexander.v@digipen.edu)

Designer:
Yau Yee Onn Oliver (y.yau@digipen.edu)

Artist:
Loo Zhi Ying (zhiying.loo@digipen.edu)

================================================================

About:

Currently, the game for M6 consists of the main exploration map, as well as 3 battle stages. There will be a final level for M6. The characters' stats are also not fully balanced yet, but it is playable.

Scenes: Start Menu, Cutscene, Exploration Map, 3 Battle Scenes

================================================================
GAME FEATURES:

1) Player Chi System - Shared skill point resource system

- All characters on the player’s team will share Chi. (If one of the characters from the player’s team uses a special skill that consumes a huge amount of Chi, the next character might not be able to use a skill that consumes Chi)

- All characters on the player’s team will have multiple skills that consume different amounts of Chi.

- Every character can use their first skill to regain 1 Chi.

- A variety of skills will be available for different situations, making skill points management crucial to winning battles.


2) Unique Player Skills
Currently, the game consists of 2 playable characters, Mei(Cat) and Yang(Goat).

[Mei - Cat]
Skill 1 (Scarlet Claw) - Deals damage and inflicts 1 bleed stack to an enemy for 3 turns.
(+1 Chi)

Skill 2 (Dance of Fury) - Deals damage and inflicts 1 bleed stack to all enemies for 2 turns.
(-1 Chi)

Skill 3 (Secret Arts: Pounce) - Deals damage to all enemies. If enemies have existing bleed stacks, all bleed will be triggered and removed.
(-3 Chi)

[Yang - Goat]
Skill 1 (Yin-Yang Strike) - Deals damage and has a 40% chance to remove all buffs from an enemy.
(+1 Chi)

Skill 2 (Chi Surge) - Enrages and makes an ally (except Yang) take action immediately for 1 turn.
(-2 Chi)

Skill 3 (Shepherd's Grace) - Removes all debuffs and heals an ally based on Yang’s maximum health.
(-2 Chi)

3) Status Effects
Below are the current status effects in the game.

[Debuffs]
Bleed - Takes damage at the start of the next turn. Maximum limit of 5 stacks.

Break - Has reduced defense and takes more damage on the next turn.

Stun - Unable to perform any actions on the next turn.

Taunt - Forced to target a specific character on the next turn.

[Buffs]
Enrage - Has increased attack and deals more damage on the next turn.

Strengthen - Has increased defense and takes less damage on the next turn.

Berserk - Has significantly increased attack and drains life from attacks on the next turn.

Counter (Unremovable) - Deals damage back to the attacker immediately

================================================================

GAME CONTROLS:

1) Start Menu
Navigate through the start menu with LMB.

2) Exploration Map
Navigate through the exploration map with LMB.

3) Battle Screen
Main Gameplay Controls
- Only mouse selection is needed.
- When it is your character's turn, select a skill.
- After selecting a skill, a popout will show which targets you can select.
- After selecting the target, the skill will take effect.
- Repeat steps 1-3, until the battle has ended.

4) Cheat Codes
- Press F1 to go to Main Menu.
- Press F2 to go to the Beginning cutscene.
- Press F3 to go to the first part of the exploration map.
- Press F4 to go to the first battle stage/tutorial.
- Press F5 to go to the second part of the exploration map.
- Press F6 to go to the second battle stage.
- Press F7 to go to the third battle stage.
- Press F9 to toggle GodMode, the player restores full health and do at least twice the normal damage.
- Press F10 to toggle EndGameMode, enemy's health will be decreased to zero after the next attack.
- Press 9 for FPS Counter
- Press 0 for viewable physics collider

5) Other Controls
- Press F11 for fullscreen.
- Press Esc for pause menu.

================================================================

OTHERS:

1) HUD/UI
Below are a list of HUD that the player will see.

[Main HUD/UI]
Player Characters' HP - Displays the player characters and their HP (Top-right)
Enemy Characters' HP - Displays the enemy characters and their HP (Top-left)
Turn Order UI - Displays the turn order of all characters in the battle stage (Bottom-left)
Characters' Skills - Displays the characters' skills based on the current character that the player is controlling (Bottom-right)
Chi Points - Displays the amount of Chi that the player currently has represented by the fire icons (Bottom-right)

[Other HUD/UI]
Pause Menu (Press Esc to toggle the Pause Menu)

2) Enemy Bosses
Currently, there are the Goat and Ox bosses.

In the final version of the game, both the Goat and Ox will also be playable characters as well. There will also be a final boss, which will have several elements of the Zodiac animals combined together.


3) Top-down Exploration Map
The player will first start on the map and navigate to the end goal. The player will then encounter characters on the map and interact with them. Dialogues will be shown based on the character that they interacted with. After interacting with them, the characters will then be transported to the turn-based battle scene.


4) Audio
Audio is implemented in all parts of the game. More dialogues will be fleshed out in M6, along with more voiceover audio, as well as more sound effects. 
================================================================

AUDIO:

Audio is implemented in all parts of the game.

================================================================

TUTORIAL:

Tutorial is implemented in the first battle (battle.scn). Rules are outlined during the tutorial phase.

================================================================
