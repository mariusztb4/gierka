#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

struct Button
{
    const char* text;
    float x, y, width, height;

    float offsetY = 0.0f;

    GLuint VAO = 0;
    GLuint VBO = 0;

    std::string action;

    GLuint textureNormal = 0;
    GLuint textureHover = 0;
    bool hasHoverTexture = false;
	bool hasNormalTexture = false;
    bool invisibleNormal = false;

    static bool IsHovered(const Button& button, double mouseX, double mouseY);
    static bool IsClicked(const Button& button);
    static std::string GetText(const Button& button);
};

class SceneManager;

class Buttons
{
public:
    Buttons();
    ~Buttons();
    const float CANVAS_W = 1920.0f;
    const float CANVAS_H = 1080.0f;

    void SetWindow(GLFWwindow* window) { m_window = window; }

    void AddButton(const Button& button);
    std::string Update();
    void RenderButtons();
    void RenderButton(const Button& button);
    void RebuildButtons(int sw, int sh);
    void ClearButtons();
    
    // DODAJ: Funkcja do obliczania przeskalowanych wspó³rzêdnych
    Button GetTransformedButton(const Button& button, int screenWidth, int screenHeight) const;

    static GLFWwindow* m_window;

private:
    std::vector<Button> m_buttons;
    GLuint newGameButtonVAO;
	GLuint optionsButtonVAO;
	GLuint creditsButtonVAO;
	GLuint exitButtonVAO;
	GLuint m_uiProgram = 0;
    
    int m_hoveredButtonIndex = -1;
};