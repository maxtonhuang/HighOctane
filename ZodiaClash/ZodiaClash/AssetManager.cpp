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
*	@file		AssetManager.cpp
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
*	- Attempt at a general get and unload functions
*   - Improve on functionalities look into implementing better functions to 
*     provide more functionalities
******************************************************************************/

#include "AssetManager.h"
#include "File.h"
#include "debugdiagnostic.h"

/**************************************TEXTURES**************************************************/
void AssetManager::LoadTexture(const std::string& texturePath, const std::string& textureName) {
    Texture texture;
    texture.Init(texturePath.c_str()); // Initialize the texture using your Texture class.
    m_textures[textureName] = texture;
}

Texture* AssetManager::GetTexture(const std::string& textureName) {
    auto it = m_textures.find(textureName);
    if (it != m_textures.end()) {
        return &it->second;
    }
    return nullptr; // Texture not found.
}

void AssetManager::UnloadTexture(const std::string& textureName) {
    auto it = m_textures.find(textureName);
    if (it != m_textures.end()) {
        it->second.FreeTexture(); // Free the texture from OpenGL memory using your Texture class.
        m_textures.erase(it);
    }
}

/**************************************AUDIO**************************************************/
void AssetManager::LoadAudio(const std::string& audioPath, const std::string& audioName) {
    AudioManager loadAudio;
    loadAudio.AddSound(audioPath.c_str());
    m_audio[audioName] = loadAudio;
}

AudioManager* AssetManager::GetAudio(const std::string& audioName) {
    auto it = m_audio.find(audioName);
    if (it != m_audio.end()) {
        return &it->second;
    }
    return nullptr; // Audio not found.
}

void AssetManager::UnloadAudio(const std::string& audioName) {
    AudioManager loadAudio;
    loadAudio.FreeSound(audioName.c_str());
}


/**************************************FONTS**************************************************/
void AssetManager::LoadFont(const std::string& fontPath, const std::string& fontName) {
    FontManager font;
    font.LoadFont(fontPath); // Load the font using your FontManager class.
    m_fonts[fontName] = font;
}

FontManager* AssetManager::GetFont(const std::string& fontName) {
    auto it = m_fonts.find(fontName);
    if (it != m_fonts.end()) {
        return &it->second;
    }
    return nullptr; // Font not found.
}

void AssetManager::UnloadFont(const std::string& fontName) {
    auto it = m_fonts.find(fontName);
    if (it != m_fonts.end()) {
        // Release resources related to the font using your FontManager class if necessary.
        m_fonts.erase(it);
    }
}

void AssetManager::LoadAssets(const std::string& assetPath, const std::string& assetName) {
    // Determine the asset type based on the file extension or other criteria
    std::string extension = FilePath::GetFileExtension(assetPath);

    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp") {
        // Load as a texture
        LoadTexture(assetPath, assetName);
    }
    else if (extension == ".mp3" || extension == ".wav" || extension == ".ogg") {
        // Load as audio
        LoadAudio(assetPath, assetName);
    }
    else if (extension == ".ttf" || extension == ".otf") {
        // Load as font
        LoadFont(assetPath, assetName);
    }
    else {
        // Error Handling
        ASSERT(true,"Unsupported asset type: ");
    }
}
