#include "graphics.h"
#include "input.h"
#include <iostream>

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
GLFWwindow* GraphicsManager::window;
int GraphicsManager::width;
int GraphicsManager::height;
Shader GraphicsManager::shaderprogram;
GraphicsManager::VAOInfo GraphicsManager::vao;

Model test;

void GraphicsManager::Init() {
    //TEMPORARY INITIALISATION, TO BE READ FROM FILE
    width = 1024;
    height = 768;

    glfwInit();

    //Set GLFW settings
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_RED_BITS, 8); glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8); glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    //Create window
    window = glfwCreateWindow(width, height, "ZodiaClash", NULL, NULL);

    glfwMakeContextCurrent(window);

    //Set functions for input manager
    glfwSetKeyCallback(window, InputManager::KeyCallback);
    glfwSetMouseButtonCallback(window, InputManager::MouseButtonCallback);
    glfwSetCursorPosCallback(window, InputManager::CursorPosCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    //Set default background colour
    glClearColor(0.f, 1.f, 0.f, 1.f);

    //Create viewport
    glViewport(0, 0, width, height);

    //Initialise glew for glew functions
    glewInit();

    //Compile shaders
    std::vector<std::pair<GLenum, std::string>> shadervector{
        std::make_pair(GL_VERTEX_SHADER, "../Assets/Shaders/vertexshader.vert"),
        std::make_pair(GL_FRAGMENT_SHADER, "../Assets/Shaders/fragmentshader.frag")
    };

    if (shaderprogram.Compile(shadervector) == false) {
        std::cout << "Unable to compile shader program! Exiting...\n";
        std::exit(EXIT_FAILURE);
    }

    shaderprogram.Use();

    //Create square VAO for use in drawing
    CreateVAO();

    //TEMP
    Draw();
}

void GraphicsManager::Cleanup() {
    shaderprogram.DeleteShader();
    glfwTerminate();
}

void GraphicsManager::Draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    test.Draw();

    glfwSwapBuffers(window);
}

void GraphicsManager::CreateVAO() {
    std::vector<Vertex> vtx_array{};
    std::vector<GLushort> idx_vtx{};

    vtx_array.push_back(Vertex{ glm::vec2(-1,-1),glm::vec3(1,1,1),glm::vec2(0,0) });
    vtx_array.push_back(Vertex{ glm::vec2(-1,1),glm::vec3(1,1,1),glm::vec2(0,1) });
    vtx_array.push_back(Vertex{ glm::vec2(1,-1),glm::vec3(1,1,1),glm::vec2(1,0) });
    vtx_array.push_back(Vertex{ glm::vec2(1,1),glm::vec3(1,1,1),glm::vec2(1,1) });

    //Buffer for vertex order
    idx_vtx.push_back(3);
    idx_vtx.push_back(1);
    idx_vtx.push_back(2);
    idx_vtx.push_back(0);

    //Create buffer vertex
    GLuint vbo_hdl;
    glCreateBuffers(1, &vbo_hdl);
    glNamedBufferStorage(vbo_hdl, sizeof(Vertex) * vtx_array.size(),
        vtx_array.data(), GL_DYNAMIC_STORAGE_BIT);

    GLuint vaoid;
    //Assign vertex positions to shader
    glCreateVertexArrays(1, &vaoid);
    glEnableVertexArrayAttrib(vaoid, 0);
    glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, 0, sizeof(Vertex));
    glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vaoid, 0, 0);

    //Assign colour positions to shader
    glEnableVertexArrayAttrib(vaoid, 1);
    glVertexArrayVertexBuffer(vaoid, 1, vbo_hdl, sizeof(Vertex::pos), sizeof(Vertex));
    glVertexArrayAttribFormat(vaoid, 1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vaoid, 1, 1);

    //Assign texture positions to shader
    glEnableVertexArrayAttrib(vaoid, 2);
    glVertexArrayVertexBuffer(vaoid, 2, vbo_hdl, sizeof(Vertex::pos) + sizeof(Vertex::col), sizeof(Vertex));
    glVertexArrayAttribFormat(vaoid, 2, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vaoid, 2, 2);

    GLuint ebo_hdl;
    glCreateBuffers(1, &ebo_hdl);
    glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * idx_vtx.size(),
        reinterpret_cast<GLvoid*>(idx_vtx.data()),
        GL_DYNAMIC_STORAGE_BIT);
    glVertexArrayElementBuffer(vaoid, ebo_hdl);

    glBindVertexArray(0);

    vao.id = vaoid;
    vao.primitivetype = GL_TRIANGLE_STRIP;
    vao.drawcnt = vtx_array.size();
}

const GraphicsManager::VAOInfo& GraphicsManager::GetVAOInfo() {
    return vao;
}

const Shader& GraphicsManager::GetShader() {
    return shaderprogram;
}

bool GraphicsManager::WindowClosed() {
    return glfwWindowShouldClose(window);
}
