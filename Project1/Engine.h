#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "SceneManager.h"
#include "Console.h"
#include "Text.h"

class Engine
{
public:
    static Engine& GetInstance(int width, int height);
    
    bool Init();
    void Run();
    void Shutdown();
    
    GLFWwindow* GetWindow() const { return window; }
    int GetScreenWidth() const { return screenWidth; }
    int GetScreenHeight() const { return screenHeight; }
    
    SceneManager& GetSceneManager() { return sceneManager; }
    ConsoleRenderer& GetConsole() { return console; }
    Text& GetText();
    
    // DODANE: Obsługa niestandardowego kursora
    void SetCustomCursor(const std::string& imagePath, int cursorX, int cursorY, 
                         int width, int height, int hotspotX, int hotspotY);
    void RestoreDefaultCursor();

    void OnResize(int w, int h);
    void Render();

    static void KeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void CharCallback(GLFWwindow* w, unsigned int codepoint);

    void CreateTestCursor();
    void LoadCursor2();  // Wczytaj drugi kursor
    void SwitchCursor(int cursorIndex);  // Przełącz kursor (0 = domyślny, 1 = cursor2)

private:
    Engine(int width, int height);
    ~Engine();
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    
    GLFWwindow* window = nullptr;
    int screenWidth;
    int screenHeight;
    
    SceneManager sceneManager;
    Game game;
    ConsoleRenderer console;
    Text textRenderer;
    
    int currentFPS = 0;
    
    // DODANE: Wskaźnik do niestandardowego kursora
    GLFWcursor* customCursor = nullptr;
    GLFWcursor* customCursor2 = nullptr;  // DODANE: Drugi kursor
    int activeCursor = 0;
};

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);