#include "Loading.h"
#include "Engine.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

Loading::Loading(SceneManager* sm)
    : sceneManager(sm), loadbg(sm), nextScene(SceneType::MainMenu),
      progress(0.0f), currentTaskIndex(0), loadingComplete(false),
      loadingStarted(false), taskTimer(0.0f), taskDelay(0.05f),
      u_proj(1.0f), sceneChangeRequested(false)
{
    std::cout << "[Loading] Constructor - Instance: " << this << std::endl;
}

Loading::~Loading()
{
    std::cout << "[Loading] Destructor - Instance: " << this << std::endl;
}

void Loading::OnEnter()
{
    std::cout << "[Loading] OnEnter() - Instance: " << this << ", Tasks: " << tasks.size() << std::endl;
    
    loadbg.OnEnter();

    progress = 0.0f;
    currentTaskIndex = 0;
    loadingComplete = false;
    loadingStarted = false;
    sceneChangeRequested = false;
    taskTimer = 0.0f;

    // Zamiast u¿ywaæ niezdefiniowanych m_Width i m_Height, pobierz rozmiar ekranu z SceneManager
    int screenWidth = sceneManager->GetScreenWidth();
    int screenHeight = sceneManager->GetScreenHeight();

    barX = (screenWidth - barWidth) / 2.0f;
    barY = (screenHeight - barHeight) / 2.0f - 50.0f;

    shader.LoadFromFiles("shaders/ui.vert", "shaders/ui.frag");
    uiShader = shader.ID;

    if (uiShader == 0)
        std::cerr << "[Loading] ERROR: shader failed!\n";

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

    u_proj = glm::ortho(0.0f, (float)screenWidth, 0.0f, (float)screenHeight);
}

void Loading::OnExit()
{
    std::cout << "[Loading] OnExit() - Instance: " << this << std::endl;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    tasks.clear();
}

void Loading::AddTask(const std::string& name, std::function<void()> task)
{
    tasks.push_back({ name, task });
}

void Loading::ClearTasks()
{
    tasks.clear();
    currentTaskIndex = 0;
    progress = 0.0f;
    loadingComplete = false;
    sceneChangeRequested = false;
}

void Loading::StartLoading(SceneType targetScene)
{
    std::cout << "[Loading] StartLoading() - Instance: " << this << ", Target: " << (int)targetScene << ", Tasks: " << tasks.size() << std::endl;
    
    nextScene = targetScene;
    loadingStarted = true;
    currentTaskIndex = 0;
    progress = 0.0f;
    loadingComplete = false;
    sceneChangeRequested = false;
    taskTimer = 0.0f;
}

void Loading::ProcessNextTask()
{
    if (currentTaskIndex >= static_cast<int>(tasks.size()))
    {
        std::cout << "[Loading] Tasks COMPLETE - Instance: " << this << std::endl;
        loadingComplete = true;
        progress = 1.0f;
        return;
    }

    currentTaskName = tasks[currentTaskIndex].name;
    tasks[currentTaskIndex].task();

    currentTaskIndex++;
    progress = static_cast<float>(currentTaskIndex) / static_cast<float>(tasks.size());

    taskTimer = 0.0f;
}

void Loading::Update(float dt)
{
    // Jeœli ju¿ za¿¹dano zmiany sceny, nic nie rób
    if (sceneChangeRequested)
        return;

    // Auto-start
    if (!loadingStarted && !tasks.empty())
    {
        std::cout << "[Loading] Auto-start - Instance: " << this << std::endl;
        StartLoading(nextScene);
        return;
    }

    // £adowanie w toku
    if (loadingStarted && !loadingComplete)
    {
        taskTimer += dt;

        if (taskTimer >= taskDelay)
        {
            ProcessNextTask();
        }
        return;
    }

    // £adowanie zakoñczone - zmieñ scenê
    if (loadingComplete && progress >= 1.0f)
    {
        taskTimer += dt;
        
        if (taskTimer >= 0.5f)
        {
            std::cout << "[Loading] SWITCHING SCENE - Instance: " << this << " -> Scene: " << (int)nextScene << std::endl;
            sceneChangeRequested = true;
            sceneManager->ChangeScene(nextScene);
        }
    }
}

void Loading::Render()
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    loadbg.Render();

    if (uiShader == 0)
        return;

    glUseProgram(uiShader);

    int loc_proj = glGetUniformLocation(uiShader, "u_proj");
    int loc_pos = glGetUniformLocation(uiShader, "u_pos");
    int loc_size = glGetUniformLocation(uiShader, "u_size");
    int loc_col = glGetUniformLocation(uiShader, "u_color");
    int loc_radius = glGetUniformLocation(uiShader, "u_radius");

    glUniformMatrix4fv(loc_proj, 1, GL_FALSE, glm::value_ptr(u_proj));

    // T³o paska (ciemne) z zaokr¹glonymi rogami
    glUniform2f(loc_pos, barX, barY);
    glUniform2f(loc_size, barWidth, barHeight);
    glUniform4f(loc_col, 0.25f, 0.17f, 0.05f, 0.8f);
    glUniform1f(loc_radius, 5.0f);  // Promieñ zaokr¹glenia

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Progress bar (jasny) z zaokr¹glonymi rogami
    glUniform2f(loc_pos, barX + 2.0f, barY + 2.0f);
    glUniform2f(loc_size, (barWidth - 4.0f) * progress, barHeight - 4.0f);
    glUniform4f(loc_col, 1.0f, 0.7f, 0.2f, 1.0f);
    glUniform1f(loc_radius, 5.0f);  // Promieñ zaokr¹glenia

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
    
    glDisable(GL_BLEND);
}