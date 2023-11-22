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
*	@file		Renderer.cpp
*
*	@author		Foong Pun Yuen Nigel
*
*	@email		p.foong\@digipen.edu
*
*	@course		CSD 2401 - Software Engineering Project 3
*				CSD 2451 - Software Engineering Project 4
*
*	@section	Section A
*
*	@date		23 September 2023
*
* *****************************************************************************
*
*	@brief		Renderer class to render for different shaders and draw types
*
*	Renderer to store vertices for each shader and draw type
*	Draw function to be called at the end of each frame to draw all vertices
*	to the screen
*
******************************************************************************/

#include "Renderer.h"
#include "debugdiagnostic.h"
#include "graphics.h"
#include "AssetManager.h"

Renderer::Renderer() {
    data = new Vertex[GRAPHICS::vertexBufferSize];
}

Renderer::~Renderer() {
    delete[] data;
}

void Renderer::Initialize(char const* vertexshader, char const* fragmentshader, GLenum type) {
    //Compile shaders
    bool compile_status;
    std::vector<std::pair<GLenum, std::string>> shadervector{
        std::make_pair(GL_VERTEX_SHADER, vertexshader),
        std::make_pair(GL_FRAGMENT_SHADER, fragmentshader)
    };

    compile_status = shaderprogram.Compile(shadervector);
    ASSERT(!compile_status, "Unable to compile shader program!");

    CreateVAO();
    drawtype = type;
    switch (drawtype) {
    case GL_POINTS:
        objvertsize = 1;
        break;
    case GL_TRIANGLES:
        objvertsize = 6;
        break;
    case GL_LINES:
        objvertsize = 2;
        break;
    case GL_TRIANGLE_FAN:
        objvertsize = 1;
        break;
    default:
        objvertsize = 0;
    }
}

void Renderer::Initialize(Shader shader, GLenum type) {
    shaderprogram = shader;
    CreateVAO();
    drawtype = type;
    switch (drawtype) {
    case GL_POINTS:
        objvertsize = 1;
        break;
    case GL_TRIANGLES:
        objvertsize = 6;
        break;
    case GL_LINES:
        objvertsize = 2;
        break;

    default:
        objvertsize = 0;
    }
}

Shader& Renderer::ShaderProgram() {
    return shaderprogram;
}

void Renderer::AddVertex(Vertex input) {
    if (drawcount + objvertsize >= GRAPHICS::vertexBufferSize) {
        static bool errorprinted = false;
        if (errorprinted == false) {
            DEBUG_PRINT("Max vertex size reached! Consider increasing vertex buffer size to prevent possible graphical errors!");
            errorprinted = true;
        }
        Draw();
    }
    input.bufPos = (float)(drawcount / objvertsize);
    data[drawcount] = input;
    drawcount++;
}

void Renderer::Draw() {
    static std::unordered_map<std::string, Texture>* textureList{ &assetmanager.texture.data };

    if (drawcount <= 0) {
        return;
    }

    graphics.framebuffer.Bind();

    int uTex[GRAPHICS::MAXTEXTURES];
    for (int i = 0; i < GRAPHICS::MAXTEXTURES; ++i) {
        uTex[i] = i;
    }
    glNamedBufferSubData(vbo, 0, sizeof(Vertex) * GRAPHICS::vertexBufferSize, data);
    shaderprogram.Use();
    for (auto& t : *textureList) {
        glBindTextureUnit(t.second.GetID() - 1, t.second.GetID());
    }
    GLint uniform_var_tex = glGetUniformLocation(
        shaderprogram.GetHandle(), "uTex2d");
    if (uniform_var_tex >= 0) {
        glUniform1iv(uniform_var_tex, GRAPHICS::MAXTEXTURES, uTex);
    }
    glBindVertexArray(vao);
    glDrawArrays(drawtype, 0, drawcount);
    drawcount = 0;
    glBindTexture(GL_TEXTURE_2D, 0);

   graphics.framebuffer.Unbind();
}

