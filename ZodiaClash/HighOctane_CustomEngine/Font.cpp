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
*	@file		Font.cpp
*
*	@author		Chua Zhen Rong (FreeType font library setup)
*
*	@email		c.zhenrong\@digipen.edu
*
*	@co-author	Foong Pun Yuen Nigel
*
*	@email		p.foong\@digipen.edu (OpenGL texture creation from fonts)
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		28 September 2023
*
* *****************************************************************************
*
*	@brief		Font System
*
*   This file contains all the functions needed for integrating the
    font library into the game engine.

    [M1] 2 font objects created. Needs further polishing
    [M2] Reading from font directory to store more than 1 font file.
            - Revamped to account for loading only requested fonts (on demand), 
            dropped unnecessary structures
            - Revamped to account for serialization and imgui support 
            (combo box selection)
*
******************************************************************************/

#include "Font.h"
#include "AssetManager.h"
#include <iostream>

FontManager fonts;

GLuint font_vaoid;
GLuint font_vboid;

namespace fileSys = std::filesystem;

/*********************************
* FONTMANAGER CLASS OBJ FUNCTIONS
*********************************/

/*!
* \brief Destructor
*
* This function is automatically called on termination.
* Frees both the FontFace and FreeType library stored during initialization.
*
*/
FontManager::~FontManager() {
    //FT_Done_Face(fonts.ft_font.fontFace);
    //FT_Done_Face(fontCollection["Danto Lite Normal.ttf"].fontFace);
    for (auto& fontPair : fontCollection) {
        FT_Done_Face(fontPair.second.font.fontFace);
    }
	FT_Done_FreeType(fontLibrary);
}

/*!
* \brief Initializer
*
* This function is called during AssetsManager initialization.
* Initializes a new FreeType library object, and passes a string of a font file's
* filepath into LoadFont function. Font file must be present in Assets/Fonts/.
*
*/
void FontManager::Initialize() {
    //DEBUG_PRINT("___ DEBUG::FONT::INITIALIZE ___");
    // initialize freetype library
    FT_Error err;
    err = FT_Init_FreeType(&fontLibrary);
    ASSERT(err, "Failed to initialise FreeType!");

    ReadFonts();
    SetDefaultFont();
}

/*!
* \brief Initializer - ReadFonts
*
* This function is called during initialization.
* Goes through directory to populate fontCollection and fontPairs entries!
* Establishes Font object, stores filepath and filename strings.
* Loading is NOT done here.
*
*/
void FontManager::ReadFonts() {
    //DEBUG_PRINT("___ DEBUG::FONT::READFONTS ___");
    // define parent fonts directory to load from
    //std::string parentDir = "../Assets/Fonts/";
    std::string parentDir = assetmanager.GetDefaultPath() + "Fonts/";

    // iterate through folders in parentDir
    for (const fileSys::directory_entry& dirItem : fileSys::directory_iterator(parentDir)) {
        // capture folder names in parentDir as font family names
        if (!fileSys::is_directory(dirItem))
            continue;

        std::string ftFamily = dirItem.path().filename().string();
        //DEBUG_PRINT("DEBUG::FONT: >>> Font family: %s", ftFamily.c_str());

        //reset variables
        std::string ftFilename{}, fileExt{}, ftVariant{};
        size_t foundVarStart{}, foundVarEnd{};

        // iterate through file items in one font parent directory
        for (const fileSys::directory_entry& fileItem : fileSys::directory_iterator(dirItem.path())) {
            fileExt = fileItem.path().extension().string();
            ftFilename = fileItem.path().filename().string();
            
            // ignore files that are not font files
            if (!fileSys::is_regular_file(fileItem) || (!(fileExt == ".ttf") && !(fileExt == ".otf")))
                continue;

            foundVarStart = ftFilename.find_last_of('-');
            foundVarEnd = ftFilename.find_last_of('.');
            ftVariant = (foundVarStart != std::string::npos) ? ftFilename.substr(foundVarStart + 1, foundVarEnd - foundVarStart - 1) : "Regular";

            // DO DUPLICATION CHECKS!!            
            if (CheckVariantName(ftFamily, ftVariant)) {
                DEBUG_PRINT("ERROR::FONT: Duplicate variant found: %s, ensure all variants are properly named!", ftVariant.c_str());
                continue;
            }

            // construct font's filepath
            std::string ftFilePath = parentDir + ftFamily + "/" + ftFilename;

            // store in fontPairs
            fontPairs.push_back(std::make_pair(std::make_unique<std::string>(ftFamily), std::make_unique<std::string>(ftVariant)));


            // construct Font with font family and font variant string references
            Font ft_data(*fontPairs.back().first, *fontPairs.back().second);

            //construct FontEntry
            FontEntry ft_entry(ft_data, ftFilePath, ftFilename);

            // store in fontCollection
            fontCollection.emplace(ftFamily, ft_entry);

            //DEBUG_PRINT("DEBUG::FONT: [+] Added to font collection: %s %s", ftFamily.c_str(), ftVariant.c_str());
        }
    }
}

