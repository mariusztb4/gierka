#pragma once
#include "Scenes.h"
#include "SceneManager.h"
#include "SceneType.h"
#include "Shader.h"
#include "LoadBG.h"
#include <functional>
#include <vector>
#include <string>

struct LoadingTask
{
    std::string name;
    std::function<void()> task;
};

class Loading : public Scene
{
public:
    void OnEnter() override;
    void OnExit() override;
    void Update(float dt) override;
    void Render() override;
    Loading(SceneManager* sm);
    ~Loading();

    void AddTask(const std::string& name, std::function<void()> task);
    void StartLoading(SceneType targetScene);
    void ClearTasks();
    
    int GetTaskCount() const { return static_cast<int>(tasks.size()); }

private:
    void ProcessNextTask();

    SceneManager* sceneManager;
    SceneType nextScene;

    // Progress tracking
    float progress;
    int currentTaskIndex;
    bool loadingComplete;
    bool loadingStarted;
    bool sceneChangeRequested; // DODAJ: Flaga czy zmiana sceny zosta³a wywo³ana

    std::vector<LoadingTask> tasks;
    std::string currentTaskName;

    // Timing
    float taskTimer;
    float taskDelay;

    // UI
    float barWidth = 600.0f;
    float barHeight = 30.0f;
    float barX;
    float barY;

    Shader shader;
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint uiShader = 0;
    glm::mat4 u_proj;

    LoadBG loadbg;
};