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
class Character {
public:
	Texture*	textureID;  // Font texture
	size_t		texPos;		// Position of glyph in font sprite sheet
	glm::ivec2  size;       // Size of glyph
	glm::ivec2	bearing;    // Offset from baseline to left/top of glyph
	size_t		advance;    // Offset to advance to next glyph
};

// font object - stores loaded FontFace and the FontFace's extracted glyphs
class Font {
public:
	FT_Face fontFace{};
	std::unordered_map<char, Character> characters{};
	int largestNegativeOffset{};

	Font(const std::string& ftFamily, const std::string& ftVariant)
		: fontFamily(ftFamily), fontVariant(ftVariant) {}

	std::pair<std::string, std::string> GetInfo();

private:
	const std::string& fontFamily;
	const std::string& fontVariant;
};

class FontEntry {
public:
	Font font;
	std::string fontFilePath{};
	std::string filename{};

	FontEntry(const Font& ftData, const std::string& ftFilePath, const std::string& ftFilename)
		: font(ftData), fontFilePath(ftFilePath), filename(ftFilename) {}
};

class FontManager {
public:
	FontManager() : fontLibrary{}, defaultFont{} {};
	~FontManager();
	void Initialize();

	// called during Initialize()
	void ReadFonts();

	// called during Intialize / for on-demand loading
	void LoadFontVariant(const std::string& ftFamily, const std::string& ftVariant);
	void LoadFontEntry(FontEntry& fontEntryData);
	void LoadFontFilePath(const std::string ftFilePath);
	void LoadValidFont(Font& fontData, const std::string& fontFilePath);
	
	// DEPRECATED //
	//void LoadDefaultFont(); -- merged into SetDefaultFont
	//void LoadChar(Font& fontData);  -- moved to TextureManager
	//void LoadAllFonts(); -- decomposed into its smaller functions
	//bool LoadNewFont(Font& fontData, const std::string& fontPath); -- dropped, to use LoadFontFilePath()

	// boolean checkers to prevent duplicates (current usage in Initialize)
	bool CheckFamilyName(const std::string& ftFamily);
	bool CheckVariantName(const std::string& ftFamily, const std::string& ftVariant);

	/**************************
	********* GETTERS *********
	**************************/
	Font* GetFont(const std::string& ftFamily, const std::string& ftVariant);
	Font* GetDefaultFont();

	FontEntry* GetFontEntryByVariant(const std::string& ftFamily, const std::string& ftVariant);
	FontEntry* GetFontEntryByFilePath(const std::string& ftFilePath);

	// primary key: font family string
	std::unordered_multimap<std::string, FontEntry>* GetFontCollection();

	// getter for retrieving fontPairs
	const std::vector<std::pair<std::unique_ptr<std::string>, std::unique_ptr<std::string>>>* GetFontPairs() const;

	// getter for retrieving list of font families
	std::vector<std::string> GetFontFamilyList();

	// getter for retrieving list of variants for set font family
	std::vector<std::string> GetFontVariantList(const std::string& ftFamily);

	/**************************
	********* SETTERS *********
	**************************/
	void SetDefaultFont();
private:
	FT_Library fontLibrary;
	Font* defaultFont;

	std::unordered_multimap<std::string, FontEntry> fontCollection;
	std::vector<std::pair<std::unique_ptr<std::string>, std::unique_ptr<std::string>>> fontPairs;
};

extern FontManager fonts;
extern GLuint font_vaoid;
extern GLuint font_vboid;