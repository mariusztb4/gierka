#include "Loading.h"
#include "Engine.h"
#include <thread>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

LoadGame::LoadGame(SceneManager* sm)
    : sceneManager(sm)
{

    progress = 0.0f;
}

LoadGame::~LoadGame()
{
}

void LoadGame::OnEnter() {

    std::cout << "LoadGame OnEnter - start" << std::endl;

    progress = 0.0f;
    progressSpeed = 0.2f;  // prędkość ładowania
    barWidth = 600.0f;
    barHeight = 30.0f;
    barX = (Engine::GetInstance().screenWidth - barWidth) / 2.0f;
    barY = (Engine::GetInstance().screenHeight - barHeight) / 2.0f;


    // Wczytaj shadery
    shader.LoadFromFiles("shaders/ui.vert", "shaders/ui.frag");
    uiShader = shader.ID;

    if (uiShader == 0)
        std::cerr << "Błąd: shader nie został poprawnie załadowany!\n";

    // Projekcja ortograficzna 2D
    u_proj = glm::ortho(0.0f, (float)Engine::GetInstance().screenWidth,
        0.0f, (float)Engine::GetInstance().screenHeight);

    progress = 0.0f;

}

void LoadGame::OnExit() {
    glDeleteVertexArrays(1, &VAO);
}

void LoadGame::InitializeLG() {

    if (uiInitialized) return;

    unsigned int VBO;

    // VAO + VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    float vertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &VBO);

    uiInitialized = true;
}

void LoadGame::Update(float dt)
{
    if (currentStep == STEP_DONE) return;

    switch (currentStep)
    {
    case STEP_PREPARE:
        // Krok przygotowawczy (np. alokacja pamięci)
        stepProgress += dt * 0.5f;  // szybko
        progress = 0.2f * stepProgress;
        if (stepProgress >= 1.0f)
        {
            currentStep = STEP_LOAD_TEXTURE_DATA;
            stepProgress = 0.0f;
        }
        break;

    case STEP_LOAD_TEXTURE_DATA:
        // Tu możesz naprawdę załadować dane tekstury z dysku (stb_image)
        // Przykład symulacji:
        stepProgress += dt * 0.3f;
        progress = 0.2f + 0.5f * stepProgress;  // 20% - 70%
        if (stepProgress >= 1.0f)
        {
            // Tu możesz zapisać dane tekstury do tymczasowej zmiennej w LoadGame
            currentStep = STEP_SIMULATE_HEAVY_WORK;
            stepProgress = 0.0f;
        }
        break;

    case STEP_SIMULATE_HEAVY_WORK:
        // Symulacja ciężkich operacji (np. generowanie heightmapy, ładowanie modeli)
        stepProgress += dt * 0.2f;  // wolno
        progress = 0.7f + 0.3f * stepProgress;  // 70% - 100%
        if (stepProgress >= 1.0f)
        {
            currentStep = STEP_DONE;
            progress = 1.0f;
        }
        break;

    case STEP_DONE:
        break;
    }

    // Po zakończeniu wszystkich kroków – przejdź do gry
    if (currentStep == STEP_DONE)
    {
        sceneManager->ChangeScene(SceneType::Game);
    }
}

void LoadGame::Render() {

    //glClearColor(0.2f, 0.6f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    InitializeLG();

    if (uiShader == 0 || !uiInitialized) return;

    glUseProgram(uiShader);

    // Poprawiona nazwa uniformu: "proj"
    int loc_proj = glGetUniformLocation(uiShader, "u_proj");
    int loc_pos = glGetUniformLocation(uiShader, "u_pos");
    int loc_size = glGetUniformLocation(uiShader, "u_size");
    int loc_col = glGetUniformLocation(uiShader, "u_color");

    if (loc_proj == -1) std::cerr << "Uniform 'proj' nie istnieje w shaderze!\n";

    glUniformMatrix4fv(loc_proj, 1, GL_FALSE, glm::value_ptr(u_proj));
    glUniform2f(loc_pos, barX, barY);
    glUniform2f(loc_size, barWidth * progress, barHeight);
    glUniform4f(loc_col, 0.0f, 0.4f, 0.0f, 1.0f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);

    // Sprawdź błędy OpenGL
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
  //      std::cerr << "OpenGL Error w Loading::Render: " << err << std::endl;
    }
}