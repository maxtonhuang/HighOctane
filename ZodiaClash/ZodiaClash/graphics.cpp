#include "GUIManager.h"
#include "Graphics.h"
#include "Input.h"
#include "graphlib.h"
#include "debugdiagnostic.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>

//Model test_model;
Texture test_tex;
GraphicsManager graphics;

Renderer flatRenderer;
Renderer textureRenderer;
Renderer fontRenderer;

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
    window = nullptr;
}

GraphicsManager::~GraphicsManager() {
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

    //Enable alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Initialize renderers
    flatRenderer.Initialize("../Assets/Shaders/FlatVertexShader2.vert", "../Assets/Shaders/FlatFragmentShader2.frag");
    textureRenderer.Initialize("../Assets/Shaders/texture.vert", "../Assets/Shaders/texture.frag");
    fontRenderer.Initialize("../Assets/Shaders/font.vert", "../Assets/Shaders/font.frag");

    texList.AddSpriteSheet("duck.png", 1, 6, 6);
    texList.AddSpriteSheet("duck2.png", 1, 6, 6);
    //test_model.AttachTexture("duck.png");

    fonts.Initialize();
    fonts.LoadFont("Danto Lite Normal.ttf");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(GRAPHICS::defaultWidth), 0.0f, static_cast<float>(GRAPHICS::defaultHeight));
    fontRenderer.shaderprogram.Use();
    std::cout << fontRenderer.shaderprogram.GetHandle() << "\n";
    std::cout << glGetUniformLocation(fontRenderer.shaderprogram.GetHandle(), "projection") << "\n";
    std::cout << glGetUniformLocation(fontRenderer.shaderprogram.GetHandle(), "textColor") << "\n";
    //glUniformMatrix4fv(glGetUniformLocation(fontRenderer.shaderprogram.GetHandle(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    guiManager.Init(window);

    //TEMP
    std::default_random_engine rng;
    std::uniform_real_distribution<float> rand_width(0, width);
    std::uniform_real_distribution<float> rand_height(0, height);
    for (int i = 0; i < 0; i++) {
        Model mdl;
        mdl.AttachTexture("cat.png");
        mdl.SetPos(rand_width(rng) - width / 2, rand_height(rng) - height / 2);
        mdl.SetScale(0.1, 0.1);
        modelList.emplace_back(mdl);
    }

    //DEBUG GRAPHIC MODELS FOR COLLISION TESTING
    test_circle1.AttachTexture("circle.png");
    test_circle2.AttachTexture("circle.png");
    test_circle3.AttachTexture("circle.png");

    test_circle1.SetDim(100, 100);
    test_circle1.SetPos(100, 100);

    test_circle2.SetDim(100, 100);
    test_circle2.SetPos(200, 200);

    test_circle3.SetDim(100, 100);
    test_circle3.SetPos(300, 300);

    test_rect1.SetDim(100, 100);
    test_rect1.SetPos(-100, -100);

    test_rect2.SetDim(200, 100);
    test_rect2.SetPos(-200, -200);

    test_rect3.SetDim(100, 200, 50);
    test_rect3.SetPos(-300, -300);

    glfwSwapInterval(0);
    //Draw();
}

void GraphicsManager::Update() {
    static float fpsInterval = 1.f;
    static int count = 0;
    fpsInterval += g_dt;
    ++count;
    if (fpsInterval > 1) {
        std::stringstream title;
        title << "ZodiaClash " << count;
        //glfwSetWindowTitle(window, title.str().c_str());
        fpsInterval -= 1;
        count = 0;
    }
}

void GraphicsManager::Draw() { // ------------- Can this go into ECS? ------------------------------//
    for (Model& m : modelList) {
        m.Update();
        m.Draw();
    }

    test_circle1.Update();
    test_circle2.Update();
    test_circle3.Update();
    test_circle1.Draw();
    test_circle2.Draw();
    test_circle3.Draw();
    test_rect1.Update();
    test_rect2.Update();
    test_rect3.Update();
    test_rect1.Draw();
    test_rect2.Draw();
    test_rect3.Draw();

    /*test_model.Update();
    test_model.Draw();*/

    // TODO: do drawing of text here?

    textureRenderer.Draw();
    fontRenderer.Draw();
    //test_model.DrawOutline();
    
    DrawLabel("AAA", "Danto Lite Normal.ttf", 0.5f, Transform{ Vec2{ 0.f,0.f }, 0.f, Vec2{ 1.f, 1.f }, vmath::Vector2{ 0,0 } }, Color{ glm::vec4{ 1,1,1,1 } });

    DrawCircle(100, -100, 20);
    //DrawRect(0, 0, 50, 50);
    //DrawOutline(-50, -50, 0, 0);

    guiManager.Update(window);
    glfwSwapBuffers(window);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GraphicsManager::DrawPoint(float x, float y, float pointsize) {
    flatRenderer.Draw();
    glPointSize(pointsize);
    flatRenderer.AddVertex(Vertex{ glm::vec2{x / GRAPHICS::w, y / GRAPHICS::h}, glm::vec3{1,1,1} });
    flatRenderer.Draw(GL_POINTS);
}

void GraphicsManager::DrawLine(float x1, float y1, float x2, float y2) {
    flatRenderer.Draw();
    flatRenderer.AddVertex(Vertex{ glm::vec2{x1 / GRAPHICS::w, y1 / GRAPHICS::h}, glm::vec3{1,1,1} });
    flatRenderer.AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w, y2 / GRAPHICS::h}, glm::vec3{1,1,1} });
    flatRenderer.Draw(GL_LINES);
}

