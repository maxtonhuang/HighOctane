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
    [M2] Reading from font directory to store more than 1 font file
*
******************************************************************************/

#include "Font.h"
#include <iostream>
#include "AssetManager.h"

FontManager fonts;

GLuint font_vaoid;
GLuint font_vboid;

namespace fileSys = std::filesystem;

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
	FT_Done_FreeType(fonts.fontLibrary);
}

/*!
* \brief Initializer
*
* This function is called during Graphics initialization.
* Initializes a new FreeType library object, and passes a string of a font file's
* filepath into LoadFont function. Font file must be present in Assets/Fonts/.
*
*/
void FontManager::Initialize() {
    // initialize freetype library
    FT_Error err;
    err = FT_Init_FreeType(&fonts.fontLibrary);
    ASSERT(err, "Failed to initialise FreeType!");

    // load all fonts in fonts directory
    fonts.LoadAllFonts();

    // tmp, to remove once fontCollection is stable
    //fonts.LoadFont("Danto Lite Normal.ttf");
}

void FontManager::LoadAllFonts() {
    // define parent fonts directory to load from
    std::string parentDir = "../Assets/Fonts/";

    //bool tmpFlag{};

    // iterate through folders in parentDir
    for (const auto& dirItem : fileSys::directory_iterator(parentDir)) {
        // capture folder names in parentDir as font family names
        if (fileSys::is_directory(dirItem)) {
            std::string ftFamily = dirItem.path().filename().string();
            DEBUG_PRINT("DEBUG::FONT: >>> Font family: %s", ftFamily.c_str());

            //reset variables
            std::string ftFilename{}, fileExt{}, ftVariant{};
            size_t foundVarStart{}, foundVarEnd{};

            // iterate through file items in one font parent directory
            for (const auto& fileItem : fileSys::directory_iterator(dirItem.path())) {
                if (fileSys::is_regular_file(fileItem)) {
                    // ignore files that are not font files
                    ftFilename = fileItem.path().filename().string();
                    foundVarStart = ftFilename.find_first_of('-');
                    foundVarEnd = ftFilename.find_last_of('.');
                    fileExt = (foundVarEnd != std::string::npos) ? ftFilename.substr(foundVarEnd) : "";

                    if ((fileExt == ".ttf") || (fileExt == ".otf")) {
                        ftVariant = (foundVarStart != std::string::npos) ? ftFilename.substr(foundVarStart + 1, foundVarEnd - foundVarStart - 1) : "";

                        // DO DUPLICATION CHECKS!!
                        if (foundVarStart == std::string::npos) {
                            if (CheckVariantName(ftFamily, "Regular")) {
                                DEBUG_PRINT("ERROR::FONT: Font file with unspecified font variant and \"Regular\" variant already exists. Ensure all variants are properly named!");
                                continue;
                            }
                            ftVariant = "Regular";
                        }
                        if (CheckVariantName(ftFamily, ftVariant)) {
                            DEBUG_PRINT("ERROR::FONT: Duplicate variant found: %s, ensure all variants are properly named!", ftVariant.c_str());
                            continue;
                        }
                    
                        // check if all data is present, then proceed extract data for font file
                        if (!ftFamily.empty() && !ftVariant.empty()) {
                            // construct font's filepath
                            std::string fontFilePath = parentDir + ftFamily + "/" + ftFilename;
                    
                            // init new font obj
                            Font ft_data;
                            
                            LoadValidFont(ft_data, fontFilePath);
                    
                            // prepare new entry to load into fontCollection
                            ft_data.isActive = 1;
                            
                            //FontInfo ft_info(ft_family, ft_variant);
                            FontEntry ft_entry(ft_data, ftVariant, ftFilename);
                    
                            // store in fontCollection
                            fontCollection.emplace(ftFamily, ft_entry);
                            
                            DEBUG_PRINT("DEBUG::FONT: [+] Added to font collection: %s %s", ftFamily.c_str(), ftVariant.c_str());
                        }
                    }
                    else {
                        DEBUG_PRINT("DEBUG::FONT: Skipped over file: %s", ftFilename.c_str());
                    }
                    
                }
            }

        }
    }
}

/*!
* \brief Loads a font file
*
* Given the font file's filepath, the font file is used to open, read and extract glyphs.
*
*/
void FontManager::LoadValidFont(Font& fontData, const std::string& fontFilePath) {
    //DEBUG_PRINT("DEBUG::FONT: Attempting to load: %s", fontFilePath.c_str());
    //Load font face
    FT_Error err;
    err = FT_New_Face(fonts.fontLibrary, fontFilePath.c_str(), 0, &fontData.fontFace);
    if (err == FT_Err_Unknown_File_Format) {
        DEBUG_PRINT("ERROR::FONT: Able to open and read font, but font format is unsupported");
    }
    else if (err == FT_Err_Cannot_Open_Resource) {
        DEBUG_PRINT("ERROR::FONT: Unable to open resource!");
    }
    else if (err) {
        DEBUG_PRINT("ERROR::FONT: Font file cannot be open or read!");
    }
    else {
        DEBUG_PRINT("DEBUG::FONT: Loaded: %s", fontFilePath.c_str());
        // Define pixel font size
        // set pixel height to 100, so relFontSize can be 0.XXf to 1.0f
        FT_Set_Pixel_Sizes(fontData.fontFace, 0, 100);
        assetmanager.texture.Add(fontData);
        //LoadChar(fontData);
    }
}


