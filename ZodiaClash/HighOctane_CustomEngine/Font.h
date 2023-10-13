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
*	@file		Font.h
*
*	@author		Chua Zhen Rong (FreeType font library setup)
*
*	@email		c.zhenrong\@digipen.edu
*
*	@co-author	Foong Pun Yuen Nigel (OpenGL texture creation from fonts)
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
*   This file contains all the declarations needed for integrating the
	font library into the game engine.
*
******************************************************************************/


#pragma once
#include "ECS.h"
#include "GraphLib.h"
#include <filesystem>
#include <string>
#include <unordered_map>
#include "texture.h"

// character object - stores data of extracted glyph
struct Character {
	Texture*	textureID;  // Font texture
	size_t		texPos;		// Position of glyph in font sprite sheet
	glm::ivec2  size;       // Size of glyph
	glm::ivec2	bearing;    // Offset from baseline to left/top of glyph
	size_t		advance;    // Offset to advance to next glyph
};

// font object - stores loaded FontFace and the FontFace's extracted glyphs
struct Font {
	FT_Face fontFace{};
	std::unordered_map<char, Character> characters{};
	bool isActive{};
};

//struct FontInfo {
//	std::string fontFamily{};
//	std::string fontVariant{};
//
//	FontInfo(const std::string& ft_family, const std::string& ft_variant)
//		: fontFamily(ft_family), fontVariant(ft_variant) {}
//};

struct FontEntry {
	Font font;
	std::string fontVariant{};
	std::string filename{};

	FontEntry(const Font& ftData, const std::string& ftVariant, const std::string& ftFilename)
		: font(ftData), fontVariant(ftVariant), filename(ftFilename) {}
};

class FontManager {
public:
	FontManager() : fontLibrary{} {};
	~FontManager();
	void Initialize();

	void LoadAllFonts();
	void LoadValidFont(Font& fontData, const std::string& fontFilePath);
	//void LoadChar(Font& fontData); DEPRECATED

	//tmp, to replace once fully integrated
	// for asset manager? new font file
	void LoadNewFont(Font& fontData, const std::string& fontPath);

	bool CheckVariantName(const std::string& ftFamily, const std::string& ftVariant);

	Font GetFont(const std::string& ftFamily, const std::string& ftVariant);

	// primary key: font family string
	std::unordered_multimap<std::string, FontEntry>* GetFontCollection();
private:
	FT_Library fontLibrary;
	//Font ft_font; //tmp

	std::unordered_multimap<std::string, FontEntry> fontCollection;
};

extern FontManager fonts;
extern GLuint font_vaoid;
extern GLuint font_vboid;