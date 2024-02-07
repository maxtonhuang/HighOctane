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

Currently, our prototype for M4 consists of the 1 battle stage, which is our second-to-second gameplay.

Scenes: Main Menu, 1 Battle Scene

================================================================
GAME FEATURES:

1) Player Chi System - Shared skill point resource system

- All characters on the player’s team will share Chi. (If one of the characters from the player’s team uses a special skill that consumes a huge amount of Chi, the next character might not be able to use a skill that consumes Chi)
- All characters on the player’s team will have multiple skills that consume different amounts of Chi.
- Every character can use their first skill to gain back one Chi.
- A variety of skills will be available for different situations, making skill points management crucial to winning battles.


2) Unique Player Skills
Currently, we only have 1 playable characters, the Cat, which is a damage dealer.

[Guiying - Cat]
Skill 1 (Scarlet Claw) - Deals damage to a single enemy and inflicts Bleed equal to 10% of Guiying's attack for 3 turns.
(+1 Chi)

Skill 2 (Dance of Fury) - Deals damage to all enemies and inflicts Bleed equal to 10% of Guiying's attack for 2 turns.
(-2 Chi)

Skill 3 (Five Elements Bloodbath) - Deals damage to all enemies, then if an enemy already has Bleed, all Bleed on that enemy will be activated and removed.
(-3 Chi)


3) Status Effects
Below are the current status effects in the game.

[Debuffs]
Bleed - Deals damage equal to a percentage of the attacker's attack every time the affected character’s turn begins for several turns. Maximum limit of 5 stacks.
================================================================

GAME CONTROLS:

1) Start Menu
Navigate through the start menu with mouse selection.

2) Battle Screen
Main Gameplay Controls
- Only mouse selection is needed.
- When it is your character's turn, select a skill.
- After selecting a skill, a popout will show which targets you can select.
- After selecting the target, the attack will be played out.
- Repeat steps 1-3, until the battle has ended.

3) Cheat Codes
- Press 'G' to toggle GodMode, player restores full health and do at least twice the normal damage.
- Press 'E' to toggle EndGameMode, enemy's health decreased to zero and end the game with the player's victory.

4) Other Controls
- Press F11 for fullscreen.
================================================================


//EDIT THE PARTS BELOW PLS//

ImGui BUTTONS:

1) "Test Loader" tests deserialization of entitites from Json file

2) "Save Scene" tests serialization of all entities in current scene (test with 2500, Json file will save accordingly)

3) "Clone entity" Render an entity to test collision

4) "Create Entity by Button Test: Cat" Initial test to just create an entity using ImGui button

5) "Clone Cat Entity" Render the cat entity created by aforementioned button. 
MUST USE AFOREMENTIONED "Create Entity by Button Test: Cat" BUTTON BEFORE CLONING. PROPER ERROR HANDLING HAS BEEN IN PLACED TO ENSURE AN ENTITY MUST BE CREATD BEFORE CLONING. CLICK "NO" IF CLONING IS DONE BEFORE CREATING TO CONTINUE PROGRAM TO CREATE AN ENTITY FIRST AS ASSERT WILL HAPPEN.
================================================================

AUDIO:

================================================================
