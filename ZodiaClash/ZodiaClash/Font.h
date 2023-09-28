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
	FontManager();
	~FontManager();
	void Initialize();
	void Update();

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