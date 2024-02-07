ZODIACLASH README

================================================================

Game name: ZodiaClash
Version: v0.4 M4 build

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

Currently, the game for M4 consists of the main exploration map, as well as 2 battle stages. For now, the exploration map is not yet complete, as since there are only 2 battles, only not all NPCs will be displayed on the map yet. The game stats are also not fully balanced yet, but it is playable.

Scenes: Start Menu, Exploration Map, 2 Battle Scenes

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
Skill 1 (Scarlet Claw) - Deals damage to a single enemy and inflicts Bleed equal to 10% of Guiying's attack for 3 turns.
(+1 Chi)

Skill 2 (Dance of Fury) - Deals damage to all enemies and inflicts Bleed equal to 10% of Guiying's attack for 2 turns.
(-2 Chi)

Skill 3 (Secret Arts: Pounce) - Deals damage to all enemies, then if an enemy already has Bleed, all Bleed on that enemy will be activated and removed.
(-3 Chi)


3) Status Effects
Below are the current status effects in the game.

[Debuffs]
Bleed - Deals damage equal to a percentage of the attacker's attack every time the affected character’s turn begins for several turns. Maximum limit of 5 stacks.

Shatter - Lowers the defense of the affected character equal to a percentage of the affected character’s defense for several turns.

Stun - Forces the affected character to not be able to take any action on their next turn.

Taunt - Forces the affected character to only be able to attack the character who used the taunt.

[Buffs]
Enrage - Increases the attack of the affected character equal to a percentage of the affected character’s attack for several turns.

Armor - Increase the defense of the affected character equal to a percentage of the affected character’s defense for several turns.

Cleanse - Removes all negative effects of the affected character.

Berserk (Boss Only) - Increases the attack of the affected character significantly equal to 100% of the affected character's attack.
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
- Press 'F1' to toggle GodMode, player restores full health and do at least twice the normal damage.
- Press 'F2' to toggle EndGameMode, enemy's health decreased to zero and end the game with the player's victory.

4) Other Controls
- Press F11 for fullscreen.
================================================================

OTHERS:

1) HUD/UI
Below are a list of HUD that the player will see.

[Main HUD/UI]
Player Characters' HP - Displays the player characters and their HP (Top-left)
Enemy Characters' HP - Displays the enemy characters and their HP (Top-right)
Turn Order UI - Displays the turn order of all characters in the battle stage (Bottom-left)
Characters' Skills - Displays the characters' skills based on the current character that the player is controlling (Bottom-right)
Chi Points - Displays the amount of Chi that the player currently has represented by the fire icons (Bottom-right)


[Other HUD/UI]
Pause Menu (Press 'Esc' to toggle the Pause Menu)


2) Enemy Bosses
Currently, there is a Goat boss within the demo.

In the final version of the game, the Goat and the Ox characters will also be an enemy at a certain point of the game, where the Goat and the Ox will also be playable characters for now. There will also be a final boss, which will have elements of the Zodiac animals combined together.


3) Top-down Exploration Map
The player will first start on the map and navigate to the end goal. The player will then encounter different characters on the map and interact with them. Dialogues with unique conversations will be shown based on the character that they interacted with. After interacting with combat NPCs, the characters will then be transported to the turn-based battle scene.
================================================================

ImGui BUTTONS:

1) "Test Loader" tests deserialization of entitites from Json file

2) "Save Scene" tests serialization of all entities in current scene (test with 2500, Json file will save accordingly)

3) "Clone entity" Render an entity to test collision

4) "Create Entity by Button Test: Cat" Initial test to just create an entity using ImGui button

5) "Clone Cat Entity" Render the cat entity created by aforementioned button. 
MUST USE AFOREMENTIONED "Create Entity by Button Test: Cat" BUTTON BEFORE CLONING. PROPER ERROR HANDLING HAS BEEN IN PLACED TO ENSURE AN ENTITY MUST BE CREATD BEFORE CLONING. CLICK "NO" IF CLONING IS DONE BEFORE CREATING TO CONTINUE PROGRAM TO CREATE AN ENTITY FIRST AS ASSERT WILL HAPPEN.
================================================================

AUDIO:

Audio is implemented in all parts of the game.

================================================================
