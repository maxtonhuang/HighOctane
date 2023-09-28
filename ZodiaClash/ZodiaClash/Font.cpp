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
*	@email		p.foong\@digipen.edu
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
*
******************************************************************************/

#include "Font.h"
#include <iostream>

FontManager fonts;

GLuint font_vaoid;
GLuint font_vboid;

//FontManager::FontManager() {
//    FT_Error err;
//    err = FT_Init_FreeType(&fonts.fontLibrary);
//    ASSERT(err, "Failed to initialise FreeType!");
//
//    fonts.Initialize();
//}

FontManager::~FontManager() {
    //FT_Done_Face(fontCollection["Danto Lite Normal.ttf"].fontFace);
    //for (auto& fontPair : fontCollection) {
    //    //FT_Done_Face(fontPair.second.fontFace);
    //}
    FT_Done_Face(fonts.ft_font.fontFace);
	FT_Done_FreeType(fonts.fontLibrary);
}

void FontManager::Initialize() {
    FT_Error err;
    err = FT_Init_FreeType(&fonts.fontLibrary);
    ASSERT(err, "Failed to initialise FreeType!");
    fonts.LoadFont("Danto Lite Normal.ttf");
}

void FontManager::LoadFont(const std::string& fontPath) {
	//Load font face
	FT_Error err;
    std::string fontFilePath = "../Assets/Fonts/" + fontPath;
	err = FT_New_Face(fonts.fontLibrary, fontFilePath.c_str(), 0, &fonts.ft_font.fontFace);
	if (err == FT_Err_Unknown_File_Format) {
		DEBUG_PRINT("ERROR::FONT: Able to open and read font, but font format is unsupported");
	}
	else if (err) {
        DEBUG_PRINT("ERROR::FONT: Font file cannot be open or read!");
	}
    else {
        // Define pixel font size
        // set pixel height to 100, so relFontSize can be 0.XXf to 1.0f
        FT_Set_Pixel_Sizes(fonts.ft_font.fontFace, 0, 100);
        LoadChar(fonts.ft_font);

        // with textures loaded, add font object to fontCollection
        //fontCollection.emplace(fontPath, font);
    }	
}

void FontManager::LoadChar(Font& font) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    //for all printable characters [32, 127], 169 (copyright) and 174 (registered copyright)
    for (unsigned char c = 32; c < 175; c++)
    {
        //skip over characters not in intended range
        if (((c > 127) && (c < 169)) || ((c > 169) && (c < 174))) {
            continue;
        }

        // load character glyph
        if (FT_Load_Char(font.fontFace, c, FT_LOAD_RENDER))
        {
            DEBUG_PRINT("ERROR::FONT: Failed to load Glyph");
            continue;
        }
        // generate texture
        unsigned int texture;
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureStorage2D(texture, 1, GL_R8, font.fontFace->glyph->bitmap.width,
            font.fontFace->glyph->bitmap.rows);
        glTextureSubImage2D(texture, 0, 0, 0, font.fontFace->glyph->bitmap.width,
            font.fontFace->glyph->bitmap.rows,
            GL_RED,
            GL_UNSIGNED_BYTE,
            font.fontFace->glyph->bitmap.buffer);
           
        Character character = {
            texture,
            glm::ivec2(font.fontFace->glyph->bitmap.width, font.fontFace->glyph->bitmap.rows),
            glm::ivec2(font.fontFace->glyph->bitmap_left, font.fontFace->glyph->bitmap_top),
            static_cast<GLuint>(font.fontFace->glyph->advance.x)
        };
        font.Characters.insert(std::pair<char, Character>(c, character));
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

Font FontManager::GetFont() {
    //auto it = fontCollection.find(fontPath);

    //if (it != fontCollection.end()) {
    //    // The font path was found in the map
    //    Font& fontData = it->second;
    //    return fontData;
    //}
    return fonts.ft_font;
}