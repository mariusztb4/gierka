#pragma once
#include "Scenes.h"
#include "Camera.h"
#include "Player.h"
#include "Shader.h"
#include "Mesh.h"
#include "Terrain.h"
#include "UI_System/HUB.h"
#include "UI_System/UIRenderer.h"
#include "GameObject.h"
#include "LightingSystem.h"
#include "Enemy.h"
#include "HealthBar3D.h"
#include "EnemyHoverDetector.h"  // DODANE
#include "UI_System/PauseMenu.h"
#include <vector>

class Game : public Scene
{
public:
    Game(SceneManager* sm, int width, int height);
    ~Game();

    void OnEnter() override;
    void OnExit() override;
    void Update(float dt) override;
    void Render() override;

    static void PreloadResources(class Loading* loadingScreen);
    static void SetupLoadingTasks(Loading* loading);

private:
    SceneManager* sceneManager = nullptr;
    Camera camera;
    Player player2;
    Shader modelShader;
    Shader terrainShader;
    Terrain terrain;
    HUB hub;
    UIRenderer uiRenderer;

    unsigned int playerTexture1 = 0;
    unsigned int emissiveTexture = 0;

    float heightOffset = 3.0f;
    float backOffset = 25.0f;

    // Obs³uga myszy
    bool isMousePressed = false;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    bool firstMouse = true;

    std::vector<GameObject*> gameObjects;
    Mesh* cubeMesh = nullptr;

    LightingSystem* lightingSystem = nullptr;

    std::vector<Enemy*> enemies;
    HealthBar3D healthBar3D;
    
    // DODANE: Detektor najechania na enemy
    EnemyHoverDetector hoverDetector;

    PauseMenu pauseMenu;
    bool escKeyPressed = false;

    int m_Width;
    int m_Height;

    Text textRenderer;
};