/*!
* \brief Load - LoadFontVariant
*
* Given fontFamily and fontVariant strings stored in a Font object,
* retrieves corresponding FontEntry to extract filepath to load font file.
*
*/
void FontManager::LoadFontVariant(const std::string& ftFamily, const std::string& ftVariant) {
    //DEBUG_PRINT("___ DEBUG::FONT::LOAD FONT VARIANT ___");
    FontEntry* ftEntryPtr = GetFontEntryByVariant(ftFamily, ftVariant);
    if (ftEntryPtr == nullptr) {
        DEBUG_PRINT("ERROR::FONT: (var) Requested font NOT found.");
        return;
    }
    LoadValidFont((*ftEntryPtr).font, (*ftEntryPtr).fontFilePath);
}

/*!
* \brief Load - LoadFontEntry
*
* Called in GetFont once it is determined the Font object has no glyphs loaded.
* Retrieves FontEntry as input to extract filepath to load font file.
*
*/
void FontManager::LoadFontEntry(FontEntry& fontEntryData) {
    //DEBUG_PRINT("___ DEBUG::FONT::LOAD FONT ENTRY ___");
    if (&fontEntryData == nullptr) {
        DEBUG_PRINT("ERROR::FONT: (entry) Requested font NOT found.");
        return;
    }
    LoadValidFont(fontEntryData.font, fontEntryData.fontFilePath);
}

/*!
* \brief Load - LoadFontFilePath
*
* Called in AssetsManager LoadFont when initialization default font.
* Retrieves FontEntry as input to extract Font object to load glyphs into.
*
*/
void FontManager::LoadFontFilePath(const std::string ftFilePath) {
    //DEBUG_PRINT("___ DEBUG::FONT::LOAD FONT FILEPATH ___");
    FontEntry* ftEntryPtr = GetFontEntryByFilePath(ftFilePath);
    if (ftEntryPtr == nullptr) {
        DEBUG_PRINT("ERROR::FONT: (path) Requested font NOT found.");
        return;
    }
    LoadValidFont((*ftEntryPtr).font, (*ftEntryPtr).fontFilePath);
}

