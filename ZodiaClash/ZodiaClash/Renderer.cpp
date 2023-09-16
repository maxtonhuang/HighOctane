#include "Renderer.h"
#include "debugdiagnostic.h"

Renderer::Renderer() {
    data = new Vertex[GRAPHICS::vertexBufferSize];
}

Renderer::~Renderer() {
    delete[] data;
}

void Renderer::Initialize(char const* vertexshader, char const* fragmentshader) {
    //Compile shaders
    bool compile_status;
    std::vector<std::pair<GLenum, std::string>> shadervector{
        std::make_pair(GL_VERTEX_SHADER, vertexshader),
        std::make_pair(GL_FRAGMENT_SHADER, fragmentshader)
    };

    compile_status = shaderprogram.Compile(shadervector);
    Assert(compile_status, "Unable to compile shader program!\n");

    CreateVAO();
}

void Renderer::AddVertex(Vertex input) {
    data[drawcount] = input;
    ++drawcount;
}

void Renderer::Draw(GLenum drawtype) {
    int uTex[GRAPHICS::MAXTEXTURES];
    for (int i = 0; i < GRAPHICS::MAXTEXTURES; ++i) {
        uTex[i] = i;
    }
    glNamedBufferSubData(vbo, 0, sizeof(Vertex) * GRAPHICS::vertexBufferSize, data);
    shaderprogram.Use();
    int count = 0;
    for (auto& t : texList.data) {
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
    glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
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

    glBindVertexArray(0);
}

GLuint Renderer::GetDrawCount() {
    return drawcount;
}
