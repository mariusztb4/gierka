#include "PauseMenu.h"
#include "../Engine.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>

PauseMenu::PauseMenu(UIRenderer* renderer, Text* textRenderer)
    : uiRenderer(renderer),
      text(textRenderer),
      isPaused(false),
      exitToMenu(false),
      quitGame(false),
      selectedButton(SelectedButton::Resume),
      screenWidth(0),
      screenHeight(0)
{
}

void PauseMenu::Init(int width, int height)
{
    screenWidth = width;
    screenHeight = height;
}

void PauseMenu::Update(float dt)
{
    if (!isPaused)
        return;
        
    HandleInput();
}

void PauseMenu::HandleInput()
{
    Engine& engine = Engine::GetInstance(screenWidth, screenHeight);
    GLFWwindow* window = engine.GetWindow();
    
    static bool upKeyPressed = false;
    static bool downKeyPressed = false;
    static bool enterKeyPressed = false;
    
    // Nawigacja w górê
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || 
        glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        if (!upKeyPressed)
        {
            int current = static_cast<int>(selectedButton);
            current--;
            if (current < 0) current = 3;
            selectedButton = static_cast<SelectedButton>(current);
            upKeyPressed = true;
        }
    }
    else
    {
        upKeyPressed = false;
    }
    
    // Nawigacja w dó³
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || 
        glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        if (!downKeyPressed)
        {
            int current = static_cast<int>(selectedButton);
            current++;
            if (current > 3) current = 0;
            selectedButton = static_cast<SelectedButton>(current);
            downKeyPressed = true;
        }
    }
    else
    {
        downKeyPressed = false;
    }
    
    // Potwierdzenie wyboru
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        if (!enterKeyPressed)
        {
            switch (selectedButton)
            {
                case SelectedButton::Resume:
                    isPaused = false;
                    break;
                case SelectedButton::MainMenu:
                    exitToMenu = true;
                    isPaused = false;
                    break;
                case SelectedButton::Options:

                    break;
                case SelectedButton::Quit:
                    quitGame = true;
                    break;
            }
            enterKeyPressed = true;
        }
    }
    else
    {
        enterKeyPressed = false;
    }
}

void PauseMenu::Render()
{
    if (!isPaused)
        return;

    float centerX = screenWidth / 2.0f;
    float centerY = screenHeight / 2.0f;

    // Pó³przezroczyste t³o
    uiRenderer->DrawQuad(0, 0, static_cast<float>(screenWidth), static_cast<float>(screenHeight), glm::vec4(0.0f, 0.0f, 0.0f, 0.7f));

    // Tytu³
    text->RenderText("PAUZA", centerX - 80, centerY - 150, 2.0f, glm::vec3(1.0f));

    // Przyciski
    glm::vec3 normalColor(0.8f, 0.8f, 0.8f);
    glm::vec3 selectedColor(1.0f, 1.0f, 0.0f);
    
    float buttonY = centerY - 50;
    float buttonSpacing = 60.0f;
    
    // Resume
    glm::vec3 resumeColor = (selectedButton == SelectedButton::Resume) ? selectedColor : normalColor;
    std::string resumeText = (selectedButton == SelectedButton::Resume) ? "> Resume <" : "Resume";
    text->RenderText(resumeText, centerX - 70, buttonY, 1.5f, resumeColor);
    
    // Main Menu
    buttonY += buttonSpacing;
    glm::vec3 menuColor = (selectedButton == SelectedButton::MainMenu) ? selectedColor : normalColor;
    std::string menuText = (selectedButton == SelectedButton::MainMenu) ? "> Main Menu <" : "Main Menu";
    text->RenderText(menuText, centerX - 120, buttonY, 1.5f, menuColor);
    
    // Options
    buttonY += buttonSpacing;
    glm::vec3 optionsColor = (selectedButton == SelectedButton::Options) ? selectedColor : normalColor;
    std::string optionsText = (selectedButton == SelectedButton::Options) ? "> Options <" : "Options";
    text->RenderText(optionsText, centerX - 70, buttonY, 1.5f, optionsColor);
    
    // Quit
    buttonY += buttonSpacing;
    glm::vec3 quitColor = (selectedButton == SelectedButton::Quit) ? selectedColor : normalColor;
    std::string quitText = (selectedButton == SelectedButton::Quit) ? "> Exit <" : "Exit";
    text->RenderText(quitText, centerX - 70, buttonY, 1.5f, quitColor);
}