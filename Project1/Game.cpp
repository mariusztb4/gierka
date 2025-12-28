#include "Game.h"
#include "TextureLoader.h"
#include "MeshFactory.h"
#include "GameResources.h"
#include <glad/glad.h>
#include <iostream>
#include "Engine.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "OBJLoader.h"
#include "Text.h"

Game::Game(SceneManager* sm, int width, int height)
    : sceneManager(sm),
      m_Width(width),
      m_Height(height),
      camera(),
      terrain(),
      hub(&uiRenderer),
      textRenderer(width, height), // Dodaj to pole!
      pauseMenu(&uiRenderer, &textRenderer) // Przekaż wskaźnik do textRenderer
{
    sceneManager = sm;
}

Game::~Game()
{
    for (auto obj : gameObjects)
    {
        delete obj;
    }
    gameObjects.clear();

    for (auto enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();

    // NIE usuwaj cubeMesh - należy do GameResources
    
    if (lightingSystem)
        delete lightingSystem;
}

void Game::OnEnter()
{
    std::cout << "[Game] OnEnter() START" << std::endl;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    int screenWidth = Engine::GetInstance(m_Width, m_Height).GetScreenWidth();
    int screenHeight = Engine::GetInstance(m_Width, m_Height).GetScreenHeight();

    camera.Aspect = (float)screenWidth / (float)screenHeight;
    camera.Distance = backOffset;
    camera.Pitch = 15.0f;
    camera.Yaw = 180.0f;

    GameResources& resources = GameResources::GetInstance();

    if (!resources.isLoaded)
    {
        std::cerr << "[Game] ERROR: Resources not loaded!" << std::endl;
        return;
    }

    std::cout << "[Game] Using pre-loaded resources..." << std::endl;

    modelShader = resources.modelShader;
    terrainShader = resources.terrainShader;

    terrain.Init(1000.0f, 1000.0f);

    player2.Init(resources.playerMesh, resources.playerTexture, resources.emissiveTexture);
    player2.Speed = 15.0f;
    
    // POPRAWIONE: Ustaw poziom ziemi dla gracza
    player2.groundLevel = -0.688395f;  // Teren jest na wysokości Y=0
    player2.Position.y = -0.688395f;  // ZMIENIONE: Obniż postać (było 0.0f)
    player2.isGrounded = true;
    
    // ALBO jeszcze niżej jeśli model jest wysoki:
    // player2.Position.y = -2.0f;

    cubeMesh = resources.cubeMesh;

    healthBar3D.Init();
    
    Enemy* enemy1 = new Enemy();
    enemy1->Init(cubeMesh, glm::vec3(0.6f, 0.0f, 0.2f), glm::vec3(-15.0f, 1.0f, 45.0f));
    enemy1->Scale = glm::vec3(1.0f);
    enemies.push_back(enemy1);
    
    Enemy* enemy2 = new Enemy();
    enemy2->Init(cubeMesh, glm::vec3(0.6f, 0.0f, 0.2f), glm::vec3(15.0f, 1.0f, 20.0f));
    enemy2->Scale = glm::vec3(1.2f);
    enemies.push_back(enemy2);

    GameObject* blueSphere = new GameObject();
    blueSphere->Init(MeshFactory::CreateSphere(32), glm::vec3(0.0f, 0.0f, 0.7f));
    blueSphere->Position = glm::vec3(0.0f, 1.0f, 35.0f);
    blueSphere->Scale = glm::vec3(0.8f);
    gameObjects.push_back(blueSphere);

    lightingSystem = new LightingSystem();
    lightingSystem->SetScreenSize(screenWidth, screenHeight);
    lightingSystem->Init();

    uiRenderer.Init(screenWidth, screenHeight);
    pauseMenu.Init(screenWidth, screenHeight);  // DODANE
    
	textRenderer.Init();
	textRenderer.LoadFont("fonts/verdana.ttf", 24);

    hub.SetHP(100, 100);
    hub.SetMana(50, 100);

    std::cout << "[Game] OnEnter() COMPLETE - All resources used from GameResources" << std::endl;
}

void Game::OnExit()
{
    std::cout << "[Game] OnExit() - cleaning up" << std::endl;
}

void Game::Update(float dt)
{
    GLFWwindow* window = Engine::GetInstance(m_Width, m_Height).GetWindow();
    
    // DODANE: Obsługa ESC dla pauzy
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        if (!escKeyPressed)
        {
            pauseMenu.SetPaused(!pauseMenu.IsPaused());
            escKeyPressed = true;
        }
    }
    else
    {
        escKeyPressed = false;
    }
    
    // DODANE: Sprawdź czy wyjść do menu lub zamknąć grę
    if (pauseMenu.ShouldExitToMenu())
    {
        pauseMenu.ResetFlags();
        sceneManager->ChangeScene(SceneType::MainMenu);  // użyj enum SceneType
        return;
    }
    
    if (pauseMenu.ShouldQuitGame())
    {
        glfwSetWindowShouldClose(window, true);
        return;
    }
    
    // DODANE: Update pauzy
    pauseMenu.Update(dt);
    
    // ZMIENIONE: Jeśli pauza, nie aktualizuj gry
    if (pauseMenu.IsPaused())
    {
        return;
    }
    
    bool consoleOpen = Engine::GetInstance(m_Width, m_Height).GetConsole().IsOpen();
    
    if (!consoleOpen)
    {
        player2.ProcessInput(window, dt, &camera);
        player2.Update(dt);

        // UPROSZCZONE: Użyj detektora hover
        int screenWidth = Engine::GetInstance(m_Width, m_Height).GetScreenWidth();
        int screenHeight = Engine::GetInstance(m_Width, m_Height).GetScreenHeight();
        
        hoverDetector.Update(window, camera, enemies, screenWidth, screenHeight);

        // Obsługa myszy (LPM)
        int mouseButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        
        if (mouseButtonState == GLFW_PRESS)
        {
            double currentMouseX, currentMouseY;
            glfwGetCursorPos(window, &currentMouseX, &currentMouseY);

            if (firstMouse)
            {
                lastMouseX = currentMouseX;
                lastMouseY = currentMouseY;
                firstMouse = false;
            }

            if (!isMousePressed)
            {
                lastMouseX = currentMouseX;
                lastMouseY = currentMouseY;
                isMousePressed = true;
            }
            else
            {
                float xoffset = static_cast<float>(currentMouseX - lastMouseX);
                float yoffset = static_cast<float>(currentMouseY - lastMouseY);

                lastMouseX = currentMouseX;
                lastMouseY = currentMouseY;

                camera.ProcessMouseMovement(xoffset, yoffset);
            }
        }
        else
        {
            isMousePressed = false;
            firstMouse = true;
        }
    }
    else
    {
        isMousePressed = false;
        firstMouse = true;
    }

    glm::vec3 cameraTarget = player2.Position + glm::vec3(0.0f, 3.0f, 0.0f);
    camera.UpdateCameraVectors(cameraTarget);

    for (auto obj : gameObjects)
    {
        obj->Update(dt);
    }

    for (auto enemy : enemies)
    {
        enemy->Update(dt);
    }

    hub.Update(dt);

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        for (auto enemy : enemies)
        {
            float dist = glm::distance(player2.Position, enemy->GetPosition());
            if (dist < 5.0f && enemy->IsAlive())
            {
                enemy->TakeDamage(10.0f);
                break;
            }
        }
    }
}