/*!
* \brief Loads a font file
*
* Given the font file's filepath, the font file is used to open, read and extract glyphs.
*
*/
void FontManager::LoadValidFont(Font& fontData, const std::string& fontFilePath) {
    //DEBUG_PRINT("___ DEBUG::FONT::LOAD VALID FONT ___");
    // if fontData's characters already has textures loaded, return
    if (!fontData.characters.empty())
        return;

    //DEBUG_PRINT("DEBUG::FONT: Attempting to load: %s", fontFilePath.c_str());
    //Load font face
    FT_Error err;
    err = FT_New_Face(fonts.fontLibrary, fontFilePath.c_str(), 0, &fontData.fontFace);
    switch (err) {
    case(FT_Err_Unknown_File_Format):
        DEBUG_PRINT("ERROR::FONT: Able to open and read font, but font format is unsupported");
        break;
    case(FT_Err_Cannot_Open_Resource):
        DEBUG_PRINT("ERROR::FONT: Unable to open resource!");
        break;
    case(FT_Err_Ok):
        //DEBUG_PRINT("DEBUG::FONT: Loaded: %s", fontFilePath.c_str());
        // Define pixel font size
        // set pixel height to 100, so relFontSize can be 0.XXf to 1.0f
        FT_Set_Pixel_Sizes(fontData.fontFace, 0, 100);
        assetmanager.texture.Add(fontData);
        //LoadChar(fontData);
        break;
    default:
        DEBUG_PRINT("ERROR::FONT: Font file cannot be open or read!");
    }

    if (fontData.characters.empty()) {
        FT_Done_Face(fontData.fontFace);
        return;
    }

    // add loaded font into loadedFilePaths
    std::string parentDir = assetmanager.GetDefaultPath() + "Fonts/";
    size_t pos = fontFilePath.find(parentDir);
    // if string starts with parentDir, remove prefix, else just store as is
    (pos == 0) ? 
        loadedFilePaths.push_back(fontFilePath.substr(pos + parentDir.length()))
        : loadedFilePaths.push_back(fontFilePath);

    for (const auto& pair : fontData.characters) {
        int yOffset = pair.second.bearing.y - pair.second.size.y;

        if (yOffset < fontData.largestNegativeOffset) {
            fontData.largestNegativeOffset = yOffset;
        }
    }

    DEBUG_PRINT("DEBUG::FONT: Loaded requested font: %s", fontFilePath.c_str());    
}

void FontManager::Clear() {
    for (auto& t : fontCollection) {
        t.second.font.characters.clear();
    }
}

/*!
* \brief Checker (family name)
*
* Duplicate checking. Returns true if fontCollection has registered the fontFamily.
*
*/
bool FontManager::CheckFamilyName(const std::string& ftFamily) {
    //DEBUG_PRINT("___ DEBUG::FONT::CHECK FAMILY NAME ___");
    using fontCollectionType = std::unordered_multimap<std::string, FontEntry>;

    std::pair<fontCollectionType::iterator, fontCollectionType::iterator> variantRange = fontCollection.equal_range(ftFamily);

    return (variantRange.first != fontCollection.end());

}

/*!
* \brief Checker (variant name)
*
* Duplicate checking. Returns true if fontCollection has registered the fontVariant.
*
*/
bool FontManager::CheckVariantName(const std::string& ftFamily, const std::string& ftVariant) {
    //DEBUG_PRINT("___ DEBUG::FONT::CHECK VARIANT NAME ___");

    using fontCollectionType = std::unordered_multimap<std::string, FontEntry>;

    std::pair <fontCollectionType::iterator, fontCollectionType::iterator> variantRange = fontCollection.equal_range(ftFamily);

    for (fontCollectionType::iterator it = variantRange.first; it != variantRange.second; it++) {
        if (it->second.font.GetInfo().second == ftVariant) {
            return true;
        }
    }
    return false;
}


/*!
* \brief Generic Font getter
*
* Retrieves Font class object to access character map stored.
*
*/
Font* FontManager::GetFont(const std::string& ftFamily, const std::string& ftVariant) {
    using fontCollectionType = std::unordered_multimap<std::string, FontEntry>;
    //DEBUG_PRINT("DEBUG::FONT: RECEIVED family: %s", ftFamily.c_str());
    //DEBUG_PRINT("DEBUG::FONT: RECEIVED variant: %s", ftVariant.c_str());

    std::pair <fontCollectionType::iterator, fontCollectionType::iterator> variantRange = fontCollection.equal_range(ftFamily);

    // explicitly putting this in
    if (variantRange.first == fontCollection.end()) {
        DEBUG_PRINT(">>> ERROR::FONT: Failed to retrieve requested font family %s", ftFamily.c_str());
        return nullptr;
    }

    for (fontCollectionType::iterator it = variantRange.first; it != variantRange.second; it++) {
        //DEBUG_PRINT("DEBUG::FONT: Reading family: %s", it->first.c_str());
        //DEBUG_PRINT("DEBUG::FONT: Reading variant: %s", it->second.fontVariant.c_str());
        if (it->second.font.GetInfo().second == ftVariant) {

            // if character textures not loaded, load textures
            if (it->second.font.characters.empty()) {
                //LoadFontVariant(ftFamily, ftVariant);
                LoadFontEntry(it->second);
            }

            return &it->second.font;
        }
    }
    DEBUG_PRINT("ERROR::FONT: Failed to retrieve requested font variant %s", ftVariant.c_str());
    return nullptr;
}

