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
*   - Research more on what an asset manager needs
******************************************************************************/

#include "AssetManager.h"
#include "File.h"
#include "debugdiagnostic.h"
#include "Serialization.h"

AssetManager assetmanager;

void AssetManager::Initialize() {
    const std::string initFilePath = "init.txt";
    std::string path{initFilePath};
    Serializer serializer;
    defaultPath = "Assets/";
    if (!serializer.Open(path)) {
        defaultPath = "../Assets/";
        path = defaultPath + initFilePath;
        if (!serializer.Open(path)) {
            ASSERT(1, "Unable to initialize asset manager!");
            return;
        }
    }

    audio.Initialize();

    while (!serializer.stream.eof()) {
        path.clear();
        serializer.ReadString(path);
        if (path != "") {
            LoadAssets(path);
        }
    }
}

void AssetManager::UnloadAll() {
    loadedFiles.clear();
    texture.Clear();
    audio.ReleaseAllSounds();
}

/**************************************TEXTURES**************************************************/
void AssetManager::LoadTexture(const std::string& texturePath) {
    std::string path{ defaultPath };
    path += "Textures/";
    path += texturePath;
    if (FileExists(path)) {
        texture.Add(path.c_str());
    }
    else {
        ASSERT(1, "Unable to open texture file!");
    }
}

/**************************************AUDIO**************************************************/
void AssetManager::LoadSound(const std::string& audioPath) {
    std::string path{ defaultPath };
    path += "Sound/" + audioPath;
    if (FileExists(path)) {
        audio.AddSound(path.c_str(), audioPath.c_str());
    }
    else {
        ASSERT(1, "Unable to open sound file!");
    }
}

void AssetManager::LoadMusic(const std::string& audioPath) {
    std::string path{ defaultPath };
    path += "Music/" + audioPath;
    if (FileExists(path)) {
        audio.AddMusic(path.c_str(), audioPath.c_str());
    }
    else {
        ASSERT(1, "Unable to open music file!");
    }
}


/**************************************FONTS**************************************************/
void AssetManager::LoadFont(const std::string& fontPath) {
    FontManager font;
    Font newFont;
    font.LoadNewFont(newFont, fontPath); // Load the font using your FontManager class.
    //m_fonts[fontName] = font;
}

Font AssetManager::GetFont(const std::string& fontFamily, const std::string& fontVariant) {
    //auto it = m_fonts.find(fontName);
    //if (it != m_fonts.end()) {
    //    return &it->second;
    //}
    //return nullptr; // Font not found.
    FontManager font;
    return font.GetFont(fontFamily, fontVariant);
}

// note: might not need, FontManager auto destructs on out of scope
//void AssetManager::UnloadFont(const std::string& fontName) {
//    //auto it = m_fonts.find(fontName);
//    //if (it != m_fonts.end()) {
//    //    // Release resources related to the font using your FontManager class if necessary.
//    //    m_fonts.erase(it);
//    //}
//}


/**********************************GENERIC METHODS*********************************************/
bool AssetManager::FileExists(const std::string& path) {
    std::fstream f{ path };
    return f.good();
}

void AssetManager::LoadAssets(const std::string& assetPath) {
    // Determine the asset type based on the file extension or other criteria
    std::string extension = FilePath::GetFileExtension(assetPath);

    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp") {
        // Load as a texture
        LoadTexture(assetPath);
    }
    else if (extension == ".mp3" || extension == ".wav" || extension == ".ogg") {
        // Load as audio
        std::string soundPath{ defaultPath };
        soundPath += "Sound/" + assetPath;
        if (FileExists(soundPath)) {
            LoadSound(assetPath);
        }
        else {
            LoadMusic(assetPath);
        }
        
    }
    else if (extension == ".ttf" || extension == ".otf") {
        // Load as font
        LoadFont(assetPath);
    }
    else {
        // Error Handling
        ASSERT(true,"Unsupported asset type: ");
    }
    loadedFiles.push_back(assetPath);
}