void Renderer::DrawFrameBuffer() {
    //Create a fullscreen quad to render framebuffer
    AddVertex(Vertex{ glm::vec2{-1,-1}, glm::vec4{1,1,1,1}, glm::vec2{0,0},0 }); //bottom left
    AddVertex(Vertex{ glm::vec2{1,-1}, glm::vec4{1,1,1,1}, glm::vec2{1,0},0 }); //bottom right
    AddVertex(Vertex{ glm::vec2{-1,1}, glm::vec4{1,1,1,1}, glm::vec2{0,1},0 }); //top left
    AddVertex(Vertex{ glm::vec2{1,1}, glm::vec4{1,1,1,1}, glm::vec2{1,1},0 }); //top right

    glNamedBufferSubData(vbo, 0, sizeof(Vertex) * GRAPHICS::vertexBufferSize, data);
    shaderprogram.Use();
    int uTex = 0;
    glBindTextureUnit(uTex, graphics.framebuffer.GetTextureID());
    GLint uniform_var_tex = glGetUniformLocation(
        shaderprogram.GetHandle(), "uTex2d");
    if (uniform_var_tex >= 0) {
        glUniform1iv(uniform_var_tex, GRAPHICS::MAXTEXTURES, &uTex);
    }
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, drawcount);
    drawcount = 0;
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::FontDraw(GLuint texID) {
    if (drawcount <= 0) {
        return;
    }
    glActiveTexture(GL_TEXTURE0);
    glNamedBufferSubData(vbo, 0, sizeof(Vertex) * GRAPHICS::vertexBufferSize, data);
    shaderprogram.Use();
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, texID);
    glDrawArrays(GL_TRIANGLES, 0, drawcount);
    drawcount = 0;
}

void Renderer::UpdateUniform1fv(char const* uniform_name, float* value, int size) {
    shaderprogram.Use();
    GLint uniform_var_matrix = glGetUniformLocation(shaderprogram.GetHandle(), uniform_name);
    if (uniform_var_matrix >= 0) {
        glUniform1fv(uniform_var_matrix, size, value);
    }
}

void Renderer::UpdateUniformMatrix3fv(char const* uniform_name, glm::mat3* matrix) {
    shaderprogram.Use();
    GLint uniform_var_matrix = glGetUniformLocation(shaderprogram.GetHandle(), uniform_name);
    if (uniform_var_matrix >= 0) {
        glUniformMatrix3fv(uniform_var_matrix, 1, GL_FALSE, glm::value_ptr(*matrix));
    }
}

void Renderer::CreateVAO() {
    //Create buffer vertex
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, sizeof(Vertex) * GRAPHICS::vertexBufferSize,
        NULL, GL_DYNAMIC_STORAGE_BIT);

    //Assign vertex positions to shader
    glCreateVertexArrays(1, &vao);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
    glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 0, 0);

    //Assign colour positions to shader
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayVertexBuffer(vao, 1, vbo, sizeof(Vertex::pos), sizeof(Vertex));
    glVertexArrayAttribFormat(vao, 1, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 1, 1);

    //Assign texture positions to shader
    glEnableVertexArrayAttrib(vao, 2);
    glVertexArrayVertexBuffer(vao, 2, vbo, sizeof(Vertex::pos) + sizeof(Vertex::col), sizeof(Vertex));
    glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 2, 2);

    //Assign texture index to shader
    glEnableVertexArrayAttrib(vao, 3);
    glVertexArrayVertexBuffer(vao, 3, vbo, sizeof(Vertex::pos) + sizeof(Vertex::col) + sizeof(Vertex::tex), sizeof(Vertex));
    glVertexArrayAttribFormat(vao, 3, 1, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 3, 3);

    //Assign renderer buffer index to shader
    glEnableVertexArrayAttrib(vao, 4);
    glVertexArrayVertexBuffer(vao, 4, vbo, sizeof(Vertex::pos) + sizeof(Vertex::col) + sizeof(Vertex::tex) + sizeof(Vertex::index), sizeof(Vertex));
    glVertexArrayAttribFormat(vao, 4, 1, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 4, 4);

    glBindVertexArray(0);
}

GLuint Renderer::GetDrawCount() {
    return drawcount;
}

void Renderer::SetName(std::string input) {
    name = input;
}

std::string Renderer::GetName() {
    return name;
}
