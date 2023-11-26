
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
*	@file		AssetManager.h
*
*	@author		Kai Alexander Van Adrichem Boogaert (Initial creation)
*
*	@email		kaialexander.v\@digipen.edu
*
*
*	@co-author	Foong Pun Yuen Nigel (Redone for loading of all assets)
*
*	@email		p.foong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		09 September 2023
*
* *****************************************************************************
*
*	@brief
*
*	This file contains all the definitions for managing assets such as textures,
*   audio and fonts.
******************************************************************************/
#pragma once
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include "AudioManager.h"
#include "Font.h"
#include "texture.h"
#include "Colors.h"
#include "Attack.h"

class AssetManager {
public:
    TextureManager texture;
    AudioManager audio;
    AttackList attacks;
public:
    /***********************************************
    GENERAL METHODS
    ***********************************************/
    /*
    Main load method of asset manager
    Input should be the asset name (without the folder directory)
    Eg texture.png instead of Folder/texture.png
    This method handles all asset types that the engine supports
    */
    void LoadAssets(const std::string& assetPath);

    //Returns the full folder directory that the game assets folder is located in
    std::string GetDefaultPath();

    //Returns a list of all assets loaded by the asset manager
    std::vector<std::string> GetFiles();

    //Initialises the asset manager
    void Initialize();

    //Unloads ALL assets, to be called when changing scene
    void UnloadAll();

    //Returns true if the path exists
    bool FileExists(const std::string& path);
    
    //Loads the scene using input scene path
    void LoadScene(const std::string& scenePath);

    //Saves the current scene to scene path
    void SaveScene(const std::string& scenePath);

    //Load all entities in the scene
    void LoadEntities(const std::string& entitiesPath);

    /***********************************************
    TEXTURE METHODS
    ***********************************************/
    //Loads texture using texture path
    void LoadTexture(const std::string& texturePath);
    //Loads spritesheet using spritesheet path
    void LoadSpritesheet(const std::string& spritePath);

    /***********************************************
    AUDIO METHODS
    ***********************************************/
    void LoadSound(const std::string& audioPath);
    void LoadMusic(const std::string& audioPath);

    /***********************************************
    FONT METHODS
    ***********************************************/
    void LoadFont(const std::string& fontPath);

    /***********************************************
    SHADER METHODS
    ***********************************************/
    void LoadRenderer(const std::string& rendererPath);

    /***********************************************
    ATTACK METHODS
    ***********************************************/
    void LoadAttack(const std::string& attackPath);

    /***********************************************
    PREFAB METHODS
    ***********************************************/
    //Loads prefab to ECS
    void LoadPrefab(const std::string& prefabPath);
    //Unloads prefab from ECS
    void UnloadPrefab(const std::string& prefabName);
    //Gets prefab entity ID using its name
    Entity GetPrefab(const std::string& prefabName);
    //Gets prefab name using its entity ID
    std::string GetPrefabName(Entity prefabID);
    //Updates prefab path with all prefabs in prefab folder
    void UpdatePrefabPaths();
    //Returns vector of strings of all prefabs in prefab folder (does not load them)
    std::vector<std::string> GetPrefabPaths();
    //Returns reference to the prefab map
    std::unordered_map<std::string, Entity>& GetPrefabMap(); 
    //Loads ALL prefabs, for editor asset library
    void LoadAllPrefabs();

private:
    std::string defaultPath{};
    std::vector<std::string> loadedFiles{};
    std::unordered_map<std::string, Entity> prefabMap{};
    std::vector<std::string> prefabPaths{}; //list of all prefabs in prefab folder
};

extern AssetManager assetmanager;
