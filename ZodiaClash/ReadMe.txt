ZODIACLASH README

================================================================

Game name: ZodiaClash
Version: v0.1 M1 build

Programmers:
Huang Maxton Xinghua (m.huang@digipen.edu)
Liu Wanting (wanting.liu@digipen.edu)
Foong Pun Yuen Nigel (p.foong@digipen.edu)
Koh Wen Yuen (k.wenyuan@digipen.edu)
Chua Zhen Rong (c.zhenrong@digipen.edu)
Van Adrichem Boogaert Kai Alexander (kaialexander.v@digipen.edu)

Designer:
Yau Yee Onn Oliver (y.yau@digipen.edu)

Artist:
Loo Zhi Ying (zhiying.loo@digipen.edu)

================================================================

CONTROLS:
W to move main character up
A to move main character left
S to move main character down
D to move main character right

O to scale main character up
P to scale main character down

Q to rotate main character anticlockwise
E to rotate main character clockwise

C to change main character animation

M to play sound 1
N to play sound 2

0 to toggle physics debug drawing mode, showing character position, velocity and AABB box
9 to toggle step by step mode
8 to move the character with respect to each frame

To test for Collison:
1. Click "Clone Entity" button in "Creating Entity" Panel
2. WASD to move main character(dynamic object) to collide with newly spawned static object

ImGui Buttons:
"Test Loader" tests deserialization of entitites from Json file

"Save Scene" tests serialization of all entities in current scene (test with 2500, Json file will save accordingly)

"Clone entity" Render an entity to test collision

"Create Entity by Button Test: Cat" Initial test to just create an entity using ImGui button

"Clone Cat Entity" Render the cat entity created by aforementioned button. 
MUST USE AFOREMENTIONED "Create Entity by Button Test: Cat" BUTTON BEFORE CLONING. PROPER ERROR HANDLING HAS BEEN IN PLACED TO ENSURE AN ENTITY MUST BE CREATD BEFORE CLONING. CLICK "NO" IF CLONING IS DONE BEFORE CREATING TO CONTINUE PROGRAM TO CREATE AN ENTITY FIRST AS ASSERT WILL HAPPEN.

 