#include "Font.h"
#include <iostream>

FontManager fonts;

FontManager::FontManager() {
	
}

FontManager::~FontManager() {
    for (auto& fontPair : fontCollection) {
        FT_Done_Face(fontPair.second.fontFace);
    }
	FT_Done_FreeType(fontLibrary);
}

void FontManager::Initialize() {
    FT_Error err;
    err = FT_Init_FreeType(&fontLibrary);
    Assert(err, "Failed to initialise FreeType!");
}

void FontManager::Update() {

}

void FontManager::LoadFont(const std::string& fontPath) {
	//Load font face
    Font font;
	FT_Error err;
    std::string fontFilePath = "../Assets/Fonts/" + fontPath;
	err = FT_New_Face(fontLibrary, fontFilePath.c_str(), 0, &font.fontFace);
	if (err == FT_Err_Unknown_File_Format) {
		DebugPrint("ERROR::FONT: Able to open and read font, but font format is unsupported");
	}
	else if (err) {
		DebugPrint("ERROR::FONT: Font file cannot be open or read!");
	}
    else {
        // Define pixel font size
        // set pixel height to 100, so scale can be 0.XXf to 1.0f
        FT_Set_Pixel_Sizes(font.fontFace, 0, 100);
        LoadChar(font);

        // with textures loaded, add font object to fontCollection
        fontCollection.emplace(fontPath, font);
    }	
}

void FontManager::LoadChar(Font& font) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    //for all 128 chars of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(font.fontFace, c, FT_LOAD_RENDER))
        {
            DebugPrint("ERROR::FONT: Failed to load Glyph");
            continue;
        }
        // generate texture
        unsigned int texture;
        //std::cout << font.fontFace->glyph->bitmap.buffer << "\n";
        /*
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            font.fontFace->glyph->bitmap.width,
            font.fontFace->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            font.fontFace->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        */
        // now store character for later use
        glCreateTextures(GL_TEXTURE_2D, 1, &texture);
        glTextureStorage2D(texture, 1, GL_RED, font.fontFace->glyph->bitmap.width,
            font.fontFace->glyph->bitmap.rows);
        glTextureSubImage2D(texture, 0, 0, 0, font.fontFace->glyph->bitmap.width,
            font.fontFace->glyph->bitmap.rows,
            GL_RED,
            GL_UNSIGNED_BYTE,
            font.fontFace->glyph->bitmap.buffer);
        std::cout << c << " " << texture << "\n";
        Character character = {
            texture,
            glm::ivec2(font.fontFace->glyph->bitmap.width, font.fontFace->glyph->bitmap.rows),
            glm::ivec2(font.fontFace->glyph->bitmap_left, font.fontFace->glyph->bitmap_top),
            font.fontFace->glyph->advance.x
        };
        font.Characters.insert(std::pair<char, Character>(c, character));
    }
}

Font FontManager::GetFont(std::string fontPath) {
    auto it = fontCollection.find(fontPath);

    if (it != fontCollection.end()) {
        // The font filepath was found in the map
        Font& fontData = it->second;
        return fontData;
    }
}