void GraphicsManager::DrawCircle(float x, float y, float radius) {
    const float PI = 3.141592653589793238463;
    const float angle = 2.f * PI / (float)GRAPHICS::CIRCLE_SLICES;
    flatRenderer.Draw();
    flatRenderer.AddVertex(Vertex{ glm::vec2{x / GRAPHICS::w, y / GRAPHICS::h}, glm::vec3{1,1,1} });
    for (int i = 0; i <= GRAPHICS::CIRCLE_SLICES; ++i) {
        flatRenderer.AddVertex(Vertex{ glm::vec2{(x + radius * std::cos(angle * i)) / GRAPHICS::w, (y + radius * std::sin(angle * i)) / GRAPHICS::h}, glm::vec3{1,1,1}});
    }
    flatRenderer.Draw(GL_TRIANGLE_FAN);
}

void GraphicsManager::DrawRect(float x1, float y1, float x2, float y2) {
    flatRenderer.Draw();
    flatRenderer.AddVertex(Vertex{ glm::vec2{x1 / GRAPHICS::w,y1 / GRAPHICS::h}, glm::vec3{1,1,1} });
    flatRenderer.AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w,y1 / GRAPHICS::h}, glm::vec3{1,1,1} });
    flatRenderer.AddVertex(Vertex{ glm::vec2{x1 / GRAPHICS::w,y2 / GRAPHICS::h}, glm::vec3{1,1,1} });
    flatRenderer.AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w,y2 / GRAPHICS::h}, glm::vec3{1,1,1} });
    flatRenderer.Draw(GL_TRIANGLE_STRIP);
}

void GraphicsManager::DrawOutline(float x1, float y1, float x2, float y2) {
    flatRenderer.Draw();
    flatRenderer.AddVertex(Vertex{ glm::vec2{x1 / GRAPHICS::w,y1 / GRAPHICS::h}, glm::vec3{1,1,1}});
    flatRenderer.AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w,y1 / GRAPHICS::h}, glm::vec3{1,1,1} });
    flatRenderer.AddVertex(Vertex{ glm::vec2{x2 / GRAPHICS::w,y2 / GRAPHICS::h}, glm::vec3{1,1,1} });
    flatRenderer.AddVertex(Vertex{ glm::vec2{x1 / GRAPHICS::w,y2 / GRAPHICS::h}, glm::vec3{1,1,1} });
    flatRenderer.Draw(GL_LINE_LOOP);
}

std::string GraphicsManager::GetName() {
    return "Graphics";
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

void GraphicsManager::DrawLabel(std::string labelText, std::string fontName, float scale, Transform transData, Color colorData) {
    
    Font fontData = fonts.GetFont(fontName);
    // TODO some sort of non null checking for fontData?
    // iterate through all characters 
    std::string::const_iterator c;
    for (c = labelText.begin(); c != labelText.end(); c++)
    {
        Character ch = fontData.Characters[*c];

        float xpos = (transData.position.x + ch.Bearing.x * scale);
        float ypos = (transData.position.y - (ch.Size.y - ch.Bearing.y) * scale);

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        glm::vec3 color(1.f, 1.f, 1.f);
        glm::vec2 botleft{ xpos / GRAPHICS::w, ypos / GRAPHICS::h };
        glm::vec2 botright{ (xpos + w) / GRAPHICS::w, ypos / GRAPHICS::h };
        glm::vec2 topright{ (xpos + w) / GRAPHICS::w, (ypos + h) / GRAPHICS::h };
        glm::vec2 topleft{ (xpos) / GRAPHICS::w, (ypos + h) / GRAPHICS::h };
        fontRenderer.AddVertex(Vertex{ botleft,color, glm::vec2{0,1}, (float)ch.TextureID - 1});
        fontRenderer.AddVertex(Vertex{ botright,color, glm::vec2{1,1}, (float)ch.TextureID - 1 });
        fontRenderer.AddVertex(Vertex{ topleft,color, glm::vec2{0,0}, (float)ch.TextureID - 1 });
        fontRenderer.AddVertex(Vertex{ topright,color, glm::vec2{1,0}, (float)ch.TextureID - 1 });
        fontRenderer.AddVertex(Vertex{ botright,color, glm::vec2{1,1}, (float)ch.TextureID - 1 });
        fontRenderer.AddVertex(Vertex{ topleft,color, glm::vec2{0,0}, (float)ch.TextureID - 1 });
        fontRenderer.FontDraw(ch.TextureID);
        transData.position.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
}
