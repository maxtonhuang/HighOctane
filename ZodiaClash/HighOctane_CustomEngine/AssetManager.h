
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
*	@author		Kai Alexander Van Adrichem Boogaert
*
*	@email		kaialexander.v\@digipen.edu
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
    audio and fonts.

*	!--Future Updates--!
*
*   -M1 checkpoint-
*	- Attempt at a general GetAssets and UnloadAssets functions
*   - Improve on functionalities look into implementing better functions to 
*     provide more functionalities
*   - Research more on what an asset manager needs
******************************************************************************/
#pragma once
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include "AudioManager.h"
#include "Font.h"
#include "texture.h"

class AssetManager {
public:
    TextureManager texture;
    AudioManager audio;
    //FontManager font;
public:
    void Initialize();
    void UnloadAll();

    //General methods
    bool FileExists(const std::string& path);
    void LoadAssets(const std::string& assetPath);
    void LoadScene(const std::string& scenePath);
    void LoadEntities(const std::string& entitiesPath);

    // Texture methods
    void LoadTexture(const std::string& texturePath);
    void LoadSpritesheet(const std::string& spritePath);
    Texture* GetTexture(const std::string& textureName);
    void UnloadTexture(const std::string& textureName);

    // Audio methods
    void LoadSound(const std::string& audioPath);
    void LoadMusic(const std::string& audioPath);
    AudioManager* GetAudio(const std::string& audioName);
    void UnloadAudio(const std::string& audioName);

    // Font methods
    void LoadFont(const std::string& fontPath);

    // Shader methods
    void LoadRenderer(const std::string& rendererPath);

private:
    std::string defaultPath{};
    std::vector<std::string> loadedFiles{};
    //std::unordered_multimap<std::string, FontEntry> m_fonts;
};

extern AssetManager assetmanager;
