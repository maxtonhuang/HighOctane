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
#include "graphics.h"

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
    fonts.Initialize();

    while (!serializer.stream.eof()) {
        path.clear();
        serializer.ReadString(path);
        if (path != "") {
            //DEBUG_PRINT("RECEIVED PATH: %s", path.c_str());
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
        texture.Add(path.c_str(), texturePath.c_str());
    }
    else {
        ASSERT(1, "Unable to open texture file!");
    }
}

void AssetManager::LoadSpritesheet(const std::string& spritePath) {
    std::string path{ defaultPath };
    path += "Textures/" + spritePath;
    if (FileExists(path)) {
        Serializer serializer;
        std::string textureName;
        int row;
        int column;
        int spritenum;
        serializer.Open(path);
        serializer.ReadString(textureName);
        serializer.ReadInt(row);
        serializer.ReadInt(column);
        serializer.ReadInt(spritenum);
        LoadAssets(textureName);
        texture.AddSpriteSheet(textureName.c_str(), row, column, spritenum);
    }
    else {
        ASSERT(1, "Unable to open sprite sheet file!");
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
    std::string path{ defaultPath };
    path += "Fonts/" + fontPath;
    if (FileExists(path)) {
        fonts.LoadFontFilePath(path);
    }
    else {
        ASSERT(1, "Unable to open font file!");
    }    
}

/**************************************SHADERS*************************************************/
void AssetManager::LoadRenderer(const std::string& rendererPath) {
    std::string path{ defaultPath };
    path += "Shaders/" + rendererPath;
    if (FileExists(path)) {
        Serializer serializer;
        std::string rendererName;
        std::string vertexShader;
        std::string fragmentShader;
        std::string typeName;
        GLenum type;
        serializer.Open(path);
        serializer.ReadString(rendererName);
        serializer.ReadString(vertexShader);
        serializer.ReadString(fragmentShader);
        serializer.ReadString(typeName);
        vertexShader = defaultPath + "Shaders/" + vertexShader;
        fragmentShader = defaultPath + "Shaders/" + fragmentShader;
        if (typeName == "GL_TRIANGLES") {
            type = GL_TRIANGLES;
        }
        else if (typeName == "GL_POINTS") {
            type = GL_POINTS;
        }
        else if (typeName == "GL_LINES") {
            type = GL_LINES;
        }
        else if (typeName == "GL_TRIANGLE_FAN") {
            type = GL_TRIANGLE_FAN;
        }
        else if (typeName == "GL_TRIANGLE_STRIP") {
            type = GL_TRIANGLE_STRIP;
        }
        else {
            ASSERT(1, "Renderer has unsupported draw type!");
            return;
        }
        graphics.AddRenderer(rendererName).Initialize(vertexShader.c_str(), fragmentShader.c_str(), type);
        //graphics.renderer[rendererName].Initialize(vertexShader.c_str(), fragmentShader.c_str(), type);
    }
    else {
        ASSERT(1, "Unable to open renderer file!");
    }
}

/**********************************GENERIC METHODS*********************************************/
bool AssetManager::FileExists(const std::string& path) {
    std::fstream f{ path };
    return f.good();
}

void AssetManager::LoadScene(const std::string& scenePath) {
    Serializer serializer;
    std::string path{ defaultPath };
    path += "Scenes/" + scenePath;
    if (serializer.Open(path)) {
        while (!serializer.stream.eof()) {
            path.clear();
            serializer.ReadString(path);
            if (path != "") {
                //DEBUG_PRINT("RECEIVED PATH: %s", path.c_str());
                LoadAssets(path);
            }
        }
    }
    else {
        ASSERT(true, "Unable to open scene file!");
    }

}

void AssetManager::LoadEntities(const std::string& entitiesPath) {
    std::string path{ defaultPath };
    path += "Scenes/" + entitiesPath;
    if (FileExists(path)) {
        Serializer::LoadEntityFromJson(path);
    }
    else {
        ASSERT(1, "Unable to open texture file!");
    }
}

void AssetManager::LoadAssets(const std::string& assetPath) {
    // Determine the asset type based on the file extension or other criteria
    
    std::string extension = FilePath::GetFileExtension(assetPath);

    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp") {
        // Load as a texture
        LoadTexture(assetPath);
    }
    else if (extension == ".spritesheet") {
        LoadSpritesheet(assetPath);
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
    else if (extension == ".renderer") {
        LoadRenderer(assetPath);
    }
    else if (extension == ".scn") {
        LoadScene(assetPath);
    }
    else if (extension == ".json") {
        LoadEntities(assetPath);
    }
    else {
        // Error Handling
        ASSERT(true,"Unsupported asset type: ");
    }
    loadedFiles.push_back(assetPath);
}