void Game::Render()
{
    // Oblicz macierze kamery RAZ na początku
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    glm::mat4 projectionMatrix = camera.GetProjectionMatrix();
    
    // ===== 1. SHADOW PASS - Renderuj cienie =====
    if (lightingSystem)
    {
        lightingSystem->BeginFrame();
        lightingSystem->RenderShadowPass(&player2, gameObjects, enemies);
        lightingSystem->EndFrame();
    }

    // ===== 2. NORMAL PASS - Renderuj scenę =====
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    // Renderuj teren - WYŁĄCZ CULLING
    glDisable(GL_CULL_FACE);
    
    terrainShader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    terrainShader.SetMat4("model", model);
    terrainShader.SetMat4("view", viewMatrix);
    terrainShader.SetMat4("projection", projectionMatrix);
    
    if (lightingSystem)
    {
        lightingSystem->ApplyLightingToShader(terrainShader, camera);
    }
    
    terrainShader.setInt("terrainTexture", 0);

    terrain.Render(viewMatrix, projectionMatrix, camera.Position);

    // WŁĄCZ Z POWROTEM CULLING dla obiektów
    glEnable(GL_CULL_FACE);

    // Renderuj obiekty z oświetleniem i cieniami
    modelShader.Use();
    modelShader.SetMat4("view", camera.GetViewMatrix());
    modelShader.SetMat4("projection", camera.GetProjectionMatrix());
    
    if (lightingSystem)
    {
        lightingSystem->ApplyLightingToShader(modelShader, camera);
    }

    // DODANE: Przekazanie pozycji i koloru światła do shadera
    modelShader.SetVec3("lightPos", lightingSystem->GetLightPosition());
    modelShader.SetVec3("lightColor", lightingSystem->GetLightColor());
    modelShader.SetVec3("viewPos", camera.Position);

    player2.Render(modelShader);

    for (auto obj : gameObjects)
    {
        obj->Render(modelShader);
    }

    // Renderuj przeciwników (bez paska życia tutaj)
    for (auto enemy : enemies)
    {
        if (enemy->IsAlive())
        {
            enemy->Render(modelShader);
        }
    }
    
    // PASKI ŻYCIA NA KOŃCU (przed UI)
    for (auto enemy : enemies)
    {
        if (enemy->IsAlive())
        {
            healthBar3D.Render(
                enemy->GetPosition(),
                enemy->GetHealthPercent(),
                camera.GetViewMatrix(),
                camera.GetProjectionMatrix()
            );
        }
    }

    glDisable(GL_DEPTH_TEST);
    hub.DrawUI();
    hub.DrawText();
    
    pauseMenu.Render();  // DODANE - renderuj pause menu na samym końcu
}