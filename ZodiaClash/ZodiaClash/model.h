#pragma once

#include <glew-2.2.0/include/GL/glew.h>
#include <glfw-3.3.8.bin.WIN64/include/GLFW/glfw3.h>
#include <glm-0.9.9.8/glm/glm.hpp>
#include "graphics.h"
#include "shaders.h"

class Model {
public:
	Model();
	void Update();
	void Draw();
private:
	glm::mat3 matrix{};
	glm::vec3 color{};
};