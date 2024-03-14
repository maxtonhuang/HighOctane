ZODIACLASH README

================================================================

Game name: ZodiaClash
Version: v0.5 M5 build

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

Currently, the game for M5 consists of the 3 exploration maps, as well as 3 battle stages. The game stats will continue to be balanced, but it is playable.

Scenes: Start Menu, 3 Exploration Maps, 3 Battle Scenes

================================================================
GAME FEATURES:

1) Player Chi System - Shared skill point resource system

- All characters on the player’s team will share Chi. (If one of the characters from the player’s team uses a special skill that consumes a huge amount of Chi, the next character might not be able to use a skill that consumes Chi)

- All characters on the player’s team will have multiple skills that consume different amounts of Chi.

- Every character can use their first skill to regain 1 Chi.

- A variety of skills will be available for different situations, making skill points management crucial to winning battles.


2) Unique Player Skills
Currently, the game consists of 1 playable character, the Cat (Mei). The Cat is a damage dealer.

[Mei - Cat]
Skill 1 (Scarlet Claw) - Deals damage to a single enemy and inflicts Bleed equal to 10% of Mei's attack for 3 turns.
(+1 Chi)

Skill 2 (Dance of Fury) - Deals damage to all enemies and inflicts Bleed equal to 10% of Mei's attack for 2 turns.
(-2 Chi)

Skill 3 (Secret Arts: Pounce) - Deals damage to all enemies, then if an enemy already has Bleed, all Bleed on that enemy will be activated and removed.
(-3 Chi)


3) Status Effects
Below are the current status effects in the game.

[Debuffs]
Bleed - Deals damage equal to a percentage of the attacker's attack every time the affected character’s turn begins for several turns. Maximum limit of 5 stacks.

Shatter - Lowers the defense of the affected character equal to a percentage of the affected character’s defense for several turns.

[Buffs]

Cleanse - Removes all negative effects of the affected character.

================================================================

GAME CONTROLS:

1) Start Menu
Navigate through the start menu with mouse selection.

2) Battle Screen
Main Gameplay Controls
- Only mouse selection is needed.
- When it is your character's turn, select a skill.
- After selecting a skill, a popout will show which targets you can select.
- After selecting the target, the attack animation will be played out.
- Repeat steps 1-3, until the battle has ended.

3) Cheat Codes
- Press 'F9' to toggle GodMode, player restores full health and do at least twice the normal damage.
- Press 'F10' to toggle EndGameMode, enemy's health decreased to zero and end the game with the player's victory.

4) Other Controls
- Press F11 for fullscreen.
- Press esc to pause the game and show the game menu
================================================================

HOTKEYS (for your convenience):

Switches to corresponding scene
F1 - mainmenu.scn
F2 - beginning_cutscene1.scn
F3 - exploration1.scn
F4 - battle.scn
F5 - exploration2.scn
F6 - battle2.scn

Toggles
9   - FPS Counter
0   - Viewable physics collider
F9  - God mode (increases damage output)
F10 - End game mode (defeats all enemies in the next attack)
F11 - Fullscreen mode

OTHERS:

1) HUD/UI
Below are a list of HUD that the player will see.

[Main HUD/UI]
Player Characters' HP - Displays the player characters and their HP (Top-right)
Enemy Characters' HP - Displays the enemy characters and their HP (Top-left)
Turn Order UI - Displays the turn order of all characters in the battle stage (Bottom-left)
Characters' Skills - Displays the characters' skills based on the current character that the player is controlling (Bottom-right)
Chi Points - Displays the amount of Chi that the player currently has represented by the fire icons (Bottom-right)

2) Enemy Bosses
Currently, there is a Goat boss within the demo.

In the final version of the game, the Goat and the Ox characters will also be an enemy at a certain point of the game, where the Goat and the Ox will also be playable characters for now. There will also be a final boss, which will have elements of the Zodiac animals combined together.


3) Top-down Exploration Map
The player will first start on the map and navigate to the end goal. The player will then encounter different characters on the map and interact with them. Dialogues with unique conversations will be shown based on the character that they interacted with. After interacting with combat NPCs, the characters will then be transported to the turn-based battle scene.
================================================================

AUDIO:

Audio is implemented in all parts of the game.

================================================================

TUTORIAL:

Tutorial is implemented in the first battle (battle.scn). Rules are outlined during the tutorial phase.

================================================================