/*!
* \brief Loads a font file
*
* Given the font file's filepath, the font file is used to open, read and extract glyphs.
* [NOTE] OLD ver, to replace later
* [NOTE] keep for now for AssetManager
*
*/
void FontManager::LoadNewFont(Font& fontData, const std::string& fontPath) {
	//Load font face
	FT_Error err;
    std::string fontFilePath = "../Assets/Fonts/" + fontPath;
	err = FT_New_Face(fonts.fontLibrary, fontFilePath.c_str(), 0, &fontData.fontFace);
	if (err == FT_Err_Unknown_File_Format) {
		DEBUG_PRINT("ERROR::FONT: Able to open and read font, but font format is unsupported");
	}
	else if (err) {
        DEBUG_PRINT("!!ERROR::FONT: Font file cannot be open or read!");
	}
    else {
        // Define pixel font size
        // set pixel height to 100, so relFontSize can be 0.XXf to 1.0f
        FT_Set_Pixel_Sizes(fontData.fontFace, 0, 100);
        //LoadChar(fontData);
        assetmanager.texture.Add(fontData);

        // with textures loaded, add font object to fontCollection
        //fontCollection.emplace(fontPath, font);
    }	
}

/*!
* \brief Loads character glyphs in the font file
*
* Each character glyph is loaded to generate a texture, which is then
* stored in an unordered map for efficient querying access.
*
*/
/*
void FontManager::LoadChar(Font& fontData) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    //for all printable characters [32, 127], 169 (copyright) and 174 (registered copyright)
    unsigned int fontWidth = 0;
    unsigned int fontHeight = 0;
    
    //first pass, get font sprite sheet size
    for (unsigned char c = 32; c < 175; c++)
    {
        //skip over characters not in intended range
        if (((c > 127) && (c < 169)) || ((c > 169) && (c < 174))) {
            continue;
        }

        // load character glyph
        if (FT_Load_Char(fontData.fontFace, c, FT_LOAD_RENDER))
        {
            DEBUG_PRINT("ERROR::FONT: Failed to load Glyph");
            continue;
        }
        // generate texture
        if (fontWidth < fontData.fontFace->glyph->bitmap.width) {
            fontWidth = fontData.fontFace->glyph->bitmap.width;
        }
        fontHeight += fontData.fontFace->glyph->bitmap.rows;
    }


    for (unsigned char c = 32; c < 175; c++)
    {
        //skip over characters not in intended range
        if (((c > 127) && (c < 169)) || ((c > 169) && (c < 174))) {
            continue;
        }

        // load character glyph
        if (FT_Load_Char(fontData.fontFace, c, FT_LOAD_RENDER))
        {
            DEBUG_PRINT("ERROR::FONT: Failed to load Glyph");
            continue;
        }
        // generate texture
        if (fontWidth < fontData.fontFace->glyph->bitmap.width) {
            fontWidth = fontData.fontFace->glyph->bitmap.width;
        }
        fontHeight += fontData.fontFace->glyph->bitmap.rows;
        unsigned int texture;
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureStorage2D(texture, 1, GL_R8, fontData.fontFace->glyph->bitmap.width,
            fontData.fontFace->glyph->bitmap.rows);
        glTextureSubImage2D(texture, 0, 0, 0, fontData.fontFace->glyph->bitmap.width,
            fontData.fontFace->glyph->bitmap.rows,
            GL_RED,
            GL_UNSIGNED_BYTE,
            fontData.fontFace->glyph->bitmap.buffer);
        
        // store texture in Character map
        Character character = {
            texture,
            glm::ivec2(fontData.fontFace->glyph->bitmap.width, fontData.fontFace->glyph->bitmap.rows),
            glm::ivec2(fontData.fontFace->glyph->bitmap_left, fontData.fontFace->glyph->bitmap_top),
            static_cast<GLuint>(fontData.fontFace->glyph->advance.x)
        };
        fontData.characters.insert(std::pair<char, Character>(c, character));
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
*/

//to test
bool FontManager::CheckVariantName(const std::string& ftFamily, const std::string& ftVariant) {

    using fontCollectionType = std::unordered_multimap<std::string, FontEntry>;

    std::pair <fontCollectionType::iterator, fontCollectionType::iterator> variantRange = fontCollection.equal_range(ftFamily);

    for (fontCollectionType::iterator it = variantRange.first; it != variantRange.second; it++) {
        if (it->second.fontVariant == ftVariant) {
            return true;
        }
    }
    return false;
}


/*!
* \brief Font getter
*
* Retrieves Font struct object to access character map stored.
*
*/
Font FontManager::GetFont(const std::string& ftFamily, const std::string& ftVariant) {

    using fontCollectionType = std::unordered_multimap<std::string, FontEntry>;
    //DEBUG_PRINT("DEBUG::FONT: RECEIVED family: %s", ftFamily.c_str());
    //DEBUG_PRINT("DEBUG::FONT: RECEIVED variant: %s", ftVariant.c_str());

    std::pair <fontCollectionType::iterator, fontCollectionType::iterator> variantRange = fontCollection.equal_range(ftFamily);

    for (fontCollectionType::iterator it = variantRange.first; it != variantRange.second; it++) {
        //DEBUG_PRINT("DEBUG::FONT: Reading family: %s", it->first.c_str());
        //DEBUG_PRINT("DEBUG::FONT: Reading variant: %s", it->second.fontVariant.c_str());
        if (it->second.fontVariant == ftVariant) {
            return it->second.font;
        }
    }
    DEBUG_PRINT("ERROR::FONT: Failed to retrieve requested font variant %s", ftVariant.c_str());
    return Font{};
}

std::unordered_multimap<std::string, FontEntry>* FontManager::GetFontCollection() {
    return &fontCollection;
}