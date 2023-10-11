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

    bool tmpFlag{};

    // iterate through folders in parentDir
    for (const auto& dirItem : fileSys::directory_iterator(parentDir)) {
        // capture folder names in parentDir as font family names
        if (fileSys::is_directory(dirItem)) {
            std::string ft_family = dirItem.path().filename().string();
            DEBUG_PRINT("DEBUG::FONT: >>> Font family: %s", ft_family.c_str());

            // reset flag for defaultVariant
            bool isDefaultSet = 0;

            //reset variables
            std::string filename{}, fileExt{}, ft_variant{};
            size_t foundVarStart{}, foundVarEnd{};

            // iterate through file items in one font parent directory
            for (const auto& fileItem : fileSys::directory_iterator(dirItem.path())) {
                if (fileSys::is_regular_file(fileItem)) {
                    // ignore files that are not font files
                    filename = fileItem.path().filename().string();
                    foundVarStart = filename.find_first_of('-');
                    foundVarEnd = filename.find_last_of('.');
                    fileExt = (foundVarEnd != std::string::npos) ? filename.substr(foundVarEnd) : "";

                    if ((fileExt == ".ttf") || (fileExt == ".otf")) {
                        ft_variant = (foundVarStart != std::string::npos) ? filename.substr(foundVarStart + 1, foundVarEnd - foundVarStart - 1) : "";
                    
                        if (!isDefaultSet) {
                            ft_variant = "Regular";
                            isDefaultSet = 1;
                        }
                        else {
                            // assert here
                            break;
                        }
                    
                    
                        // check if all data is present, then proceed extract data for font file
                        if (!ft_family.empty() && !ft_variant.empty()) {
                            // construct font's filepath
                            std::string fontFilePath = parentDir + ft_family + "/" + filename;
                    
                            // init new font obj
                            Font ft_data;
                            
                            if (tmpFlag == 0) {
                                // NOTE: DO NOT RELEASE OTHER FONT FOLDERS FROM ACHIEVES UNTIL TEXTURES ARE CONVERTED INTO ONE SPRITESHEET! IF TESTING COMMENT THIS OUT!
                                LoadValidFont(ft_data, fontFilePath);
                    
                                // prepare new entry to load into fontCollection
                                ft_data.isActive = 1;
                                FontInfo ft_info(ft_family, ft_variant);
                                FontEntry ft_entry(ft_data, ft_info);
                    
                                // store in fontCollection
                                fontCollection.emplace(filename, ft_entry);
                    
                                tmpFlag = 1;
                            }
                            
                            DEBUG_PRINT("DEBUG::FONT: [+] Added to font collection: %s %s", ft_family.c_str(), ft_variant.c_str());
                        }
                    }
                    else {
                        DEBUG_PRINT("DEBUG::FONT: Skipped over file: %s", filename.c_str());
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
    //Load font face
    FT_Error err;
    err = FT_New_Face(fonts.fontLibrary, fontFilePath.c_str(), 0, &fontData.fontFace);
    if (err == FT_Err_Unknown_File_Format) {
        DEBUG_PRINT("ERROR::FONT: Able to open and read font, but font format is unsupported");
    }
    else if (err) {
        DEBUG_PRINT("ERROR::FONT: Font file cannot be open or read!");
    }
    else {
        // Define pixel font size
        // set pixel height to 100, so relFontSize can be 0.XXf to 1.0f
        FT_Set_Pixel_Sizes(fontData.fontFace, 0, 100);
        LoadChar(fontData);
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
        LoadChar(fontData);

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
void FontManager::LoadChar(Font& fontData) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    //for all printable characters [32, 127], 169 (copyright) and 174 (registered copyright)
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

/*!
* \brief Font getter
*
* Retrieves Font struct object to access character map stored.
*
*/
Font FontManager::GetFont(const std::string& filename) {

    std::unordered_map<std::string, FontEntry>::iterator it = fontCollection.find(filename);

    if (it != fontCollection.end()) {
        // The font path was found in the map
        FontEntry& ft_entry = it->second;
        return ft_entry.font;
    }
    else {
        Font emptyFont;
        DEBUG_PRINT("ERROR::FONT: Failed to retrieve requested font!");
        return emptyFont;
    }
}

std::unordered_map<std::string, FontEntry>* FontManager::GetFontCollection() {
    return &fontCollection;
}