/*!
* \brief Default Font getter
*
* Retrieves Font class object to access character map stored.
*
*/
Font* FontManager::GetDefaultFont() {
    //DEBUG_PRINT("___ DEBUG::FONT::GET DEFAULT FONT ___");
    return defaultFont;
}

/*!
* \brief FontEntry getter (by variant)
*
* Retrieves FontEntry class object
* For access to mainly both Font class object and fontFilePath attributes
* Given font family strings and font variant strings, returns pointer stored in fontCollection
*
*/
FontEntry* FontManager::GetFontEntryByVariant(const std::string& ftFamily, const std::string& ftVariant) {
    //DEBUG_PRINT("___ DEBUG::FONT::GET ENTRY BY VARIANT ___");
    using fontCollectionType = std::unordered_multimap<std::string, FontEntry>;
    //DEBUG_PRINT("DEBUG::FONT: RECEIVED family: %s", ftFamily.c_str());
    //DEBUG_PRINT("DEBUG::FONT: RECEIVED variant: %s", ftVariant.c_str());

    std::pair <fontCollectionType::iterator, fontCollectionType::iterator> variantRange = fontCollection.equal_range(ftFamily);

    for (fontCollectionType::iterator it = variantRange.first; it != variantRange.second; it++) {
        //DEBUG_PRINT("DEBUG::FONT: Reading family: %s", it->first.c_str());
        //DEBUG_PRINT("DEBUG::FONT: Reading variant: %s", it->second.fontVariant.c_str());
        if (it->second.font.GetInfo().second == ftVariant) {
            return &it->second;
        }
    }
    DEBUG_PRINT("ERROR::FONT: Failed to retrieve requested font entry by variant");
    return nullptr;
}

/*!
* \brief FontEntry getter (by filepath)
*
* Retrieves FontEntry class object
* For access to mainly both Font class object and fontFilePath attributes
* Given only filepath string, returns pointer stored in fontCollection.
* Identifies font family in filepath to narrow down search in fontCollection
*
*/
FontEntry* FontManager::GetFontEntryByFilePath(const std::string& ftFilePath) {
    //DEBUG_PRINT("___ DEBUG::FONT::GET ENTRY BY PATH ___");
    std::string ftFamily = fileSys::path(ftFilePath).parent_path().filename().string();
    //DEBUG_PRINT("DEBUG::FONT: EXTRACTED family: %s", ftFamily.c_str());

    using fontCollectionType = std::unordered_multimap<std::string, FontEntry>;

    std::pair <fontCollectionType::iterator, fontCollectionType::iterator> variantRange = fontCollection.equal_range(ftFamily);

    for (fontCollectionType::iterator it = variantRange.first; it != variantRange.second; it++) {
        //DEBUG_PRINT("DEBUG::FONT: Reading family: %s", it->first.c_str());
        //DEBUG_PRINT("DEBUG::FONT: Reading variant: %s", it->second.fontVariant.c_str());
        if (it->second.fontFilePath == ftFilePath) {
            return &it->second;
        }
    }
    DEBUG_PRINT("ERROR::FONT: Failed to retrieve requested font entry by filepath");
    return nullptr;
}

/*!
* \brief fontCollection getter
*
* Retrieves fontCollection stored in FontManager
* For iterating to get or updating Font objects stored, PK: font family string
*
*/
std::unordered_multimap<std::string, FontEntry>* FontManager::GetFontCollection() {
    //DEBUG_PRINT("___ DEBUG::FONT::GET FONTCOLLECTION ___");
    return &fontCollection;
}

