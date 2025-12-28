#pragma once
#include "UIRenderer.h"
#include <string>

// Dodaj forward declaration
class Text;

class PauseMenu
{
public:
    PauseMenu(UIRenderer* renderer, Text* textRenderer);
    ~PauseMenu() = default;

    void Init(int screenWidth, int screenHeight);
    void Update(float dt);
    void Render();
    
    bool IsPaused() const { return isPaused; }
    void SetPaused(bool paused) { isPaused = paused; }
    
    // Zwraca true jeœli gracz chce wyjœæ do menu
    bool ShouldExitToMenu() const { return exitToMenu; }
    bool ShouldQuitGame() const { return quitGame; }
    
    void ResetFlags() { exitToMenu = false; quitGame = false; }

private:
    UIRenderer* uiRenderer;
    Text* text; // Dodaj pole
    bool isPaused;
    bool exitToMenu;
    bool quitGame;
    
    int screenWidth;
    int screenHeight;
    
    // Przyciski
    enum class SelectedButton
    {
        Resume,
        MainMenu,
        Options,
        Quit
    };
    
    SelectedButton selectedButton;
    
    void HandleInput();
};