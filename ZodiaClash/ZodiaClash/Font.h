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
*   This file contains all the declarations needed for integrating the
	font library into the game engine.
*
******************************************************************************/


#pragma once
#include "ECS.h"
#include "GraphLib.h"
#include <string>
#include<map>


struct Character {
	GLuint		TextureID;  // ID handle of the glyph texture
	glm::ivec2  Size;       // Size of glyph
	glm::ivec2	Bearing;    // Offset from baseline to left/top of glyph
	GLuint		Advance;    // Offset to advance to next glyph
};

struct Font {
	FT_Face fontFace;
	std::unordered_map<char, Character> Characters;
};

class FontManager {
public:
	// to create def init list!
	FontManager() : fontLibrary{}, ft_font{} {};
	~FontManager();
	void Initialize();

	void LoadFont(const std::string& fontPath);
	void LoadChar(Font& font);

	Font GetFont();
private:
	FT_Library fontLibrary;
	Font ft_font;

	//std::unordered_map<std::string, Font> fontCollection;
};

extern FontManager fonts;
extern GLuint font_vaoid;
extern GLuint font_vboid;