/*!
* \brief fontPairs getter
*
* Retrieves fontPairs stored in FontManager
* Contains string pointers to Font objects stored in fontCollection (prevent duplicate storage)
* Mainly for use by GetFontFamilyList() and GetFontVariantList()
*
*/
const std::vector<std::pair<std::unique_ptr<std::string>, std::unique_ptr<std::string>>>* FontManager::GetFontPairs() const {
    //DEBUG_PRINT("___ DEBUG::FONT::GET FONTPAIRS ___");
    return &fontPairs;
}

const std::vector<std::string>* FontManager::GetLoadedFilePaths() const {
    return &loadedFilePaths;
}

/*!
* \brief fontFamily list getter
*
* Retrieves list of font family strings in fontPairs
* Mainly for ImGui usage: dropdown selection for font family
* when selecting UI component with text
*
*/
std::vector<std::string> FontManager::GetFontFamilyList() {
    //DEBUG_PRINT("___ DEBUG::FONT::GET FAMILY LIST ___");
    // extract pointer to fontPairs in FontManager
    const std::vector<std::pair<std::unique_ptr<std::string>, std::unique_ptr<std::string>>>* fontPairList = GetFontPairs();
    
    // set to ensure uniqueness
    std::set<std::string> fontFamilyList;

    // iterate through fontPairList to get fontFamily strings
    for (const std::pair<std::unique_ptr<std::string>, std::unique_ptr<std::string>>& pair : *fontPairList) {
        fontFamilyList.insert(*(pair.first));
    }

    //convert to vector
    return std::vector<std::string>(fontFamilyList.begin(), fontFamilyList.end());
}

/*!
* \brief fontVariant list getter
*
* Retrieves list of font variant strings in fontPairs, given set font family
* Mainly for ImGui usage: dropdown selection for font variant
* when selecting UI component with text
*
*/
std::vector<std::string> FontManager::GetFontVariantList(const std::string& ftFamily) {
    //DEBUG_PRINT("___ DEBUG::FONT::GET VARIANT LIST ___");
    // extract pointer to fontPairs in FontManager
    const std::vector<std::pair<std::unique_ptr<std::string>, std::unique_ptr<std::string>>>* fontPairList = GetFontPairs();

    // set to ensure uniqueness
    std::set<std::string> fontVariantList;

    // iterate through fontPairList to get fontFamily strings
    for (const std::pair<std::unique_ptr<std::string>, std::unique_ptr<std::string>>& pair : *fontPairList) {
        if (*pair.first == ftFamily) {
            fontVariantList.insert(*(pair.second));
        }
    }

    //convert to vector
    return std::vector<std::string>(fontVariantList.begin(), fontVariantList.end());
}

/*!
* \brief defaultFont setter
*
* Sets default font. Called in Initialize() unless otherwise modified.
*
*/
void FontManager::SetDefaultFont() {
    //DEBUG_PRINT("___ DEBUG::FONT::SET DEFAULT FONT ___");
    FontEntry* ftEntryPtr = fonts.GetFontEntryByVariant("Danto Lite Normal", "Regular");
    if (ftEntryPtr == nullptr) {
        DEBUG_PRINT("ERROR::FONT: Requested default font NOT found.");
        return;
    }

    defaultFont = &ftEntryPtr->font;
}

/**************************
* FONT CLASS OBJ FUNCTIONS
**************************/

std::pair<std::string, std::string> Font::GetInfo() {
    /*DEBUG_PRINT("DEBUG::FONT: Recieved family: %s", (*fontFamily).c_str());
    DEBUG_PRINT("DEBUG::FONT: Received variant: %s", (*fontVariant).c_str());*/
    if (!fontFamily.empty() && !fontVariant.empty()) {
        /*std::cout << "DEBUG::FONT: Recieved family:" << fontFamily << std::endl;
        std::cout << "DEBUG::FONT: Recieved variant:" << fontVariant << std::endl;*/
        return std::make_pair(fontFamily, fontVariant);
    }
    else {
        return std::make_pair("empty", "empty");
    }   
}