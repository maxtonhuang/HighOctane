#include "Graphics.h"
#include "Input.h"
#include "graphlib.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>

Model test_model;
Texture test_tex;
GraphicsManager graphics;

/*                                                   objects with file scope
----------------------------------------------------------------------------- */
//GLFWwindow* GraphicsManager::window;
//int GraphicsManager::width;
//int GraphicsManager::height;
//Shader GraphicsManager::shaderprogram;
//GraphicsManager::VAOInfo GraphicsManager::vao;

GraphicsManager::GraphicsManager() {
    width = 0;
    height = 0;
    vao = VAOInfo{};
    lineloopvao = VAOInfo{};
    window = nullptr;
    textureshaderprogram = Shader{};
    flatshaderprogram = Shader{};
}

GraphicsManager::~GraphicsManager() {
    textureshaderprogram.DeleteShader();
    flatshaderprogram.DeleteShader();
    glfwTerminate();
}

void GraphicsManager::Initialize(int w, int h) {
    //TEMPORARY INITIALISATION, TO BE READ FROM FILE
    width = w;
    height = h;

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
    glClearColor(1.f, 0.f, 0.f, 1.f);

    //Create viewport
    glViewport(0, 0, width, height);

    //Initialise glew for glew functions
    glewInit();

    //Compile shaders
    std::vector<std::pair<GLenum, std::string>> textureshadervector{
        std::make_pair(GL_VERTEX_SHADER, "../Assets/Shaders/TextureVertexShader.vert"),
        std::make_pair(GL_FRAGMENT_SHADER, "../Assets/Shaders/TextureFragmentShader.frag")
    };

    if (textureshaderprogram.Compile(textureshadervector) == false) {
        std::cout << "Unable to compile shader program! Exiting...\n";
        std::exit(EXIT_FAILURE);
    }

    std::vector<std::pair<GLenum, std::string>> flatshadervector{
        std::make_pair(GL_VERTEX_SHADER, "../Assets/Shaders/FlatVertexShader.vert"),
        std::make_pair(GL_FRAGMENT_SHADER, "../Assets/Shaders/FlatFragmentShader.frag")
    };

    if (flatshaderprogram.Compile(flatshadervector) == false) {
        std::cout << "Unable to compile shader program! Exiting...\n";
        std::exit(EXIT_FAILURE);
    }

    textureshaderprogram.Use();

    //Create square VAO for use in drawing
    {
        std::vector<Vertex> vtx_array{};
        std::vector<GLushort> idx_vtx{};

        vtx_array.push_back(Vertex{ glm::vec2(-1,-1),glm::vec3(1,1,1),glm::vec2(0,1) });
        vtx_array.push_back(Vertex{ glm::vec2(-1,1),glm::vec3(1,1,1),glm::vec2(0,0) });
        vtx_array.push_back(Vertex{ glm::vec2(1,-1),glm::vec3(1,1,1),glm::vec2(1,1) });
        vtx_array.push_back(Vertex{ glm::vec2(1,1),glm::vec3(1,1,1),glm::vec2(1,0) });

        //Buffer for vertex order
        idx_vtx.push_back(3);
        idx_vtx.push_back(1);
        idx_vtx.push_back(2);
        idx_vtx.push_back(0);

        vao.primitivetype = GL_TRIANGLE_STRIP;
        CreateVAO(vao,vtx_array,idx_vtx);
    }

    //Create line VAO for use in drawing lines
    {
        std::vector<Vertex> vtx_array{};
        std::vector<GLushort> idx_vtx{};

        vtx_array.push_back(Vertex{ glm::vec2(-1,-1),glm::vec3(1,1,1),glm::vec2(0,1) });
        vtx_array.push_back(Vertex{ glm::vec2(-1,1),glm::vec3(1,1,1),glm::vec2(0,0) });
        vtx_array.push_back(Vertex{ glm::vec2(1,-1),glm::vec3(1,1,1),glm::vec2(1,1) });
        vtx_array.push_back(Vertex{ glm::vec2(1,1),glm::vec3(1,1,1),glm::vec2(1,0) });

        //Buffer for vertex order
        idx_vtx.push_back(0);
        idx_vtx.push_back(1);
        idx_vtx.push_back(3);
        idx_vtx.push_back(2);

        lineloopvao.primitivetype = GL_LINE_LOOP;
        CreateVAO( lineloopvao, vtx_array, idx_vtx);
    }

    //Enable alpha
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    texList.AddSpriteSheet("duck.png", 1, 6, 6);
    test_model.AttachTexture("duck.png");

    //TEMP
    std::default_random_engine rng;
    std::uniform_real_distribution<float> rand_width(0, 2 * width);
    std::uniform_real_distribution<float> rand_height(0, 2 * height);
    for (int i = 0; i < 5; i++) {
        Model mdl;
        mdl.AttachTexture("cat.png");
        mdl.SetPos(rand_width(rng) - width, rand_height(rng) - height);
        modelList.emplace_back(mdl);
    }

    Draw();
}

