
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
    AssetManager(FontManager fonts) : m_fonts(*fonts.GetFontCollection()) {};

    //General methods
    void LoadAssets(const std::string& assetPath, const std::string& assetName);

    // Texture methods
    void LoadTexture(const std::string& texturePath, const std::string& textureName);
    Texture* GetTexture(const std::string& textureName);
    void UnloadTexture(const std::string& textureName);

    // Audio methods
    void LoadAudio(const std::string& audioPath, const std::string& audioName);
    AudioManager* GetAudio(const std::string& audioName);
    void UnloadAudio(const std::string& audioName);

    // Font methods
    void LoadFont(const std::string& fontPath);
    Font GetFont(const std::string& fontFamily, const std::string& fontVariant);
    //void UnloadFont(const std::string& fontName);

private:
    std::unordered_map<std::string, Texture> m_textures;
    std::unordered_map<std::string, AudioManager> m_audio;
    std::unordered_map<std::string, FontEntry> m_fonts;
};