#pragma once
#include <glew-2.2.0/include/GL/glew.h>
#include <string>
#include <vector>

class Shader {
public:
	bool Compile(std::vector<std::pair<GLenum, std::string>> vec);
	void DeleteShader();
	void Use();
	void UnUse();
	GLuint GetHandle() const;
private:
	GLuint handle;
};