void GraphicsManager::Update(float dt) {
    static float fpsInterval = 1.f;
    static int count = 0;
    fpsInterval += dt;
    ++count;
    if (fpsInterval > 1) {
        std::stringstream title;
        title << "ZodiaClash " << count;
        glfwSetWindowTitle(window, title.str().c_str());
        fpsInterval -= 1;
        count = 0;
    }
}

void GraphicsManager::Draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (Model& m : modelList) {
        m.Update();
        m.Draw();
    }

    test_model.Update();
    test_model.Draw();
    test_model.DrawOutline();

    glfwSwapBuffers(window);
}

void GraphicsManager::CreateVAO(VAOInfo& vao, std::vector<Vertex>& vtx_array, std::vector<GLushort>& idx_vtx) {
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
    vao.drawcnt = vtx_array.size();
}

void GraphicsManager::DrawPoint(float x, float y) {
    glPointSize(10.f);
    flatshaderprogram.Use();
    glBindVertexArray(vao.id);

    glm::mat3 matrix{ 0,0,0,0,0,0, x / (float)width, y / (float)height,1 };

    //Pass matrix to shader
    GLint uniform_var_matrix = glGetUniformLocation(
        flatshaderprogram.GetHandle(), "uModelToNDC");
    if (uniform_var_matrix >= 0) {
        glUniformMatrix3fv(uniform_var_matrix, 1, GL_FALSE, glm::value_ptr(matrix));
    }
    else {
        std::cout << "Uniform variable uModelToNDC doesn't exist!\n";
        std::cout << "Please check vertex shader!\n";
        std::exit(EXIT_FAILURE);
    }

    glDrawElements(GL_POINTS, vao.drawcnt, GL_UNSIGNED_SHORT, NULL);
    textureshaderprogram.Use();
}

void GraphicsManager::DrawLineLoop(const glm::mat3& input) {
    flatshaderprogram.Use();
    glBindVertexArray(lineloopvao.id);

    //Pass matrix to shader
    GLint uniform_var_matrix = glGetUniformLocation(
        flatshaderprogram.GetHandle(), "uModelToNDC");
    if (uniform_var_matrix >= 0) {
        glUniformMatrix3fv(uniform_var_matrix, 1, GL_FALSE, glm::value_ptr(input));
    }
    else {
        std::cout << "Uniform variable uModelToNDC doesn't exist!\n";
        std::cout << "Please check vertex shader!\n";
        std::exit(EXIT_FAILURE);
    }

    glDrawElements(lineloopvao.primitivetype, lineloopvao.drawcnt, GL_UNSIGNED_SHORT, NULL);
    textureshaderprogram.Use();
}

std::string GraphicsManager::GetName() {
    return "Graphics";
}

const GraphicsManager::VAOInfo& GraphicsManager::GetVAOInfo() {
    return vao;
}

const Shader& GraphicsManager::GetShader() {
    return textureshaderprogram;
}

bool GraphicsManager::WindowClosed() {
    return glfwWindowShouldClose(window);
}

float GraphicsManager::GetWidth() {
    return (float)width;
}

float GraphicsManager::GetHeight() {
    return (float)height;
}
