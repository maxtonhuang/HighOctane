
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
    void Initialize();
    void UnloadAll();

    //General methods
    bool FileExists(const std::string& path);
    void LoadAssets(const std::string& assetPath);
    void LoadScene(const std::string& scenePath);
    void LoadEntities(const std::string& entitiesPath);
    std::vector<std::string> GetFiles();
    std::string GetDefaultPath();

    // Texture methods
    void LoadTexture(const std::string& texturePath);
    void LoadSpritesheet(const std::string& spritePath);

    // Audio methods
    void LoadSound(const std::string& audioPath);
    void LoadMusic(const std::string& audioPath);

    // Font methods
    void LoadFont(const std::string& fontPath);

    // Shader methods
    void LoadRenderer(const std::string& rendererPath);

    //Attack methods
    void LoadAttack(const std::string& attackPath);

    //Prefab methods
    void LoadPrefab(const std::string& prefabPath);
    void UnloadPrefab(const std::string& prefabName);
    Entity GetPrefab(const std::string& prefabName);
    std::string GetPrefabName(Entity prefabID);
    void UpdatePrefabPaths();
    std::vector<std::string> GetPrefabPaths();
    std::unordered_map<std::string, Entity>& GetPrefabMap();
    void LoadAllPrefabs(); //Loads ALL prefabs, for editor asset library

private:
    std::string defaultPath{};
    std::vector<std::string> loadedFiles{};
    std::unordered_map<std::string, Entity> prefabMap{};
    std::vector<std::string> prefabPaths{}; //list of all prefabs in prefab folder
    //std::unordered_multimap<std::string, FontEntry> m_fonts;
};

extern AssetManager assetmanager;
